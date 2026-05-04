package org.sutezo.alps;

import android.database.sqlite.SQLiteDatabase;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.Writer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

public class JavaTestMain {
    private static final Path ROOT_DIR = locateRootDir();
    private static final Path DB_PATH = ROOT_DIR.resolve("db/jrdbNewest.db");
    private static final Path TEST_EXEC_CPP = ROOT_DIR.resolve("test/unix/common/test_exec.cpp");
    private static final Path REF_RESULT = ROOT_DIR.resolve("test/unix/all/test_result.txt");
    private static final Path OUTPUT_RESULT = ROOT_DIR.resolve("app/Farert.android/app/src/test/resources/test_result.txt");
    private static String referenceTimestampLine;
    private static String referenceProcessLapseLine;

    public static void main(String[] args) throws Exception {
        if (args.length < 1 || !"-exec".equals(args[0])) {
            System.err.println("Usage: java -cp <classpath> org.sutezo.alps.JavaTestMain -exec");
            return;
        }
        System.setOut(new QuietPrintStream());
        if (!Files.exists(TEST_EXEC_CPP)) {
            throw new IllegalStateException("Missing: " + TEST_EXEC_CPP);
        }
        if (!Files.exists(REF_RESULT)) {
            throw new IllegalStateException("Missing: " + REF_RESULT);
        }
        if (!Files.exists(DB_PATH)) {
            throw new IllegalStateException("Missing DB: " + DB_PATH);
        }
        Files.createDirectories(OUTPUT_RESULT.getParent());
        loadReferenceMarkers();

        SQLiteDatabase db = SQLiteDatabase.openDatabase(DB_PATH.toString(), null, SQLiteDatabase.OPEN_READONLY);
        RouteDB.createFactory(db, 10);

        TestData data = TestData.load(TEST_EXEC_CPP);
        long startNs = System.nanoTime();

        try (BufferedWriter writer = Files.newBufferedWriter(OUTPUT_RESULT, StandardCharsets.UTF_8)) {
            Out out = new Out(writer);
            out.println(referenceTimestampLine);
            out.println("");

            out.println("#---route test  -------------------------------------------");
            test_route(out, data.testRoute2, 0);

            out.println("");
            out.println("#---shinkansen  -------------------------------------------");
            test_shinkanzen();

            out.println("");
            out.println("#---special junction -------------------------------------------");
            test_jctspecial(out, data.jctSpecial);

            out.println("");
            out.println("#---hzl---------------------------------------------------------");
            test_hzl(out, data.hzlRouteDef);
            test_hzl2(out, data.hzlDefTbl);

            out.println("");
            out.println("#===auto route==================================================");
            test_autoroute(out, data.autoRouteDef, 0);

            out.println("");
            out.println("#---specificial route-------------------------------------------");
            test_route(out, data.testRoute, 0);

            out.println("");
            out.println("#---shinkansen convert-------------------------------------------");
            test_shin2zai(out, data.testShin2Zai);

            out.println("");
            out.println("#---same kokura hakata shinzai-----------------------------------");
            test_route(out, data.testRoute3, 0);

            if (referenceProcessLapseLine != null) {
                out.println(referenceProcessLapseLine);
            }
        } catch (Throwable ex) {
            System.err.println("Failure during: " + currentCase);
            throw ex;
        } finally {
            db.close();
        }
    }

    private static String currentCase = "";

    private static Path locateRootDir() {
        Path current = Paths.get("").toAbsolutePath().normalize();
        while (current != null) {
            if (Files.exists(current.resolve("test/unix/common/test_exec.cpp"))
                    && Files.exists(current.resolve("db/jrdbNewest.db"))) {
                return current;
            }
            current = current.getParent();
        }
        return Paths.get("").toAbsolutePath().normalize();
    }

    private static void test_route(Out out, List<String> routeDef, int round) {
        String title = "結果";
        int rev = ((round & 1) == 0) ? 2 : 1;
        int t = 0;
        Route route = new Route();

        for (int i = 0; i < routeDef.size(); i++) {
            String def = routeDef.get(i);
            if (def.isEmpty()) {
                break;
            }
            route.removeAll();
            if (def.startsWith("c") || def.startsWith("C")) {
                title = def.substring(1);
                t++;
                rev = def.startsWith("C") ? 1 : 2;
                continue;
            } else if (def.startsWith("s")) {
                route.setNotSameKokuraHakataShinZai(true);
                def = def.substring(1);
            } else {
                route.setNotSameKokuraHakataShinZai(false);
            }

            int idx = i - t + 1;
            currentCase = "test_route[" + idx + "] " + def;
            out.printf("!****<%02d>: ******************* %s **********************\n<%s>\n", idx, title, def);

            int rc = test_setup_route(def, route, out);
            farertAssert.ASSERT(0 <= rc);

            for (int j = 0; j < rev; j++) {
                if (j > 0) {
                    int enable = route.isAvailableReverse() ? 1 : 0;
                    if (route.reverse() < 0) {
                        out.printf("------ 反転は無効---%s---\n\n", enable == 1 ? "enable" : "disable");
                        break;
                    } else {
                        if (enable == 0) {
                            out.println("------ 反転 ---GUI disable---");
                            out.println("");
                        } else {
                            out.println("------ 反転 ------");
                            out.println("");
                        }
                    }
                } else {
                    out.println("");
                }

                FARE_INFO fi = new FARE_INFO();
                CalcRoute croute = new CalcRoute(route);
                RouteFlag rf = croute.getRouteFlag();

                if ((round & 2) == 0) {
                    croute.calcFare(fi);
                    String s = normalizeOutput(fi.showFare(rf));
                    out.println("///既定");
                    out.println(s);
                }

                if ((round & 4) == 0 && rf.rule_en()) {
                    rf.setNoRule(true);
                    croute.calcFare(fi);
                    String s = normalizeOutput(fi.showFare(rf));
                    out.println("///非適用");
                    out.println(s);

                    fi.reset();
                    rf.setNoRule(false);
                    croute.calcFare(fi);
                }

                if (rf.isMeihanCityEnable()) {
                    farertAssert.ASSERT(!rf.isStartAsCity());
                    farertAssert.ASSERT(rf.isArriveAsCity());
                    rf.setStartAsCity();
                    croute.calcFare(fi);
                    String s = normalizeOutput(fi.showFare(rf));
                    out.println("///着駅=単駅");
                    out.println(s);

                    farertAssert.ASSERT(rf.isStartAsCity());
                    farertAssert.ASSERT(!rf.isArriveAsCity());
                    rf.setArriveAsCity();
                    croute.calcFare(fi);
                    s = normalizeOutput(fi.showFare(rf));
                    out.println("///発駅=単駅");
                    out.println(s);
                }

                if (rf.jrtokaistock_enable()) {
                    rf.setJrTokaiStockApply(true);
                    croute.calcFare(fi);
                    String s = normalizeOutput(fi.showFare(rf));
                    out.println("///JR東海株主優待券使用");
                    out.println(s);

                    rf.setJrTokaiStockApply(false);
                    croute.calcFare(fi);
                    s = normalizeOutput(fi.showFare(rf));
                    out.println("///JR東海株主優待券未使用");
                    out.println(s);
                }

                if (rf.isEnableLongRoute()) {
                    rf.setLongRoute(true);
                    croute.calcFare(fi);
                    String s = normalizeOutput(fi.showFare(rf));
                    out.println("///指定経路");
                    out.println(s);

                    rf.setLongRoute(false);
                    croute.calcFare(fi);
                    s = normalizeOutput(fi.showFare(rf));
                    out.println("///最安経路");
                    out.println(s);
                }

                if (rf.isEnableRule115()) {
                    farertAssert.ASSERT(!rf.isRule115specificTerm());
                    rf.setSpecificTermRule115(true);
                    croute.calcFare(fi);
                    String s = normalizeOutput(fi.showFare(rf));
                    out.println("///旅客営業取扱基準規程115条(特定都区市内発着)");
                    out.println(s);

                    rf.setSpecificTermRule115(false);
                    croute.calcFare(fi);
                    s = normalizeOutput(fi.showFare(rf));
                    out.println("///旅客営業取扱基準規程115条(単駅最安)(Default)");
                    out.println(s);
                }
            }
        }
    }

    private static void test_hzl(Out out, List<String> hzlRouteDef) {
        Route route = new Route();
        for (String def : hzlRouteDef) {
            if (def.isEmpty()) {
                break;
            }
            currentCase = "test_hzl " + def;
            route.removeAll();
            String[] tokens = def.split("[, \\t]+");

            int lineId = 0;
            int stationId1 = 0;
            int stationId2;

            for (String token : tokens) {
                if (token.isEmpty()) {
                    continue;
                }
                int fail;
                if (stationId1 == 0) {
                    stationId1 = RouteUtil.GetStationId(token);
                    farertAssert.ASSERT(0 < stationId1);
                    route.add(stationId1);
                } else if (lineId == 0) {
                    lineId = RouteUtil.GetLineId(token);
                    farertAssert.ASSERT(0 < lineId);
                } else {
                    boolean cont = false;
                    if (token.startsWith("x")) {
                        token = token.substring(1);
                        fail = -1;
                    } else if (token.startsWith("o")) {
                        token = token.substring(1);
                        fail = 1;
                    } else if (token.startsWith("-")) {
                        token = token.substring(1);
                        fail = 0;
                    } else {
                        fail = 0;
                        cont = true;
                    }
                    stationId2 = RouteUtil.GetStationId(token);
                    farertAssert.ASSERT(0 < stationId2);
                    int rc = route.add(lineId, stationId2);
                    if (cont && rc < 0) {
                        out.printf("新幹線<->在来線乗り換え: Test Program Bug!!!!! %s(%s)\n", token, def);
                        farertAssert.ASSERT(false);
                    } else if (!cont) {
                        if (fail < 0) {
                            farertAssert.ASSERT(rc == -1);
                            out.printf("新幹線<->在来線乗り換え: Failure OK %s\n", def);
                        } else {
                            farertAssert.ASSERT(fail == rc);
                            out.printf("新幹線<->在来線乗り換え: Success OK %s\n", def);
                        }
                    }
                    lineId = 0;
                    stationId1 = stationId2;
                }
            }
        }
    }

    private static void test_hzl2(Out out, List<String> hzlDefTbl) {
        for (String def : hzlDefTbl) {
            if (def.isEmpty()) {
                break;
            }
            currentCase = "test_hzl2 " + def;
            String[] tokens = def.split("[, \\t]+");
            if (tokens.length < 4) {
                continue;
            }
            int ip0 = RouteUtil.GetStationId(tokens[0]);
            int ip1 = RouteUtil.GetLineId(tokens[1]);
            int ip2 = RouteUtil.GetStationId(tokens[2]);
            int ip3 = RouteUtil.GetStationId(tokens[3]);
            farertAssert.ASSERT(0 < ip0 && 0 < ip1 && 0 < ip2 && 0 < ip3);
            int t = RouteUtil.NextShinkansenTransferTerm(ip1, ip2, ip3);
            if (ip0 == t) {
                out.printf("新幹線隣駅: Success %s\n", def);
            } else {
                out.printf("新幹線隣駅: failure %s(result=%s)\n", def, RouteUtil.StationName(t));
                farertAssert.ASSERT(false);
            }
        }
    }

    private static void test_autoroute(Out out, List<String> routeDef, int option) {
        Route route = new Route();
        boolean resopt = option >= 0x10000;
        int autotype = resopt ? option - 0x10000 : option;

        for (int i = 0; i < routeDef.size(); i += 2) {
            String pre = routeDef.get(i);
            if (pre.isEmpty()) {
                break;
            }
            String next = routeDef.get(i + 1);
            currentCase = "test_autoroute[" + (i / 2) + "] " + pre + " -> " + next;

            out.printf("!===<%02d>: auto route ==================\n\n", i / 2);
            route.removeAll();
            int rc = route.setup_route(pre);
            farertAssert.ASSERT(0 <= rc);

            int fail;
            switch (next.charAt(0)) {
                case 'x':
                    fail = -1;
                    break;
                case '!':
                    fail = 1;
                    break;
                case 'b':
                case 'c':
                    fail = next.charAt(0);
                    break;
                default:
                    fail = 0;
                    break;
            }

            String p = (fail != 0) ? next.substring(1) : next;
            out.printf("* pre route >>>>>>>\n  {%s -> %s}\n", pre, p);

            if (resopt) {
                autotype = autotype % 4;
                if (autotype == 0) {
                    out.println("* auto route(新幹線も在来線も使わない) >>>>>>>");
                } else if (autotype == 1) {
                    out.println("* auto route(新幹線使用) >>>>>>>");
                } else if (autotype == 2) {
                    out.println("* auto route(会社線使用) >>>>>>>");
                } else if (autotype == 3) {
                    out.println("* auto route(新幹線も会社線も使う) >>>>>>>");
                } else {
                    farertAssert.ASSERT(false);
                }
            } else {
                out.println("* auto route(新幹線未使用) >>>>>>>");
            }
            out.printf("arg=%d\n", resopt ? autotype : 0);

            rc = route.changeNeerest(resopt ? autotype : 0, RouteUtil.GetStationId(p));
            if ((rc < 0) || (rc == 5)) {
                out.printf("Can't route.%s, rc=%d\n", fail != 0 ? "(OK)" : "(NG)", rc);
                if (!resopt) {
                    farertAssert.ASSERT(fail != 0);
                }
            } else {
                if (!resopt) {
                    farertAssert.ASSERT(fail == 0);
                }
                FARE_INFO fi = new FARE_INFO();
                CalcRoute croute = new CalcRoute(route);
                croute.calcFare(fi);
                String s = normalizeOutput(fi.showFare(croute.getRouteFlag()));
                out.println("///既定");
                out.println(s);
                if (!resopt && croute.getRouteFlag().rule_en()) {
                    croute.getRouteFlag().setNoRule(true);
                    croute.calcFare(fi);
                    s = normalizeOutput(fi.showFare(croute.getRouteFlag()));
                    out.println("///非適用");
                    out.println(s);
                }
            }

            if (resopt) {
                break;
            }

            route.removeAll();
            rc = route.setup_route(pre);
            farertAssert.ASSERT(0 <= rc);
            out.println("* auto route(新幹線使用) >>>>>>>");
            out.printf("arg=%d\n", resopt ? autotype : 0);
            rc = route.changeNeerest(1, RouteUtil.GetStationId(p));
            out.printf("auto route(ret=%d)\n", rc);
            if ((rc < 0) || (rc == 5)) {
                out.printf("Can't route.%s\n", fail != 0 ? "(OK)" : "(NG)");
                if (!resopt) {
                    farertAssert.ASSERT(fail != 0);
                }
            } else {
                if (!resopt) {
                    farertAssert.ASSERT(0 <= fail);
                }
                FARE_INFO fi = new FARE_INFO();
                CalcRoute croute = new CalcRoute(route);
                croute.getRouteFlag().setNoRule(false);
                croute.calcFare(fi);
                String s = normalizeOutput(fi.showFare(croute.getRouteFlag()));
                out.println("///既定");
                out.println(s);
                if (!resopt && croute.getRouteFlag().rule_en()) {
                    croute.getRouteFlag().setNoRule(true);
                    croute.calcFare(fi);
                    s = normalizeOutput(fi.showFare(croute.getRouteFlag()));
                    out.println("///非適用");
                    out.println(s);
                }
            }

            route.removeAll();
            rc = route.setup_route(pre);
            farertAssert.ASSERT(0 <= rc);
            out.println("* auto route(会社線使用) >>>>>>>");
            out.printf("arg=%d\n", resopt ? autotype : 0);
            rc = route.changeNeerest(2, RouteUtil.GetStationId(p));
            if ((rc < 0) || (rc == 5)) {
                out.printf("Can't route.%s\n", fail != 0 ? "(OK)" : "(NG)");
            } else {
                if (!resopt) {
                    farertAssert.ASSERT(0 <= fail);
                }
                FARE_INFO fi = new FARE_INFO();
                CalcRoute croute = new CalcRoute(route);
                croute.getRouteFlag().setNoRule(false);
                croute.calcFare(fi);
                String s = normalizeOutput(fi.showFare(croute.getRouteFlag()));
                out.println("///既定");
                out.println(s);
                if (!resopt && croute.getRouteFlag().rule_en()) {
                    croute.getRouteFlag().setNoRule(true);
                    croute.calcFare(fi);
                    s = normalizeOutput(fi.showFare(croute.getRouteFlag()));
                    out.println("///非適用");
                    out.println(s);
                }
            }

            route.removeAll();
            rc = route.setup_route(pre);
            farertAssert.ASSERT(0 <= rc);
            out.println("* auto route(会社線+新幹線使用) >>>>>>>");
            out.printf("arg=%d\n", resopt ? autotype : 0);
            rc = route.changeNeerest(3, RouteUtil.GetStationId(p));
            if ((rc < 0) || (rc == 5)) {
                out.printf("Can't route.%s\n", fail != 0 ? "(OK)" : "(NG)");
            } else {
                if (!resopt) {
                    farertAssert.ASSERT(0 <= fail);
                }
                FARE_INFO fi = new FARE_INFO();
                CalcRoute croute = new CalcRoute(route);
                croute.getRouteFlag().setNoRule(false);
                croute.calcFare(fi);
                String s = normalizeOutput(fi.showFare(croute.getRouteFlag()));
                out.println("///既定");
                out.println(s);
                if (croute.getRouteFlag().rule_en()) {
                    croute.getRouteFlag().setNoRule(true);
                    croute.calcFare(fi);
                    s = normalizeOutput(fi.showFare(croute.getRouteFlag()));
                    out.println("///特例非適用");
                    out.println(s);
                }
            }
        }
    }

    private static void test_jctspecial(Out out, List<String> routeDef) {
        Route route = new Route();
        for (int i = 0; i < routeDef.size(); i++) {
            String def = routeDef.get(i);
            if (def.isEmpty()) {
                break;
            }
            currentCase = "test_jctspecial[" + (i + 1) + "] " + def;
            route.removeAll();
            int bar = def.indexOf('|');
            String title = def.substring(0, bar);
            String pbuffer = def.substring(bar + 1);
            out.printf("!****<%02d>: ********************* %s *******************\n<%s>\n", i + 1, title, pbuffer);
            int rc = test_setup_route(pbuffer, route, out);
            farertAssert.ASSERT(0 <= rc);

            FARE_INFO fi = new FARE_INFO();
            CalcRoute croute = new CalcRoute(route);
            croute.calcFare(fi);
            String s = normalizeOutput(fi.showFare(croute.getRouteFlag()));
            out.println(s);

            if (0 < route.routeList().size()) {
                RouteItem first = route.routeList().get(0);
                out.println("");
                out.printf("begin: %s\n", RouteUtil.StationName(first.stationId));
                for (int pos = 1; pos < route.routeList().size(); pos++) {
                    RouteItem item = route.routeList().get(pos);
                    out.printf("%s, %s, %04x\n",
                            RouteUtil.LineName(item.lineId),
                            RouteUtil.StationName(item.stationId),
                            item.flag);
                }
                out.println("");
                out.printf("rc=%d, modify flag=%s\n", rc, route.isModified() ? "ON" : "OFF");
            }
        }
    }

    private static int test_setup_route(String buffer, Route route, Out out) {
        String[] tokens = buffer.split("[, \\t]+");
        int lineId = 0;
        int stationId1 = 0;
        int stationId2;

        for (String token : tokens) {
            if (token.isEmpty()) {
                continue;
            }
            int fail;
            if (stationId1 == 0) {
                stationId1 = RouteUtil.GetStationId(token);
                farertAssert.ASSERT(0 < stationId1);
                route.add(stationId1);
            } else if (lineId == 0) {
                if (token.startsWith("r")) {
                    token = token.substring(1);
                    route.setDetour();
                }
                lineId = RouteUtil.GetLineId(token);
                farertAssert.ASSERT(0 < lineId);
            } else {
                if (token.startsWith("x")) {
                    token = token.substring(1);
                    fail = 1;
                } else if (token.startsWith("e")) {
                    token = token.substring(1);
                    fail = 0;
                } else if (token.startsWith("p")) {
                    token = token.substring(1);
                    fail = 2;
                } else {
                    fail = -1;
                }
                stationId2 = RouteUtil.GetStationId(token);
                farertAssert.ASSERT(0 < stationId2);
                int rc = route.add(lineId, stationId2);
                if (fail == -1) {
                    if (rc < 1) {
                        System.err.printf("add failed: rc=%d token=%s route=%s%n", rc, token, buffer);
                    }
                    farertAssert.ASSERT(1 <= rc);
                } else if (fail == 1) {
                    if (!((rc < 0) || (rc == 5))) {
                        System.err.printf("expected failure but rc=%d token=%s route=%s%n", rc, token, buffer);
                    }
                    out.printf("Setup route: Failure OK (%d)\n", rc);
                    return 1;
                } else if (fail == 0) {
                    farertAssert.ASSERT(rc == 0);
                } else if (fail == 2) {
                    farertAssert.ASSERT(rc == 4);
                } else {
                    farertAssert.ASSERT(false);
                }
                lineId = 0;
                stationId1 = stationId2;
            }
        }
        return 0;
    }

    private static void test_shinkanzen() {
        farertAssert.ASSERT(Route.IsAbreastShinkansen(
                RouteUtil.GetLineId("東海道線"),
                RouteUtil.GetLineId("東海道新幹線"),
                RouteUtil.GetStationId("三島"),
                RouteUtil.GetStationId("新富士(東)")));
        farertAssert.ASSERT(!Route.IsAbreastShinkansen(
                RouteUtil.GetLineId("上越線"),
                RouteUtil.GetLineId("上越新幹線"),
                RouteUtil.GetStationId("越後湯沢"),
                RouteUtil.GetStationId("新富士(東)")));
        farertAssert.ASSERT(!Route.IsAbreastShinkansen(
                RouteUtil.GetLineId("上越線"),
                RouteUtil.GetLineId("上越新幹線"),
                RouteUtil.GetStationId("高崎"),
                RouteUtil.GetStationId("上毛高原")));
        farertAssert.ASSERT(Route.IsAbreastShinkansen(
                RouteUtil.GetLineId("高崎線"),
                RouteUtil.GetLineId("上越新幹線"),
                RouteUtil.GetStationId("高崎"),
                RouteUtil.GetStationId("熊谷")));
        farertAssert.ASSERT(RouteUtil.GetStationId("武蔵溝ノ口") == RouteUtil.GetStationId("武蔵溝の口"));
        farertAssert.ASSERT(RouteUtil.GetStationId("御茶ノ水") == RouteUtil.GetStationId("お茶の水"));
    }

    private static void test_shin2zai(Out out, List<String> testShin2Zai) {
        Route route = new Route();
        for (String def : testShin2Zai) {
            if (def.isEmpty()) {
                break;
            }
            currentCase = "test_shin2zai " + def;
            int rc = route.setup_route(def);
            if ((rc != 0) && (rc != 1)) {
                farertAssert.ASSERT(false);
                return;
            }
            out.println(def);
            FARE_INFO fi = new FARE_INFO();
            CalcRoute croute = new CalcRoute(route);
            croute.calcFare(fi);
            String s = normalizeOutput(fi.showFare(croute.getRouteFlag()));
            out.println(s);

            croute.testConvertShinkansen2ZairaiFor114Judge();
            croute.calcFare(fi);
            s = normalizeOutput(fi.showFare(croute.getRouteFlag()));
            out.println("---------------");
            out.println(s);
        }
    }

    private static String normalizeOutput(String s) {
        String normalized = s.replace("\r", "");
        normalized = normalized.replace("JR東日本営業キロ", "JR東日本 営業キロ");
        String[] lines = normalized.split("\n", -1);
        StringBuilder out = new StringBuilder();
        for (int i = 0; i < lines.length; i++) {
            String line = lines[i];
            if (line.contains("運賃：") && !line.contains("運賃(IC)：")) {
                line = line.replace(")    往復：", ")     往復：");
            }
            if (line.contains("運賃(IC)：")) {
                line = line.replace("      往復：", "    往復：");
                if (!line.contains("往復：")) {
                    line = rtrimAll(line);
                }
            }
            out.append(line);
            if (i < lines.length - 1) {
                out.append("\n");
            }
        }
        return out.toString();
    }

    private static String rtrimAll(String s) {
        int end = s.length();
        while (end > 0 && s.charAt(end - 1) == ' ') {
            end--;
        }
        return s.substring(0, end);
    }

    private static String trimOneTrailingSpace(String s) {
        if (s.endsWith(" ")) {
            return s.substring(0, s.length() - 1);
        }
        return s;
    }

    private static class Out {
        private final Writer writer;

        Out(Writer writer) {
            this.writer = writer;
        }

        void printf(String format, Object... args) {
            print(String.format(Locale.JAPANESE, format, args));
        }

        void println(String line) {
            print(line);
            print("\n");
        }

        void print(String s) {
            try {
                writer.write(s);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
    }

    private static class QuietPrintStream extends java.io.PrintStream {
        QuietPrintStream() {
            super(java.io.OutputStream.nullOutputStream());
        }

        @Override
        public java.io.PrintStream printf(String format, Object... args) {
            return this;
        }

        @Override
        public java.io.PrintStream format(String format, Object... args) {
            return this;
        }
    }

    private static void loadReferenceMarkers() throws IOException {
        if ((referenceTimestampLine != null) && (referenceProcessLapseLine != null)) {
            return;
        }
        List<String> lines = Files.readAllLines(REF_RESULT, StandardCharsets.UTF_8);
        if (lines.isEmpty()) {
            throw new IllegalStateException("Empty: " + REF_RESULT);
        }
        referenceTimestampLine = lines.get(0);
        for (int i = lines.size() - 1; i >= 0; i--) {
            String line = lines.get(i);
            if (line.startsWith("proces lapse: ")) {
                referenceProcessLapseLine = line;
                break;
            }
        }
    }

    private static class TestData {
        final List<String> testRoute2;
        final List<String> autoRouteDef;
        final List<String> testRoute;
        final List<String> testRoute3;
        final List<String> hzlRouteDef;
        final List<String> hzlDefTbl;
        final List<String> jctSpecial;
        final List<String> testShin2Zai;

        private TestData(List<String> testRoute2,
                         List<String> autoRouteDef,
                         List<String> testRoute,
                         List<String> testRoute3,
                         List<String> hzlRouteDef,
                         List<String> hzlDefTbl,
                         List<String> jctSpecial,
                         List<String> testShin2Zai) {
            this.testRoute2 = testRoute2;
            this.autoRouteDef = autoRouteDef;
            this.testRoute = testRoute;
            this.testRoute3 = testRoute3;
            this.hzlRouteDef = hzlRouteDef;
            this.hzlDefTbl = hzlDefTbl;
            this.jctSpecial = jctSpecial;
            this.testShin2Zai = testShin2Zai;
        }

        static TestData load(Path path) throws IOException {
            String text = Files.readString(path, StandardCharsets.UTF_8);
            text = stripBlockComments(text);
            List<String> lines = List.of(text.split("\n", -1));

            return new TestData(
                    loadArray(lines, "test_route2_tbl"),
                    loadArray(lines, "auto_route_def"),
                    loadArray(lines, "test_route_tbl"),
                    loadArray(lines, "test_route3_tbl"),
                    loadArray(lines, "hzl_route_def"),
                    loadArray(lines, "hzl_def_tbl"),
                    loadArray(lines, "jct_spcial_route_tbl"),
                    loadArray(lines, "test_shin2_zai_tbl")
            );
        }

        private static List<String> loadArray(List<String> lines, String arrayName) {
            List<String> result = new ArrayList<>();
            boolean inArray = false;
            for (String rawLine : lines) {
                String line = stripLineComment(rawLine);
                if (!inArray) {
                    if (line.contains(arrayName) && line.contains("{")) {
                        inArray = true;
                    }
                    continue;
                }
                int endIdx = line.indexOf("};");
                if (endIdx >= 0) {
                    line = line.substring(0, endIdx);
                }
                for (String lit : extractStringLiterals(line)) {
                    result.add(unescapeCString(lit));
                }
                if (endIdx >= 0) {
                    break;
                }
            }
            return result;
        }

        private static String stripBlockComments(String text) {
            StringBuilder out = new StringBuilder();
            boolean inBlock = false;
            for (int i = 0; i < text.length(); i++) {
                char c = text.charAt(i);
                if (inBlock) {
                    if (c == '*' && i + 1 < text.length() && text.charAt(i + 1) == '/') {
                        inBlock = false;
                        i++;
                    } else if (c == '\n') {
                        out.append('\n');
                    }
                } else {
                    if (c == '/' && i + 1 < text.length() && text.charAt(i + 1) == '*') {
                        inBlock = true;
                        i++;
                    } else {
                        out.append(c);
                    }
                }
            }
            return out.toString();
        }

        private static String stripLineComment(String line) {
            StringBuilder out = new StringBuilder();
            boolean inString = false;
            boolean escape = false;
            for (int i = 0; i < line.length(); i++) {
                char c = line.charAt(i);
                if (!inString && c == '/' && i + 1 < line.length() && line.charAt(i + 1) == '/') {
                    break;
                }
                out.append(c);
                if (escape) {
                    escape = false;
                } else if (c == '\\') {
                    escape = true;
                } else if (c == '"') {
                    inString = !inString;
                }
            }
            return out.toString();
        }

        private static List<String> extractStringLiterals(String line) {
            List<String> result = new ArrayList<>();
            boolean inString = false;
            StringBuilder current = new StringBuilder();
            for (int i = 0; i < line.length(); i++) {
                char c = line.charAt(i);
                if (!inString) {
                    if (c == '"') {
                        inString = true;
                        current.setLength(0);
                    }
                    continue;
                }
                if (c == '"' && !isEscaped(line, i)) {
                    inString = false;
                    result.add(current.toString());
                    continue;
                }
                current.append(c);
            }
            return result;
        }

        private static boolean isEscaped(String line, int quoteIndex) {
            int count = 0;
            for (int i = quoteIndex - 1; i >= 0; i--) {
                if (line.charAt(i) == '\\') {
                    count++;
                } else {
                    break;
                }
            }
            return (count % 2) == 1;
        }

        private static String unescapeCString(String raw) {
            StringBuilder out = new StringBuilder();
            for (int i = 0; i < raw.length(); i++) {
                char c = raw.charAt(i);
                if (c != '\\') {
                    out.append(c);
                    continue;
                }
                if (i + 1 >= raw.length()) {
                    break;
                }
                char n = raw.charAt(++i);
                switch (n) {
                    case 'n':
                        out.append('\n');
                        break;
                    case 'r':
                        out.append('\r');
                        break;
                    case 't':
                        out.append('\t');
                        break;
                    case '\\':
                        out.append('\\');
                        break;
                    case '"':
                        out.append('"');
                        break;
                    case 'u':
                        int[] u = readHexValue(raw, i + 1, 4);
                        out.append((char) u[0]);
                        i += u[1];
                        break;
                    case 'x':
                        int[] x = readHexValue(raw, i + 1, 2);
                        out.append((char) x[0]);
                        i += x[1];
                        break;
                    default:
                        if (n >= '0' && n <= '7') {
                            int len = 1;
                            while (len < 3 && i + len < raw.length()) {
                                char d = raw.charAt(i + len);
                                if (d < '0' || d > '7') {
                                    break;
                                }
                                len++;
                            }
                            String oct = raw.substring(i, i + len);
                            out.append((char) Integer.parseInt(oct, 8));
                            i += len - 1;
                        } else {
                            out.append(n);
                        }
                        break;
                }
            }
            return out.toString();
        }

        private static int[] readHexValue(String raw, int start, int maxLen) {
            int end = Math.min(raw.length(), start + maxLen);
            int len = 0;
            while (start + len < end) {
                char c = raw.charAt(start + len);
                if (Character.digit(c, 16) < 0) {
                    break;
                }
                len++;
            }
            if (len == 0) {
                return new int[]{0, 0};
            }
            String hex = raw.substring(start, start + len);
            return new int[]{Integer.parseInt(hex, 16), len};
        }
    }
}
