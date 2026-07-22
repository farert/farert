// repro.cpp : add() -> removeTail() -> add() の対称性検証ドライバ
// 経路に区間を追加し、removeTail()で戻し、同じ区間を再addしたときに
// 戻り値・経路文字列が一致するかを総当たりで検証する。
#include <stdafx.h>
#include <cstdio>
#include <string>
#include <vector>

int g_tax = 10;

static int tested = 0;
static int mismatches = 0;
static bool verbose = false;

// 1ケース検証:
//   r.add(s1); r.add(l1,s2)=rcA; r.add(l2,s3)=rcB1; removeTail(); r.add(l2,s3)=rcB2
//   rcB1 と rcB2、経路文字列を比較
static bool test_case(int s1, int l1, int s2, int l2, int s3)
{
    Route r;
    r.add(s1);
    int rcA = r.add(l1, s2);
    if (rcA != ADDRC_OK) {
        return true;    // 前提区間が張れないのでスキップ
    }
    int rcB1 = r.add(l2, s3);
    tstring script1 = r.route_script();
    bool end1 = r.isEnd();

    if (rcB1 < 0) {
        // 初回で失敗する経路は対象外(追加されていないのでremoveTailしない)
        return true;
    }
    tested++;

    r.removeTail();
    int rcB2 = r.add(l2, s3);
    tstring script2 = r.route_script();
    bool end2 = r.isEnd();

    bool ok = (rcB1 == rcB2) && (script1 == script2) && (end1 == end2);
    if (!ok || verbose) {
        printf("%s rc1=%d rc2=%d end=%d/%d\n    1st: %s\n    2nd: %s\n    [%s(%d) %s(%d) %s(%d) %s(%d) %s(%d)]\n",
               ok ? "OK  " : "NG**",
               rcB1, rcB2, (int)end1, (int)end2,
               script1.c_str(), script2.c_str(),
               RouteUtil::StationName(s1).c_str(), s1,
               RouteUtil::LineName(l1).c_str(), l1,
               RouteUtil::StationName(s2).c_str(), s2,
               RouteUtil::LineName(l2).c_str(), l2,
               RouteUtil::StationName(s3).c_str(), s3);
        fflush(stdout);
    }
    if (!ok) {
        mismatches++;
    }
    return ok;
}

// 路線の端点駅(sales_km最小/最大)を返す
static void line_edges(int line_id, int* e1, int* e2)
{
    static const char sql[] =
        "select (select station_id from t_lines where line_id=?1 order by sales_km asc limit 1),"
        "       (select station_id from t_lines where line_id=?1 order by sales_km desc limit 1)";
    DBO dbo = DBS::getInstance()->compileSql(sql);
    *e1 = *e2 = 0;
    if (dbo.isvalid()) {
        dbo.setParam(1, line_id);
        if (dbo.moveNext()) {
            *e1 = dbo.getInt(0);
            *e2 = dbo.getInt(1);
        }
    }
}

// 全分岐駅×路線ペアで総当たり
static void brute()
{
    // 分岐駅一覧
    std::vector<int> jcts;
    {
        static const char sql[] = "select station_id from t_jct order by id";
        DBO dbo = DBS::getInstance()->compileSql(sql);
        while (dbo.isvalid() && dbo.moveNext()) {
            jcts.push_back(dbo.getInt(0));
        }
    }
    printf("junctions: %d\n", (int)jcts.size());

    for (size_t ji = 0; ji < jcts.size(); ji++) {
        int j = jcts[ji];
        // この駅を通る路線
        std::vector<int> lines;
        {
            static const char sql[] = "select line_id from t_lines where station_id=?1";
            DBO dbo = DBS::getInstance()->compileSql(sql);
            if (dbo.isvalid()) {
                dbo.setParam(1, j);
                while (dbo.moveNext()) {
                    lines.push_back(dbo.getInt(0));
                }
            }
        }
        for (size_t a = 0; a < lines.size(); a++) {
            int l1 = lines[a];
            int s1a, s1b;
            line_edges(l1, &s1a, &s1b);
            for (size_t b = 0; b < lines.size(); b++) {
                if (a == b) continue;
                int l2 = lines[b];
                int s3a, s3b;
                line_edges(l2, &s3a, &s3b);
                const int s1s[2] = { s1a, s1b };
                const int s3s[2] = { s3a, s3b };
                for (int x = 0; x < 2; x++) {
                    for (int y = 0; y < 2; y++) {
                        int s1 = s1s[x];
                        int s3 = s3s[y];
                        if (s1 == 0 || s3 == 0 || s1 == j || s3 == j || s1 == s3) continue;
                        test_case(s1, l1, j, l2, s3);
                    }
                }
            }
        }
        if ((ji % 50) == 0) {
            fprintf(stderr, "@@@ progress %d/%d tested=%d mismatch=%d\n",
                    (int)ji, (int)jcts.size(), tested, mismatches);
        }
    }
    printf("done. tested=%d mismatches=%d\n", tested, mismatches);
}

// 乱択で経路を伸ばしながら add/removeTail の churn を行い、
// 各ステップで「素の再構築経路(setup_route)」との等価性を検証する。
// removeTail が隠れ状態(jct_mask/route_flag)を正しく巻き戻せていなければ、
// 同じ可視経路から同じ区間を足しても素の経路と結果が食い違う。
static void fuzz(unsigned seed, int iterations, int maxdepth)
{
    srand(seed);
    std::vector<int> all_stations;
    {
        static const char sql[] = "select distinct station_id from t_lines";
        DBO dbo = DBS::getInstance()->compileSql(sql);
        while (dbo.isvalid() && dbo.moveNext()) {
            all_stations.push_back(dbo.getInt(0));
        }
    }
    // 大阪環状線と接続路線の駅プール(ループ状態機械を重点的に叩く)
    std::vector<int> osaka_pool;
    {
        static const char sql[] =
            "select distinct station_id from t_lines where line_id in ("
            " select distinct l2.line_id from t_lines l1"
            " join t_lines l2 on l2.station_id=l1.station_id"
            " where l1.line_id=(select rowid from t_line where name='大阪環状線'))";
        DBO dbo = DBS::getInstance()->compileSql(sql);
        while (dbo.isvalid() && dbo.moveNext()) {
            osaka_pool.push_back(dbo.getInt(0));
        }
    }
    printf("stations: %d osaka_pool: %d seed=%u\n",
           (int)all_stations.size(), (int)osaka_pool.size(), seed);

    int ng_modified = 0;   // 直前addが経路置換(isModified)由来の不一致
    int ng_pure = 0;       // 再add不一致(純粋追加)
    int ng_state = 0;      // 素の経路との等価性不一致(=churnによる状態破壊)

    for (int it = 0; it < iterations; it++) {
        int s0;
        if (!osaka_pool.empty() && (rand() % 4) == 0) {
            s0 = osaka_pool[rand() % osaka_pool.size()];
        } else {
            s0 = all_stations[rand() % all_stations.size()];
        }
        Route r;
        r.add(s0);
        std::string history = RouteUtil::StationName(s0);
        bool churned = false;   // これまでに removeTail を行ったか

        for (int depth = 0; depth < maxdepth; depth++) {
            int tail = r.arriveStationId();
            if (tail <= 0 || r.isEnd()) break;
            std::vector<int> lines;
            {
                static const char sql[] = "select line_id from t_lines where station_id=?1";
                DBO dbo = DBS::getInstance()->compileSql(sql);
                if (dbo.isvalid()) {
                    dbo.setParam(1, tail);
                    while (dbo.moveNext()) lines.push_back(dbo.getInt(0));
                }
            }
            if (lines.empty()) break;
            int l = lines[rand() % lines.size()];
            std::vector<int> stations;
            {
                static const char sql[] =
                    "select l.station_id, (l.lflg>>15)&1 from t_lines l where l.line_id=?1";
                DBO dbo = DBS::getInstance()->compileSql(sql);
                if (dbo.isvalid()) {
                    dbo.setParam(1, l);
                    while (dbo.moveNext()) {
                        stations.push_back(dbo.getInt(0));
                        if (dbo.getInt(1)) stations.push_back(dbo.getInt(0)); // 分岐駅は2票
                    }
                }
            }
            if (stations.empty()) break;
            int s = stations[rand() % stations.size()];
            if (s == tail) continue;

            // ---- 等価性プローブ: 現在の可視経路を素に再構築し、同じaddを比較 ----
            Route fresh;
            bool fresh_valid = false;
            tstring cur_script = r.route_script();
            if (churned && !cur_script.empty()) {
                fresh.setup_route(cur_script.c_str());
                fresh_valid = (fresh.route_script() == cur_script) &&
                              (fresh.isEnd() == r.isEnd());
            }

            int rc1 = r.add(l, s);
            if (fresh_valid) {
                int rcF = fresh.add(l, s);
                if ((rc1 != rcF) || (rc1 >= 0 && r.route_script() != fresh.route_script())) {
                    ng_state++;
                    printf("NGSTATE rc_churned=%d rc_fresh=%d it=%d depth=%d\n"
                           "    visible: %s\n    add: %s(%d) -> %s(%d)\n"
                           "    churned-> %s\n    fresh  -> %s\n    hist: %s\n",
                           rc1, rcF, it, depth,
                           cur_script.c_str(),
                           RouteUtil::LineName(l).c_str(), l,
                           RouteUtil::StationName(s).c_str(), s,
                           r.route_script().c_str(), fresh.route_script().c_str(),
                           history.c_str());
                    fflush(stdout);
                    break;
                }
            }
            if (rc1 < 0) continue;
            history += " +[" + std::string(RouteUtil::LineName(l)) + "]"
                     + std::string(RouteUtil::StationName(s));

            bool modified = r.isModified();
            tstring script1 = r.route_script();
            bool end1 = r.isEnd();
            tested++;

            int action = rand() % 10;
            if (action < 4) {
                // 40%: removeTail して同じ区間を再add(往復churn)
                r.removeTail();
                churned = true;
                int rc2 = r.add(l, s);
                tstring script2 = r.route_script();
                bool ok = (rc1 == rc2) && (script1 == script2) && (end1 == r.isEnd());
                if (!ok) {
                    if (modified) {
                        ng_modified++;
                    } else {
                        ng_pure++;
                        printf("NGPURE rc1=%d rc2=%d it=%d depth=%d\n"
                               "    hist: %s\n    1st: %s\n    2nd: %s\n",
                               rc1, rc2, it, depth, history.c_str(),
                               script1.c_str(), script2.c_str());
                        fflush(stdout);
                    }
                    break;
                }
            } else if (action < 6) {
                // 20%: removeTail して別方向へ(戻して継続)
                r.removeTail();
                churned = true;
                history += " -undo";
            }
            // 40%: そのまま継続
        }
        if ((it % 2000) == 0) {
            printf("@@@ progress it=%d tested=%d ng_pure=%d ng_state=%d ng_modified=%d\n",
                   it, tested, ng_pure, ng_state, ng_modified);
            fflush(stdout);
        }
    }
    printf("done. tested=%d ng_pure=%d ng_state=%d ng_modified=%d\n",
           tested, ng_pure, ng_state, ng_modified);
}

// 大阪環状線特化: ループ上の分岐駅に他路線から進入し、環状線区間を
// add→removeTail→再add で全組合せ検証
static void osaka()
{
    int oskk = RouteUtil::GetLineId("大阪環状線");
    // ループ上の全駅
    std::vector<int> loop_stations;
    {
        static const char sql[] = "select station_id from t_lines where line_id=?1";
        DBO dbo = DBS::getInstance()->compileSql(sql);
        if (dbo.isvalid()) {
            dbo.setParam(1, oskk);
            while (dbo.moveNext()) loop_stations.push_back(dbo.getInt(0));
        }
    }
    printf("loop stations: %d\n", (int)loop_stations.size());

    for (size_t bi = 0; bi < loop_stations.size(); bi++) {
        int board = loop_stations[bi];
        // boardを通る他路線
        std::vector<int> lines;
        {
            static const char sql[] = "select line_id from t_lines where station_id=?1";
            DBO dbo = DBS::getInstance()->compileSql(sql);
            if (dbo.isvalid()) {
                dbo.setParam(1, board);
                while (dbo.moveNext()) {
                    if (dbo.getInt(0) != oskk) lines.push_back(dbo.getInt(0));
                }
            }
        }
        for (size_t li = 0; li < lines.size(); li++) {
            int l1 = lines[li];
            int e1, e2;
            line_edges(l1, &e1, &e2);
            const int starts[2] = { e1, e2 };
            for (int x = 0; x < 2; x++) {
                int s1 = starts[x];
                if (s1 == 0 || s1 == board) continue;
                for (size_t ti = 0; ti < loop_stations.size(); ti++) {
                    int target = loop_stations[ti];
                    if (target == board || target == s1) continue;
                    test_case(s1, l1, board, oskk, target);
                }
            }
        }
    }
    printf("done. tested=%d mismatches=%d\n", tested, mismatches);
}

// 経路ファイルの各経路について、末尾からk段 removeTail して同じ区間列を
// 再addし、元の経路・状態に戻るかを検証する
static void undo_file(const char* path)
{
    FILE* fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "can't open %s\n", path);
        return;
    }
    char buf[4096];
    int lineno = 0;
    int skipped = 0;
    while (fgets(buf, sizeof buf, fp)) {
        lineno++;
        size_t len = strlen(buf);
        while (len && (unsigned char)buf[len - 1] <= ' ') buf[--len] = '\0';
        if (!len) continue;

        Route r;
        r.setup_route(buf);
        const std::vector<RouteItem>& items = r.routeList();
        if (items.size() < 3) { skipped++; continue; }   // 2区間以上のみ
        tstring script0 = r.route_script();
        bool end0 = r.isEnd();

        std::vector<std::pair<int,int> > segs;
        for (size_t i = 1; i < items.size(); i++) {
            segs.push_back(std::make_pair((int)items[i].lineId, (int)items[i].stationId));
        }
        for (size_t k = 1; k < segs.size(); k++) {
            Route w;
            w.setup_route(buf);
            if (w.route_script() != script0) break;   // 再現不能(乱数要素なし、保険)
            for (size_t u = 0; u < k; u++) {
                w.removeTail();
            }
            int rc2 = 0;
            bool fail = false;
            size_t fail_at = 0;
            for (size_t u = segs.size() - k; u < segs.size(); u++) {
                rc2 = w.add(segs[u].first, segs[u].second);
                if (rc2 < 0) { fail = true; fail_at = u; break; }
            }
            tested++;
            if (fail || (w.route_script() != script0) || (w.isEnd() != end0)) {
                mismatches++;
                std::string detail;
                if (fail) {
                    detail = " failed at add(" + std::string(RouteUtil::LineName(segs[fail_at].first))
                           + "," + std::string(RouteUtil::StationName(segs[fail_at].second)) + ")";
                }
                printf("NGUNDO line=%d k=%d rc=%d%s\n    orig: %s\n    got : %s\n",
                       lineno, (int)k, rc2, detail.c_str(),
                       script0.c_str(), w.route_script().c_str());
                fflush(stdout);
            }
        }
    }
    fclose(fp);
    printf("done. tested=%d mismatches=%d skipped=%d\n", tested, mismatches, skipped);
}

int main(int argc, char** argv)
{
    const char* dbpath = getenv("farertDB");
    if (!dbpath || !DBS::getInstance()->open(dbpath)) {
        fprintf(stderr, "can't open db\n");
        return 1;
    }
    if (argc == 2 && std::string(argv[1]) == "-osaka") {
        osaka();
        return 0;
    }
    if (argc == 3 && std::string(argv[1]) == "-undo") {
        undo_file(argv[2]);
        return 0;
    }
    // -diff "full route script": (A)末尾1区間を除いた経路を素に構築 と
    //                            (B)フル経路を構築後 removeTail した状態 を
    //                            経路末尾RouteItemのflag/isEnd で比較
    // -flags "S1 L1 S2 L2 S3": 各操作後に route_script と会社線フラグを表示し、
    //   add(L2,S3) 前の状態(B0) と removeTail 後の状態(B1) を比較する
    if (argc == 7 && std::string(argv[1]) == "-flags") {
        int s1 = RouteUtil::GetStationId(argv[2]);
        int l1 = RouteUtil::GetLineId(argv[3]);
        int s2 = RouteUtil::GetStationId(argv[4]);
        int l2 = RouteUtil::GetLineId(argv[5]);
        int s3 = RouteUtil::GetStationId(argv[6]);
        auto dump = [](const char* tag, Route& r) {
            RouteFlag f = r.getRouteFlag();
            printf("%-18s %-40s | check=%d pass=%d da=%d begin=%d end=%d term=%d\n",
                   tag, r.route_script().c_str(),
                   f.compncheck, f.compnpass, f.compnda,
                   f.compnbegin, f.compnend, f.compnterm);
        };
        Route r;
        r.add(s1);                 dump("add(begin)", r);
        r.add(l1, s2);             dump("add(l1,s2)", r);
        Route b0; b0.add(s1); b0.add(l1, s2);    // B0: 期待復元状態
        int rc1 = r.add(l2, s3);   printf("rc1=%d\n", rc1); dump("add(l2,s3)", r);
        r.removeTail();            dump("removeTail", r);
        printf("---- B0(expected) vs B1(removeTail'd) ----\n");
        dump("B0 expected", b0);
        dump("B1 actual  ", r);
        int rc2 = r.add(l2, s3);   printf("rc2=%d\n", rc2); dump("re-add(l2,s3)", r);
        return 0;
    }
    if (argc == 3 && std::string(argv[1]) == "-diff") {
        // (A) truncated fresh
        Route full;
        full.setup_route(argv[2]);
        const std::vector<RouteItem>& fi = full.routeList();
        printf("full: %s (n=%d, end=%d)\n", full.route_script().c_str(),
               (int)fi.size(), (int)full.isEnd());
        // 末尾1区間を削った可視経路文字列を作る
        std::string s = argv[2];
        // route_script はカンマ区切り: 末尾2トークン(",line,station")を落とす
        size_t p = s.rfind(',');
        p = (p == std::string::npos) ? p : s.rfind(',', p - 1);
        std::string truncated = (p == std::string::npos) ? s : s.substr(0, p);

        Route a;
        a.setup_route(truncated.c_str());
        const std::vector<RouteItem>& ai = a.routeList();

        Route b;
        b.setup_route(argv[2]);
        b.removeTail();
        const std::vector<RouteItem>& bi = b.routeList();

        printf("(A) fresh truncated: %s (n=%d, end=%d, modified=%d)\n",
               a.route_script().c_str(), (int)ai.size(), (int)a.isEnd(), (int)a.isModified());
        printf("(B) removeTail'd   : %s (n=%d, end=%d, modified=%d)\n",
               b.route_script().c_str(), (int)bi.size(), (int)b.isEnd(), (int)b.isModified());
        printf("(A) tail item: line=%d station=%d flag=0x%08x\n",
               ai.empty()?0:ai.back().lineId, ai.empty()?0:ai.back().stationId,
               ai.empty()?0:ai.back().flag);
        printf("(B) tail item: line=%d station=%d flag=0x%08x\n",
               bi.empty()?0:bi.back().lineId, bi.empty()?0:bi.back().stationId,
               bi.empty()?0:bi.back().flag);
        // 末尾区間を両者へ再add して rc を比較
        int line_last = fi.back().lineId;   // full の末尾 lineId (=長崎線(長与経由))
        int stat_last = fi.back().stationId;
        printf("re-add seg: line=%d(%s) station=%d(%s)\n",
               line_last, RouteUtil::LineName(line_last).c_str(),
               stat_last, RouteUtil::StationName(stat_last).c_str());
        int rca = a.add(line_last, stat_last);
        int rcb = b.add(line_last, stat_last);
        printf("(A) re-add rc=%d -> %s\n", rca, a.route_script().c_str());
        printf("(B) re-add rc=%d -> %s\n", rcb, b.route_script().c_str());
        return 0;
    }
    if (argc == 2 && std::string(argv[1]) == "-brute") {
        brute();
        return 0;
    }
    if (argc >= 2 && std::string(argv[1]) == "-fuzz") {
        unsigned seed = (argc >= 3) ? (unsigned)atoi(argv[2]) : 1;
        int iters = (argc >= 4) ? atoi(argv[3]) : 20000;
        fuzz(seed, iters, 7);
        return 0;
    }
    if (argc == 6) {
        verbose = true;
        int s1 = RouteUtil::GetStationId(argv[1]);
        int l1 = RouteUtil::GetLineId(argv[2]);
        int s2 = RouteUtil::GetStationId(argv[3]);
        int l2 = RouteUtil::GetLineId(argv[4]);
        int s3 = RouteUtil::GetStationId(argv[5]);
        printf("ids: %d %d %d %d %d\n", s1, l1, s2, l2, s3);
        if (s1 <= 0 || l1 <= 0 || s2 <= 0 || l2 <= 0 || s3 <= 0) {
            fprintf(stderr, "name resolve error\n");
            return 1;
        }
        test_case(s1, l1, s2, l2, s3);
        return 0;
    }
    fprintf(stderr, "usage: repro -brute | repro S1 L1 S2 L2 S3\n");
    return 1;
}
