package org.sutezo.alps

import android.content.Context
import org.json.JSONArray
import org.sutezo.farert.ArchiveRouteActivity


const val MAX_HOLDER : Int = 60
const val MAX_HISTORY = 20
const val MAX_ARCHIVE_ROUTE = 100

fun getCompanys() : List<Int> {
    val companys : MutableList<Int> = mutableListOf() // JR group
    val dbo = RouteUtil.Enum_company_prefect()

    dbo.use {
        while (dbo.moveToNext()) {
            val ident = dbo.getInt(1);
            if (ident < 0x10000) {
                companys.add(ident)
            }
        }
    }
    return companys
}

fun getPrefects() : List<Int> {

    val prefects : MutableList<Int> = ArrayList(46) // Prefect
    val dbo = RouteUtil.Enum_company_prefect()

    dbo.use {
        while (dbo.moveToNext()) {
            val ident = dbo.getInt(1);
            if (ident < 0x10000) {
                // ignored
            } else {
                prefects.add(ident)
            }
        }
    }
    return prefects
}

fun fareNumStr(num: Int): String
{
    return RouteUtil.num_str_yen(num)
}

fun kmNumStr(num: Int): String
{
    return RouteUtil.num_str_km(num)
}

// Kana from Stations
//  キーワードに一致した駅名を返す
//  ひらがな昇順ソート
fun keyMatchStations(key: String): List<Int>
{
    val stations: MutableList<Int> = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_station_match(key)
    dbo.use {
        while (dbo.moveToNext()) {
            val station_id = dbo.getInt(1);
            stations.add(station_id)
        }
    }
    return stations;
}


fun getStationId(name: String) : Int
{
    return RouteUtil.GetStationId(name)
}

// StationName
fun stationName(ident: Int):String
{
    return RouteUtil.StationName(ident)
}

// StationNameEx
fun stationNameEx(ident:Int):String
{
    return RouteUtil.StationNameEx(ident)
}

fun lineName(ident:Int) : String
{
    return RouteUtil.LineName(ident)
}

// Company or Prefect Name
fun companyOrPrefectName(ident:Int):String
{
    if (ident < 0x10000) {
        return RouteUtil.CompanyName(ident)
    } else {
        return RouteUtil.PrefectName(ident)
    }
}

// line from prefect or company
//  会社/都道府県idから路線を返す
fun linesFromCompanyOrPrefect(id:Int): List<Int>
{
    val lines : MutableList<Int> = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_lines_from_company_prefect(id)
    dbo.use {
        while (dbo.moveToNext()) {
            lines.add(dbo.getInt(1))
        }
    }
    return lines
}

//	Enum menu_station_select with in company or prefect + line
//	会社or都道府県 + 路線に属する駅を列挙
fun stationsWithInCompanyOrPrefectAnd(id:Int, line_id:Int) : List<Int>
{
    val stations : MutableList<Int> = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_station_located_in_prefect_or_company_and_line(id, line_id)
    dbo.use {
        while (dbo.moveToNext()) {
            stations.add(dbo.getInt(1))
        }
    }
    return stations;
}

fun lineIdsFromStation(id:Int):List<Int>
{
    val lines = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_line_of_stationId(id)
    dbo.use {
        while (dbo.moveToNext()) {
            lines.add(dbo.getInt(1))
        }
    }
    return lines;
}

fun stationsIdsOfLineId(lineId:Int):List<Int>
{
    val stations = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_station_of_lineId(lineId)
    dbo.use {
        while (dbo.moveToNext()) {
            stations.add(dbo.getInt(1))
        }
    }
    return stations;
}

fun junctionIdsOfLineId(lineId:Int, stationId: Int = 0):List<Int>
{
    val stations = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_junction_of_lineId(lineId, stationId)
    dbo.use {
        while (dbo.moveToNext()) {
            stations.add(dbo.getInt (1))
        }
    }
    return stations;
}

fun terminalName(ident:Int):String = CalcRoute.BeginOrEndStationName(ident)

fun isJunction(station_id:Int): Boolean = RouteUtil.STATION_IS_JUNCTION(station_id)

fun isSpecificJunction(lineId:Int, station_id:Int) : Boolean =
    0 != ((RouteUtil.AttrOfStationOnLineLine(lineId, station_id) and (1.shl(31))))

fun PrectNameByStation(stationId:Int):String = RouteUtil.GetPrefectByStationId(stationId)

//	駅のひらがな読み
fun getKanaFromStationId(ident:Int): String = RouteUtil.GetKanaFromStationId(ident)

////  履歴配列を追加保存する
////
////
//fun SaveToTerminalHistoryWithArray(historyAry:Array<String>) : Unit
//{
//    /* Store */
//    [[NSUserDefaults standardUserDefaults] setObject:historyAry forKey:@"HistoryTerminalName"];
//    [[NSUserDefaults standardUserDefaults] synchronize];
//}
//
////  不揮発メモリから履歴配列を読み込む
////
//fun ReadFromTerminalHistory() : List<String>
//{
//    NSArray *storedDevices = [[NSUserDefaults standardUserDefaults] arrayForKey:@"HistoryTerminalName"];
//
//    if (![storedDevices isKindOfClass:[NSArray class]]) {
//    NSLog(@"Can't find/create an array to store");
//    return nil;
//}
//    return storedDevices;
//}
//
//// 設定ファイルから経路配列を読み込む
////
//fun loadStrageRoute(): List<String>
//{
//    return [self loadFromPlistWithArray:@"route.plist"];
//}
//
////  不揮発メモリに保持された経路配列内に指定した経路が格納されているか？
////
//fun isRouteInStrage(routeString:String):Boolean
//{
//    NSArray * ary = [self loadStrageRoute];
//    return [ary containsObject:routeString];
//}
//
////  経路情報文字列を不揮発メモリへ書き込む（最大MAX_ARCHIVE_ROUTE未満を切り詰め）
////
//+ (void)saveToRouteArray:(NSArray*)routeList
//{
//    if (MAX_ARCHIVE_ROUTE < [routeList count]) {
//        NSMutableArray* writeRouteList = [NSMutableArray arrayWithArray:routeList];
//        while (MAX_ARCHIVE_ROUTE < [writeRouteList count]) {
//            [writeRouteList removeLastObject];
//        }
//        [self saveToPlistWithArray:writeRouteList filename:@"route.plist"];
//    } else {
//        [self saveToPlistWithArray:routeList filename:@"route.plist"];
//    }
//}
//
////  設定ファイルへ保存
////
////
//+ (void)saveToPlistWithArray:(NSArray *)array filename:(NSString*)filename
//{
//    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//    NSString *directory = [paths objectAtIndex:0];
//    NSString *filePath = [directory stringByAppendingPathComponent:filename];
//
//    BOOL successful = [array writeToFile:filePath atomically:NO];
//
//    if (successful) {
//        // success
//    }
//}
//
////  設定ファイルから読み出し
////
////
//+ (NSArray*)loadFromPlistWithArray:(NSString*)filename
//{
//    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//    NSString *directory = [paths objectAtIndex:0];
//    NSString *filePath = [directory stringByAppendingPathComponent:filename];
//    NSArray *array = [[NSArray alloc] initWithContentsOfFile:filePath];
//    NSMutableArray* newArray = [[NSMutableArray alloc] initWithCapacity:[array count]];
//
//    // 旧リリース済みバージョンには改行コード付きで保持しているのがあるので除去
//    for (NSString* s in array) {
//    [newArray addObject:[s stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]];
//}
//    return newArray;
//}
//
//// Identifier of Start Terminal
//// 開始駅id
//- (NSInteger)startStationId
//{
//    return obj_route_list->startStationId();
//}
//
//- (NSInteger)lastStationId
//{
//    return obj_route_list->routeList().back().stationId;
//}
//
//- (NSString*)routeScript
//{
//    return [NSString stringWithUTF8String:obj_route_list->route_script().c_str()];
//}
//
//
//- (void)sync:(cCalcRoute*)source
//{
//    obj_route->assign(*source->obj_calcroute);
//}
//
//- (void)assign:(cRouteList*)source
//{
//    obj_route->assign(*source->obj_route_list);
//}
//
//// 開始駅（全経路）クリア
//-(void)removeAll   // removeAll() with start
//{
//    obj_route->removeAll();
//}
//
//// set route Start
//- (NSInteger)addRoute:(NSInteger)stationId
//{
//    return obj_route->add((int)stationId);
//}
//
//// apeend to route
//- (NSInteger)addRoute:(NSInteger)line_Id stationId:(NSInteger)stationId
//{
//    return obj_route->add((int)line_Id, (int)stationId);
//}
//
//- (void)removeTail
//{
//    obj_route->removeTail();
//}
//
//// Auto route
////	最短経路に変更(raw immidiate)
////
////	@param [in] useBulletTrain (bool)新幹線使用有無
////	@retval true success
////	@retval 1 : success
////	@retval 0 : loop end.
////	@retval -n: add() error(re-track)
////	@retval -32767 unknown error(DB error or BUG)
////
//- (NSInteger)autoRoute:(BOOL)useBullet arrive:(NSInteger)stationId
//{
//    return obj_route->changeNeerest(useBullet, (int)stationId);
//}
//
//// reverse
//- (NSInteger)reverseRoute
//{
//    return obj_route->reverse();
//}
//
///*!	@brief ルート作成(文字列からRouteオブジェクトの作成)
// *
// *	@param [in] route_str	カンマなどのデリミタで区切られた文字列("駅、路線、分岐駅、路線、..."）
// *	@retval -200 failure(駅名不正)
// *	@retval -300 failure(線名不正)
// *	@retval -1   failure(経路重複不正)
// *	@retval -2   failure(経路不正)
// *	@retval 1 success
// *	@retval 0 success
// */
//- (NSInteger)setupRoute:(NSString*)routeString
//{
//    return obj_route->setup_route([routeString UTF8String]);
//}
//
////- (NSInteger)getFareOption;
//- (NSInteger)setDetour:(BOOL)enabled
//{
//    return obj_route->setDetour(enabled);
//}
//
//// Number of Route
//- (NSInteger)getRouteCount
//{
//    return obj_route->routeList().size();
//}
//
//// Retrieve route item
//- (cRouteItem*)getRouteItem:(NSInteger)index
//{
//    RouteItem ri(obj_route->routeList().at(index));
//    NSString* linename = [NSString stringWithUTF8String:RouteUtil::LineName(ri.lineId).c_str()];
//    NSString* stationname = [NSString stringWithUTF8String:RouteUtil::StationNameEx(ri.stationId).c_str()];
//    return [[cRouteItem alloc] initWithName:linename stationName:stationname];
//}
//
//// Identifier of Start Terminal
//// 開始駅id
//- (NSInteger)startStationId
//{
//    return obj_route->startStationId();
//}
//
//// route string
//// 規則非適用の表示指定の経路文字列をカンマで区切って返す（archive保存用　setup_route()で復元可能）
//- (NSString*)routeScript
//{
//    return [NSString stringWithUTF8String:obj_route->route_script().c_str()];
//}
//
//- (NSInteger)lastStationId
//{
//    return obj_route->routeList().back().stationId;
//}
//
//- (NSInteger)lastLineId
//{
//    return obj_route->routeList().back().lineId;
//}
//
//
//
////////////////////////////////////////////////////////////////
//#pragma mark - cCalcRoute
//
//@implementation cCalcRoute
//
///*
//-(id)init {
//    self = [super init];
//    obj_calcroute = new CalcRoute();
//    return self;
//}
//
//*/
//
//- (id)initWithRoute:(cRoute*)source
//{
//    return [self initWithRoute:source count:-1];
//}
//
//- (id)initWithRoute:(cRoute*)source count:(NSInteger)count
//{
//    if (self = [super init]) {
//        self->obj_calcroute = new CalcRoute(*source->obj_route, (int)count);
//    }
//    return self;
//}
//
//- (id)initWithRouteList:(cRouteList*)source
//{
//    if (self = [super init]) {
//        self->obj_calcroute = new CalcRoute(*source->obj_route_list, -1);
//    }
//    return self;
//}
//
//
//- (void)dealloc
//{
//    delete obj_calcroute;
////    [super dealloc];
//}
//
//- (void)sync:(cRoute*)source
//{
//    obj_calcroute->assign(*source->obj_route);
//}
//
//- (void)sync:(cRoute*)source count:(NSInteger)count
//{
//    obj_calcroute->assign(*source->obj_route, (int)count);
//}
//
//
//// fare option
////
//- (void)setFareOption:(NSInteger)optMask availMask:(NSInteger) availMask
//{
//    obj_calcroute->setFareOption(optMask, availMask);
//}
//
//// fare calc
//- (FareInfo*)calcFare
//{
//    FARE_INFO fi;
//    int fare_result;
//
//    fi = obj_calcroute->calcFare();
//    switch (fi.resultCode()) {
//        case 0:     // success, company begin/first or too many company
//        fare_result = 0;
//        break;  // OK
//        case -1:    /* In completed (吉塚、西小倉における不完全ルート) */
//        fare_result = 1;     //"この経路の片道乗車券は購入できません.続けて経路を指定してください."
//        break;
//        default:
//        return nil; /* -2:empty or -3:fail */
//        break;
//    }
//
//    int calcFlag = obj_calcroute->getFareOption();
//    FareInfo* result = nil;
//    string str1;
//    string str2;
//    string notused;
//    int w2;
//    int w3;
//    FARE_INFO::Fare rule114Fare;
//    FARE_INFO::FareResult fareResult;
//
//    result = [[FareInfo alloc] init];
//
//    result.result = fare_result;        /* calculate result */
//    result.calcResultFlag = calcFlag;   /* calculate flag */
//    result.isResultCompanyBeginEnd = fi.isBeginEndCompanyLine();
//    result.isResultCompanyMultipassed = fi.isMultiCompanyLine();
//
//    /* begin/end terminal */
//    result.beginStationId = fi.getBeginTerminalId();
//    result.endStationId = fi.getEndTerminalId();
//
//    result.isBeginInCity = FARE_INFO::IsCityId(fi.getBeginTerminalId());
//    result.isEndInCity = FARE_INFO::IsCityId(fi.getEndTerminalId());
//
//    /* route */
//    result.routeList = [NSString stringWithUTF8String:fi.getRoute_string().c_str()];
//
//    /* stock discount (114 no applied) */
//    w2 = fi.getFareStockDiscount(0, str1);
//    w3 = fi.getFareStockDiscount(1, str2);
//    [result setFareForStockDiscounts:w2 + fi.getFareForCompanyline()
//            title1:[NSString stringWithUTF8String:str1.c_str()]
//            Discount2:w3 + fi.getFareForCompanyline()
//            title2:[NSString stringWithUTF8String:str2.c_str()]];
//    result.availCountForFareOfStockDiscount = fi.countOfFareStockDiscount();
//
//    /* rule 114 */
//    rule114Fare = fi.getRule114();
//    if (rule114Fare.fare == 0) {
//        result.rule114_salesKm = 0;
//        result.rule114_calcKm = 0;
//        result.isRule114Applied = NO;
//    } else {
//        result.isRule114Applied = YES;
//        result.rule114_salesKm = rule114Fare.sales_km;
//        result.rule114_calcKm = rule114Fare.calc_km;
//
//        /* stock discount (114 applied) */
//        w2 = fi.getFareStockDiscount(0, notused, true);
//        w3 = fi.getFareStockDiscount(1, notused, true);
//
//        [result setFareForStockDiscountsForR114:w2 + fi.getFareForCompanyline()
//                Discount2:w3 + fi.getFareForCompanyline()];
//    }
//
//    result.isArbanArea = fi.isUrbanArea();
//
//
//    result.totalSalesKm = fi.getTotalSalesKm();
//    result.jrCalcKm = fi.getJRCalcKm();
//    result.jrSalesKm = fi.getJRSalesKm();
//
//    /* 会社線有無はtotalSalesKm != jrSalesKm */
//
//    result.companySalesKm = fi.getCompanySalesKm();
//    result.salesKmForHokkaido = fi.getSalesKmForHokkaido();
//    result.calcKmForHokkaido = fi.getCalcKmForHokkaido();
//
//
//    result.salesKmForShikoku = fi.getSalesKmForShikoku();
//    result.calcKmForShikoku = fi.getCalcKmForShikoku();
//
//
//    result.salesKmForKyusyu = fi.getSalesKmForKyusyu();
//    result.calcKmForKyusyu = fi.getCalcKmForKyusyu();
//
//
//    // 往復割引有無
//    result.isRoundtripDiscount = fi.isRoundTripDiscount();
//
//    // 会社線部分の運賃
//    result.fareForCompanyline = fi.getFareForCompanyline();
//
//    // 普通運賃
//    result.fare = fi.getFareForDisplay();
//    result.farePriorRule114 = fi.getFareForDisplayPriorRule114();
//
//    // 往復
//    fareResult = fi.roundTripFareWithCompanyLine();
//    result.roundTripFareWithCompanyLine = fareResult.fare;
//    result.roundTripFareWithCompanyLinePriorRule114 = fi.roundTripFareWithCompanyLinePriorRule114();
//
//    // IC運賃
//    result.fareForIC = fi.getFareForIC();
//
//    // 小児運賃
//    result.childFare = fi.getChildFareForDisplay();
//
//    // 学割運賃
//    if (0 < (result.academicFare = fi.getAcademicDiscountFare())) {
//        result.isAcademicFare = TRUE;
//        result.roundtripAcademicFare = fi.roundTripAcademicFareWithCompanyLine();
//    } else {
//        result.isAcademicFare = FALSE;
//        result.roundtripAcademicFare = 0;
//    }
//    // 有効日数
//    result.ticketAvailDays = fi.getTicketAvailDays();
//
//    return result;
//}
//
//
//// result (for sendmail)
//- (NSString*)showFare
//{
//    return [NSString stringWithUTF8String:obj_calcroute->showFare().c_str()];
//}
//
//// getFareOption() - 運賃計算オプションを得る
////	@return
////     & 0x80 = 0    : 該当なし
////     & 0x80 = 0x80 : empty or non calc.
////     & 0x03 = 0 : 無し(通常)(発・着が特定都区市内駅で特定都区市内間が100/200km以下ではない)
////			 (以下、発・着が特定都区市内駅で特定都区市内間が100/200kmを越える)
////	   & 0x03 = 0x01 : 結果表示状態は{特定都区市内 -> 単駅} (「発駅を単駅に指定」と表示)
////	   & 0x03 =	0x02 : 結果表示状態は{単駅 -> 特定都区市内} (「着駅を単駅に指定」と表示)
////     & 0x0c = 0x04 : 大阪環状線1回通過(近回り)(規定)
////     & 0x0c = 0x08 : 大阪環状線1回通過(遠回り)
//
//// 大阪環状線 遠回りか近回りか？
//- (BOOL)isOsakakanDetourShortcut
//{
//    int option = obj_calcroute->getFareOption();
//    return 0x08 == (option & 0x0c);
//}
//
//// 大阪環状線 遠回り設定か？
//- (BOOL)isOsakakanDetourEnable
//{
//    int option = obj_calcroute->getFareOption();
//    return 0x00 != (option & 0x0c);
//}
//
////-- cRouteList --
//// Number of Route
//- (NSInteger)getRouteCount
//{
//    return obj_calcroute->routeList().size();
//}
//
//// Identifier of Start Terminal
//// 開始駅id
//- (NSInteger)startStationId
//{
//    return obj_calcroute->startStationId();
//}
//
//// route string
//// 規則非適用の表示指定の経路文字列をカンマで区切って返す（archive保存用　setup_route()で復元可能）
//- (NSString*)routeScript
//{
//    return [NSString stringWithUTF8String:obj_calcroute->route_script().c_str()];
//}
//
//fun lastStationId : String =
//{
//    return obj_calcroute->routeList().back().stationId;
//}
//
////-- cRouteList --
//
//@end
/* End of CalcRoute */

fun saveParam(context: Context, key:String, value:String) : Unit
{
    val sharedPref = context.getSharedPreferences("settings", Context.MODE_PRIVATE)
    val sharedEdit = sharedPref.edit()
    sharedEdit.putString(key, value)
    sharedEdit.apply()
}

fun saveParam(context: Context, key: String, values: List<String>):Unit {
    val sharedPref = context.getSharedPreferences("settings", Context.MODE_PRIVATE)
    val jsonArray = JSONArray(values)
    val shredEdit = sharedPref.edit()
    shredEdit.putString(key, jsonArray.toString())
    shredEdit.apply()
}

fun readParam(context: Context, key: String) : String {
    val sharedPref = context.getSharedPreferences("settings", Context.MODE_PRIVATE)
    return sharedPref.getString(key, "")?: ""
}

fun readParams(context: Context, key: String) : List<String> {
    val values = mutableListOf<String>()
    var sharedPref  = context.getSharedPreferences("settings", Context.MODE_PRIVATE ) ?:
    return values

    val params = sharedPref.getString(key, "[]")
    val jsonArray = JSONArray(params)
    for (i in 0 until jsonArray.length()) {
        values.add(jsonArray.get(i).toString())
    }
    return values
}

fun saveHistory(context: Context, history : List<String>) {
    saveParam(context,"history", history)
}

fun appendHistory(context: Context, terminal : String) : Unit {
    val history = mutableListOf<String>()
    history.addAll(0, readParams(context, "history"))
    if (history.contains(terminal)) {
        history.remove(terminal)
    }
    history.add(0, terminal)
    saveParam(context,"history", history)
}


// 駅リスト選択でつかう
// line_id : 除外する路線
fun getDetailStationInfoForSelList(line_id : Int, station_id: Int) : String {
    var details : String = "(${RouteUtil.GetKanaFromStationId(station_id)}) "
    var f : Boolean = false

    if (RouteUtil.STATION_IS_JUNCTION(station_id) &&
            !RouteUtil.IsSpecificJunction(line_id, station_id)) {
        for (lid in lineIdsFromStation(station_id)) {
            if ((lid != line_id) &&
                    !RouteUtil.IsSpecificJunction(lid, station_id)) {
                /* junction(lflag.bit12 on) */
                if (f) {
                    details += "/${RouteUtil.LineName(lid)}"
                } else {
                    details += " ${RouteUtil.LineName(lid)}"
                    f = true
                }
            }
        }
    }
    return details
}


// fare calc Extend method
fun CalcRoute.calcFareInfo() : FareInfo
{
    val result = FareInfo()

    val fi = this.calcFare()
    when (fi.resultCode()) {
        0 -> {     // success, company begin/first or too many company
            result.result = 0;
        }
        -1 -> {    /* In completed (吉塚、西小倉における不完全ルート) */
            result.result = 1;     //"この経路の片道乗車券は購入できません.続けて経路を指定してください."
        }
        else -> {
            result.result = -2 /* -2:empty or -3:fail */
            return result
        }
    }

    val calcFlag = this.getFareOption()

    result.calcResultFlag = calcFlag;   /* calculate flag */
    result.isResultCompanyBeginEnd = fi.isBeginEndCompanyLine()
    result.isResultCompanyMultipassed = fi.isMultiCompanyLine()

    result.isEnableTokaiStockSelect = fi.isEnableTokaiStockSelect()

    /* begin/end terminal */
    result.beginStationId = fi.getBeginTerminalId()
    result.endStationId = fi.getEndTerminalId()

    result.isBeginInCity = FARE_INFO.IsCityId(fi.getBeginTerminalId());
    result.isEndInCity = FARE_INFO.IsCityId(fi.getEndTerminalId())

    /* route */
    result.routeList = fi.getRoute_string()

    /* stock discount (114 no applied) */
    val stks = mutableListOf<Triple<String, Int, Int>>()
    for (i in 0..1) {
        val stk = fi.getFareStockDiscount(i)
        if (0 < stk.fare) {
            stks.add(Triple(stk.title, stk.fare + fi.fareForCompanyline, 0))
        }
    }

    /* rule 114 */
    val rule114Fare = fi.getRule114();
    if (rule114Fare.fare == 0) {
        result.rule114_salesKm = 0;
        result.rule114_calcKm = 0;
        result.isRule114Applied = false;
        result.fareForStockDiscounts = stks
    } else {
        result.isRule114Applied = true;
        result.rule114_salesKm = rule114Fare.sales_km;
        result.rule114_calcKm = rule114Fare.calc_km;

        /* stock discount (114 applied) */
        for (i in 0..1) {
            val stk = fi.getFareStockDiscount(i, true)
            if (0 < stk.fare) {
                stks[i] = Triple(stks[i].first, stks[i].second + fi.fareForCompanyline, stk.fare + fi.fareForCompanyline)
            }
        }
        result.fareForStockDiscounts = stks
    }

    result.isArbanArea = fi.isUrbanArea();


    result.totalSalesKm = fi.getTotalSalesKm();
    result.jrCalcKm = fi.getJRCalcKm();
    result.jrSalesKm = fi.getJRSalesKm();

    /* 会社線有無はtotalSalesKm != jrSalesKm */

    result.companySalesKm = fi.getCompanySalesKm();
    result.salesKmForHokkaido = fi.getSalesKmForHokkaido();
    result.calcKmForHokkaido = fi.getCalcKmForHokkaido();

    // BRT営業キロ
    result.brtSalesKm = fi.brtSalesKm

    result.salesKmForShikoku = fi.getSalesKmForShikoku();
    result.calcKmForShikoku = fi.getCalcKmForShikoku();


    result.salesKmForKyusyu = fi.getSalesKmForKyusyu();
    result.calcKmForKyusyu = fi.getCalcKmForKyusyu();


    // 往復割引有無
    result.isRoundtripDiscount = fi.isRoundTripDiscount();

    // 会社線部分の運賃
    result.fareForCompanyline = fi.getFareForCompanyline();

    // BRT運賃
    result.fareForBRT = fi.fareForBRT

    result.isBRT_discount = fi.isBRT_discount

    // 普通運賃
    result.fare = fi.getFareForDisplay();
    result.farePriorRule114 = fi.getFareForDisplayPriorRule114();
    result.isSpecificFare = fi.applied_specic_fare

    // 往復
    val fareResult = fi.roundTripFareWithCompanyLine()
    result.roundTripFareWithCompanyLine = fareResult.fare
    if (fi.isRule114()) {
        result.roundTripFareWithCompanyLinePriorRule114 = fi.roundTripFareWithCompanyLinePriorRule114();
    }

    // IC運賃
    result.fareForIC = fi.getFareForIC();

    // 小児運賃
    result.childFare = fi.getChildFareForDisplay();

    // 学割運賃
    result.academicFare = fi.getAcademicDiscountFare()
    if (0 < result.academicFare) {
        result.isAcademicFare = true
        result.roundtripAcademicFare = fi.roundTripAcademicFareWithCompanyLine()
    } else {
        result.isAcademicFare = false
        result.roundtripAcademicFare = 0
    }
    // 有効日数
    result.ticketAvailDays = fi.getTicketAvailDays();

    result.routeListForTOICA = fi.calc_route_for_disp

    return result;
}

// 経路は不揮発メモリに保存されたものか？
fun isStrageInRoute(context: Context, routeScript : String) : Boolean {
    var listItems = readParams(context, ArchiveRouteActivity.KEY_ARCHIVE)
    return listItems.contains(routeScript)
}