/* c_route.mm */
#import <Foundation/Foundation.h>
#import "../../alps/alpdb.h"
#import "c_route.h"
#import "Farert-Bridging-Header.h"

@class FareInfo;

int g_tax; /* main.m */

#pragma mark - cRouteItem
@implementation cRouteItem

- (id)initWithName:(NSString*)line stationName:(NSString*)station //salesKm:(NSString*)salesKm fare:(NSString*)fare
{
    self = [super init];
    if (self) {
        _line = line;
        _station = station;
//        _fare = fare;
//        _salesKm = salesKm;
		return self;
	}
	return nil;
}

@end

#pragma mark - cRouteUtil

@implementation cRouteUtil {
}

+ (DbSys*)DatabaseVersion
{
    DBsys c_dbsys;

    if (RouteUtil::DbVer(&c_dbsys)) {
        DbSys* dbSys = [[DbSys alloc] initWithName:[NSString stringWithUTF8String:c_dbsys.name]
                                 sales_tax:/*c_dbsys.tax*/g_tax
                                      Date:[NSString stringWithUTF8String:c_dbsys.createdate]];
        return dbSys;
    } else {
        return nil;
    }
}

#pragma mark - Management of database

+ (BOOL)OpenDatabase
{
    return [self OpenDatabase:[self GetDatabaseId]];
}

+ (BOOL)OpenDatabase:(enum DB)ident
{
    NSString* dbname;
    NSString* dbpath;

    switch (ident) {
        case DB_2014:
            g_tax = 8;          // %
            dbname = @"jrdb2014";
            break;
        case DB_TAX5:
            g_tax = 5;
            dbname = @"jrdb2014";
            break;
        case DB_2015:
            g_tax = 8;
            dbname = @"jrdb2015";
            break;
        case DB_2017:
            g_tax = 8;
            dbname = @"jrdb2017";
            break;
        case DB_2019_8:
            g_tax = 8;
            dbname = @"jrdbnewest";
            break;
        case DB_2019_10:
        default:
            g_tax = 10;
            dbname = @"jrdbnewest";
            break;
    }
    dbpath = [[NSBundle mainBundle] pathForResource:dbname ofType:@"db"];
    return DBS::getInstance()->open([dbpath UTF8String]); // C++
}

+ (void)CloseDatabase
{
    return DBS::getInstance()->close();
}

// Save databse index
+ (void)SaveToDatabaseId:(NSInteger)dbid
{
    [self SaveToDatabaseId:dbid sync:YES];
}

+ (void)SaveToDatabaseId:(NSInteger)dbid sync:(BOOL) sync
{
    /* Store */
    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithInteger:dbid] forKey:@"JrDatabaseId"];
    if (sync) {
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
}

// Retrieve database index
+ (enum DB)GetDatabaseId
{
    NSInteger dbid =
    [[NSUserDefaults standardUserDefaults] integerForKey:@"JrDatabaseId"];
    return DB(dbid);
}


#pragma mark - alpdb utils

+ (NSString*)fareNumStr:(NSInteger)num
{
    return [NSString stringWithUTF8String:num_str_yen((int)num).c_str()];
}

+ (NSString*)kmNumStr:(NSInteger)num
{
    return [NSString stringWithUTF8String:num_str_km((int)num).c_str()];
}

// All list of Comapny and prefects
+ (NSArray*)GetCompanyAndPrefects
{
    NSMutableArray* companys = [NSMutableArray arrayWithCapacity:6];      /* JR group */
    NSMutableArray* prefects = [NSMutableArray arrayWithCapacity:46];     /* prefect */
    DBO dbo = RouteUtil::Enum_company_prefect();
    int ident;
	if (dbo.isvalid()) {
		while (dbo.moveNext()) {
            ident = dbo.getInt(1);
            if (ident < 0x10000) {
                [companys addObject:[NSNumber numberWithInt:ident]];
            } else {
                [prefects addObject:[NSNumber numberWithInt:ident]];
            }
		}
	}
    return [NSArray arrayWithObjects:companys, prefects, nil];
    // return [[comany array] [prefect array]]
}

// Kana from Stations
//  キーワードに一致した駅名を返す
//  ひらがな昇順ソート
+ (NSArray*)KeyMatchStations:(NSString*)key
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = RouteUtil::Enum_station_match([key UTF8String]);
    if (dbo.isvalid()) {
        while (dbo.moveNext()) {
            int station_id = dbo.getInt(1);
            [stations addObject:[NSNumber numberWithInt:station_id]];
        }
    }
    return stations;
}


+ (NSInteger)GetStationId:(NSString*)name
{
    return RouteUtil::GetStationId([name UTF8String]);
}

// StationName
+ (NSString*)StationName:(NSInteger)ident
{
    return [NSString stringWithUTF8String:RouteUtil::StationName((int)ident).c_str()];
}

// StationNameEx
+ (NSString*)StationNameEx:(NSInteger)ident
{
    return [NSString stringWithUTF8String:RouteUtil::StationNameEx((int)ident).c_str()];
}

+ (NSString*)LineName:(NSInteger)ident
{
    return [NSString stringWithUTF8String:RouteUtil::LineName((int)ident).c_str()];
}

// Company or Prefect Name
+ (NSString*)CompanyOrPrefectName:(NSInteger)ident
{
    if (ident < 0x10000) {
        return [NSString stringWithUTF8String:RouteUtil::CompanyName((int)ident).c_str()];
    } else {
        return [NSString stringWithUTF8String:RouteUtil::PrefectName((int)ident).c_str()];
    }
}

// line from prefect or company
//  会社/都道府県idから路線を返す
+ (NSArray*)LinesFromCompanyOrPrefect:(NSInteger)ident
{
    NSMutableArray* lines = [NSMutableArray array];
    DBO dbo = RouteUtil::Enum_lines_from_company_prefect((int)ident);
	if (dbo.isvalid()) {
		while (dbo.moveNext()) {
#if 0
            [lines addObject:[NSDictionary dictionaryWithObject:[NSString stringWithUTF8String:dbo.getText(0).c_str()] forKey:[NSNumber numberWithInt:dbo.getInt(1)]]];
#else
            [lines addObject:[NSNumber numberWithShort:dbo.getInt(1)]];
#endif
		}
	}
    return lines;
}

//	Enum station with in company or prefect + line
//	会社or都道府県 + 路線に属する駅を列挙
+ (NSArray*)StationsWithInCompanyOrPrefectAnd:(NSInteger)ident LineId:(NSInteger)line_id
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = RouteUtil::Enum_station_located_in_prefect_or_company_and_line((int)ident, (int)line_id);
	if (dbo.isvalid()) {
		while (dbo.moveNext()) {
#if 0
            [stations addObject:[NSDictionary dictionaryWithObject:[NSString stringWithUTF8String:dbo.getText(0).c_str()] forKey:[NSNumber numberWithInt:dbo.getInt(1)]]];
#else
            [stations addObject:[NSNumber numberWithShort:dbo.getInt(1)]];
#endif
        }
	}
    return stations;
}

+ (NSArray*)LineIdsFromStation:(NSInteger)ident
{
    NSMutableArray* lines = [NSMutableArray array];
    DBO dbo = RouteUtil::Enum_line_of_stationId((int)ident);
	if (dbo.isvalid()) {
		while (dbo.moveNext()) {
            [lines addObject:[NSNumber numberWithInt:dbo.getInt(1)]];
        }
    }
    return lines;
}

+ (NSArray*)StationsIdsOfLineId:(NSInteger)lineId
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = RouteUtil::Enum_station_of_lineId((int)lineId);
    while (dbo.moveNext()) {
#if 0
        [stations addObject:[NSArray arrayWithObjects:[NSString stringWithUTF8String:dbo.getText(0).c_str()],
                             [NSNumber numberWithInt:dbo.getInt(1)],
                             [NSNumber numberWithInt:dbo.getInt(2)], nil]];
#else
        [stations addObject:[NSNumber numberWithInt:dbo.getInt(1)]];
#endif
	}
    return stations;
}

+ (NSArray*)JunctionIdsOfLineId:(NSInteger)lineId stationId:(NSInteger)station_id
{
    int stationId = (int)station_id;
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = RouteUtil::Enum_junction_of_lineId((int)lineId, (int)stationId);
    while (dbo.moveNext()) {
#if 0
        [stations addObject:[NSArray arrayWithObjects:[NSString stringWithUTF8String:dbo.getText(0).c_str()],
                             [NSNumber numberWithInt:dbo.getInt(1)],
                             [NSNumber numberWithInt:dbo.getInt(2)], nil]];
#else
        [stations addObject:[NSNumber numberWithInt:dbo.getInt(1)]];
#endif
	}
    return stations;
}

+ (NSString*)TerminalName:(NSInteger)ident
{
    return [NSString stringWithUTF8String:CalcRoute::BeginOrEndStationName((int32_t)ident).c_str()];
}

+ (BOOL)IsJunction:(NSInteger)station_id
{
    return 0 != (RouteUtil::AttrOfStationId((int)station_id) & (1 << 12));
}

+ (BOOL)IsSpecificJunction:(NSInteger)lineId stationId:(NSInteger)station_id
{
    return 0 != (RouteUtil::AttrOfStationOnLineLine((int)lineId, (int)station_id) & (1 << 31));
}

+ (NSString*)PrectNameByStation:(NSInteger)stationId
{
    return [NSString stringWithUTF8String:RouteUtil::GetPrefectByStationId((int32_t)stationId).c_str()];
}

//	駅のひらがな読み
+ (NSString*)GetKanaFromStationId:(NSInteger)ident
{
    NSString * stationName = [NSString stringWithUTF8String:RouteUtil::GetKanaFromStationId((int)ident).c_str()];
    if (!stationName) {
        return @"";
    }
    return stationName;
}

#pragma mark - Profile

+ (void)SaveToTerminalHistory:(NSString*)terminalName
{
    if (!terminalName || ([terminalName length] <= 0)) {
        return;
    }

    NSArray *storedDevices = [[NSUserDefaults standardUserDefaults] arrayForKey:@"HistoryTerminalName"];
    NSMutableArray *newDevices = [NSMutableArray arrayWithCapacity:([storedDevices count] + 1)];
    NSInteger c = 1;

    [newDevices addObject:terminalName];
    for (NSString* s in storedDevices) {
        if (![s isEqualToString:terminalName]) {
            [newDevices addObject:s];
            c++;
            if (MAX_HISTORY <= c) {
                break;
            }
        }
    }

    /* Store */
    [[NSUserDefaults standardUserDefaults] setObject:newDevices forKey:@"HistoryTerminalName"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

//  履歴配列を追加保存する
//
//
+ (void)SaveToTerminalHistoryWithArray:(NSArray*)historyAry
{
    /* Store */
    [[NSUserDefaults standardUserDefaults] setObject:historyAry forKey:@"HistoryTerminalName"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

//  不揮発メモリから履歴配列を読み込む
//
+ (NSArray*)ReadFromTerminalHistory
{
    NSArray *storedDevices = [[NSUserDefaults standardUserDefaults] arrayForKey:@"HistoryTerminalName"];

    if (![storedDevices isKindOfClass:[NSArray class]]) {
        NSLog(@"Can't find/create an array to store");
        return nil;
    }
    return storedDevices;
}

// 設定ファイルから経路配列を読み込む
//
+ (NSArray*)loadStrageRoute
{
    return [self loadFromPlistWithArray:@"route.plist"];
}

//  不揮発メモリに保持された経路配列内に指定した経路が格納されているか？
//
+ (BOOL)IsRouteInStrage:(NSString*)routeString
{
    NSArray * ary = [self loadStrageRoute];
    return [ary containsObject:routeString];
}

//  経路情報文字列を不揮発メモリへ書き込む（最大MAX_ARCHIVE_ROUTE未満を切り詰め）
//
+ (NSInteger)saveToRouteArray:(NSArray*)routeList
{
    if (MAX_ARCHIVE_ROUTE < [routeList count]) {
        NSMutableArray* writeRouteList = [NSMutableArray arrayWithArray:routeList];
        while (MAX_ARCHIVE_ROUTE < [writeRouteList count]) {
            [writeRouteList removeLastObject];
        }
        [self saveToPlistWithArray:writeRouteList filename:@"route.plist"];
    } else {
        [self saveToPlistWithArray:routeList filename:@"route.plist"];
    }
    return [routeList count];
}

//  経路を文字列で返す（１経路１行✖️n行='\n')
//
+ (NSString*)scriptFromRouteArray:(NSArray*)routeList
{
    return [routeList componentsJoinedByString:@"\n"];
}

// 保存経路を文字列で返す（１経路１行✖️n行='\n')
+ (NSString* )scriptFromRouteArray
{
    NSArray *routeList = [self loadStrageRoute];
    return [self scriptFromRouteArray:routeList];
}

// 経路文字列を保存する(経路が有効かはチェックする)
// 戻り値に失敗した行数を返す(0を返すと成功)
//
+ (NSInteger)importRouteScripts:(NSString*)routeList
{
    NSArray *stringArray = [routeList componentsSeparatedByString: @","];
    
    return [self saveToRouteArray:stringArray];
}

+ (NSInteger*)parseScript:(NSString*)route_script
{
    return 0;
}

//  設定ファイルへ保存
//
//
+ (void)saveToPlistWithArray:(NSArray *)array filename:(NSString*)filename
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *directory = [paths objectAtIndex:0];
    NSString *filePath = [directory stringByAppendingPathComponent:filename];

    BOOL successful = [array writeToFile:filePath atomically:NO];

    if (successful) {
        // success
    }
}

//  設定ファイルから読み出し
//
//
+ (NSArray*)loadFromPlistWithArray:(NSString*)filename
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *directory = [paths objectAtIndex:0];
    NSString *filePath = [directory stringByAppendingPathComponent:filename];
    NSArray *array = [[NSArray alloc] initWithContentsOfFile:filePath];
    NSMutableArray* newArray = [[NSMutableArray alloc] initWithCapacity:[array count]];

    // 旧リリース済みバージョンには改行コード付きで保持しているのがあるので除去
    for (NSString* s in array) {
        [newArray addObject:[s stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]];
    }
    return newArray;
}

// generic parameter

+ (void)SaveToKey:(NSString*)key Value:(NSString*) value sync:(BOOL) sync
{
    /* Store */
    [[NSUserDefaults standardUserDefaults] setObject:value forKey:key];

    if (sync) {
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
}

// Retrieve database index
+ (NSString* )ReadFromKey:(NSString*)name
{
    return
    [[NSUserDefaults standardUserDefaults] stringForKey:name];
}



@end
/* cRouteUtil */

/////////////////////////////////////////////////////////////////////////////
#pragma mark - cRouteList

@implementation cRouteList

/*
-(id)init
{
    self = [super init];
    obj_route_list = new routeList();
    return self;
}
*/

-(id)initWithRoute:(cRoute*)source
{
    self = [super init];
    obj_route_list = new RouteList(*source->obj_route);
    return self;
}

-(id)initWithRouteList:(cRouteList*)source
{
    self = [super init];
    obj_route_list = new RouteList(*source->obj_route_list);
    return self;
}

- (void)dealloc
{
    delete obj_route_list;
    // [super dealloc];
}

// Identifier of Start Terminal
// 開始駅id
- (NSInteger)startStationId
{
    return obj_route_list->departureStationId();
}

- (NSInteger)lastStationId
{
    return obj_route_list->routeList().back().stationId;
}

- (NSString*)routeScript
{
    return [NSString stringWithUTF8String:obj_route_list->route_script().c_str()];
}

@end
/* end of cRouteList */

//////////////////////////////////////////////////////////////////////////////
#pragma mark - cRoute

@implementation cRoute

-(id)init {
    self = [super init];
    obj_route = new Route();
    return self;
}

// 複製初期化
- (id)initWithRoute:(cRoute*)source
{
    return [self initWithRoute:source count: -1];
}

- (id)initWithRouteList:(cRouteList*)source
{
    if (self = [super init]) {
        obj_route = new Route();
        [self assign:source];
        return self;
    }
    return nil;
}


- (id)initWithRoute:(cRoute*)source count:(NSInteger)count;
{
    if (self = [super init]) {
        obj_route = new Route();
        obj_route->assign(*source->obj_route, (int)count);
        return self;
    }
    return nil;
}

- (void)dealloc
{
    delete obj_route;
//    [super dealloc];
}

/*****
- (void*)cobj
{
    return obj_route;
}
*****/


- (void)sync:(cCalcRoute*)source
{
    obj_route->assign(*source->obj_calcroute);
}

- (void)assign:(cRouteList*)source
{
    obj_route->assign(*source->obj_route_list);
}

// 開始駅（全経路）クリア
-(void)removeAll   // removeAll() with start
{
    obj_route->removeAll();
}

// set route Start
- (NSInteger)addRoute:(NSInteger)stationId
{
    return obj_route->add((int)stationId);
}

// apeend to route
- (NSInteger)addRoute:(NSInteger)line_Id stationId:(NSInteger)stationId
{
    return obj_route->add((int)line_Id, (int)stationId);
}

- (void)removeTail
{
    obj_route->removeTail();
}

// Auto route
//	最短経路に変更(raw immidiate)
//
//	@param [in] useBulletTrain (bool)新幹線使用有無
//
//	@retval 1 : success
//	@retval 0 : loop end.(modified was to be update)
//  @retval 4 : already routed.(destroy)
//  @retval 5 : already finished.(modified was to be update)
//	@retval -n: add() error(re-track)
//	@retval -32767 unknown error(DB error or BUG)
//
- (NSInteger)autoRoute:(NSInteger)useLine arrive:(NSInteger)stationId
{
    return obj_route->changeNeerest(useLine, (int)stationId);
}

- (NSInteger)typeOfPassedLine:(NSInteger)offset
{
    return obj_route->typeOfPassedLine((int)offset);
}

// reverse
- (NSInteger)reverseRoute
{
    return obj_route->reverse();
}

/*!	@brief ルート作成(文字列からRouteオブジェクトの作成)
 *
 *	@param [in] route_str	カンマなどのデリミタで区切られた文字列("駅、路線、分岐駅、路線、..."）
 *	@retval -200 failure(駅名不正)
 *	@retval -300 failure(線名不正)
 *	@retval -1   failure(経路重複不正)
 *	@retval -2   failure(経路不正)
 *	@retval 1 success
 *	@retval 0 success
 */
- (NSInteger)setupRoute:(NSString*)routeString
{
    return obj_route->setup_route([routeString UTF8String]);
}

//- (NSInteger)getFareOption;
- (NSInteger)setDetour:(BOOL)enabled
{
    return obj_route->setDetour(enabled);
}

- (void)setNoRule:(BOOL)enabled
{
    obj_route->setNoRule(enabled);
}

// - 博多小倉間新幹線在来線別線
- (void)setNotSameKokuraHakataShinZai:(BOOL)enabled
{
    return obj_route->setNotSameKokuraHakataShinZai(enabled);
}

- (BOOL)isNotSameKokuraHakataShinZai
{
    return obj_route->isNotSameKokuraHakataShinZai();
}

// Number of Route
- (NSInteger)getRouteCount
{
    return obj_route->routeList().size();
}

// Retrieve route item
- (cRouteItem*)getRouteItem:(NSInteger)index
{
    RouteItem ri(obj_route->routeList().at(index));
    NSString* linename = [NSString stringWithUTF8String:RouteUtil::LineName(ri.lineId).c_str()];
    NSString* stationname = [NSString stringWithUTF8String:RouteUtil::StationNameEx(ri.stationId).c_str()];
    return [[cRouteItem alloc] initWithName:linename stationName:stationname];
}

// Identifier of Start Terminal
// 開始駅id
- (NSInteger)startStationId
{
    return obj_route->departureStationId();
}

// route string
// 規則非適用の表示指定の経路文字列をカンマで区切って返す（archive保存用　setup_route()で復元可能）
- (NSString*)routeScript
{
    return [NSString stringWithUTF8String:obj_route->route_script().c_str()];
}

- (NSInteger)lastStationId
{
    return obj_route->routeList().back().stationId;
}

- (NSInteger)lastLineId
{
    return obj_route->routeList().back().lineId;
}

// Enable Reverse
- (BOOL)isReverseAllow
{
    return obj_route->isAvailableReverse();
}

- (BOOL)isEnd
{
    return obj_route->isEnd();
}

// 大阪環状線 遠回り設定か？
- (BOOL)isOsakakanDetourEnable
{
    return obj_route->getRouteFlag().is_osakakan_1pass();
}

// 大阪環状線 遠回りか近回りか？
- (BOOL)isOsakakanDetourShortcut
{
    return obj_route->getRouteFlag().osakakan_detour;
}


@end
/* End of cRoute */

//////////////////////////////////////////////////////////////
#pragma mark - cCalcRoute

@implementation cCalcRoute

/*
-(id)init {
    self = [super init];
    obj_calcroute = new CalcRoute();
    return self;
}

*/

- (id)initWithRoute:(cRoute*)source
{
    return [self initWithRoute:source count:-1];
}

- (id)initWithRoute:(cRoute*)source count:(NSInteger)count
{
    if (self = [super init]) {
        self->obj_calcroute = new CalcRoute(*source->obj_route, (int)count);
    }
    return self;
}

- (id)initWithRouteList:(cRouteList*)source
{
    if (self = [super init]) {
        self->obj_calcroute = new CalcRoute(*source->obj_route_list, -1);
    }
    return self;
}


- (void)dealloc
{
    delete obj_calcroute;
//    [super dealloc];
}

- (void)sync:(cRoute*)source
{
    obj_calcroute->assign(*source->obj_route);
}

- (void)sync:(cRoute*)source count:(NSInteger)count
{
    obj_calcroute->assign(*source->obj_route, (int)count);
}


// fare calc
- (FareInfo*)calcFare
{
    FARE_INFO fi;
    int fare_result;

    obj_calcroute->calcFare(&fi);
    switch (fi.resultCode()) {
        case 0:     // success, company begin/first or too many company
            fare_result = 0;
            break;  // OK
        case -1:    /* In completed (吉塚、西小倉における不完全ルート) */
            fare_result = 1;     //"この経路の片道乗車券は購入できません.続けて経路を指定してください."
            break;
        default:
            return nil; /* -2:empty or -3:fail */
            break;
    }

    FareInfo* result = nil;
    string str1;
    string str2;
    string notused;
    int w2;
    int w3;
    FARE_INFO::FareResult fareResult;
    const static char msgPossibleLowcost[] =
                    "近郊区間内ですので最短経路の運賃で利用可能です";
    const static char msgAppliedLowcost[] =
                    "近郊区間内ですので最安運賃の経路で計算";
    const static char msgSpecificFareApply[] = "特定区間割引運賃適用";
    //const static char msgCantMetroTicket[] = "近郊区間内ですので同一駅発着のきっぷは購入できません.";
    const static char msgCanYouNeerestStation[] = "「単駅最安」で単駅発着が選択可能です";
    const static char msgCanYouSpecificTerm[] = "「特定都区市内発着」で特定都区市内発着が選択可能です";
    NSMutableArray* resultMessage = [NSMutableArray array];

    result = [[FareInfo alloc] init];

    result.result = fare_result;        /* calculate result */
    result.isResultCompanyBeginEnd = fi.isBeginEndCompanyLine();
    result.isResultCompanyMultipassed = fi.isMultiCompanyLine();

    //result.isEnableTokaiStockSelect = fi.isEnableTokaiStockSelect();


    /* begin/end terminal */
    result.beginStationId = fi.getBeginTerminalId();
    result.endStationId = fi.getEndTerminalId();

    result.isBeginInCity = FARE_INFO::IsCityId(fi.getBeginTerminalId());
    result.isEndInCity = FARE_INFO::IsCityId(fi.getEndTerminalId());

    /* route */
    result.routeList = [NSString stringWithUTF8String:fi.getRoute_string().c_str()];
    result.routeListForTOICA = [NSString stringWithUTF8String:fi.getTOICACalcRoute_string().c_str()];

    /* stock discount (114 no applied) */
    w2 = fi.getFareStockDiscount(0, str1);
    w3 = fi.getFareStockDiscount(1, str2);
    [result setFareForStockDiscounts:w2 + fi.getFareForCompanyline()
                              title1:[NSString stringWithUTF8String:str1.c_str()]
                            Discount2:w3 + fi.getFareForCompanyline()
                               title2:[NSString stringWithUTF8String:str2.c_str()]];
    result.availCountForFareOfStockDiscount = fi.countOfFareStockDiscount();

    /* rule 114 */
    if (!fi.isRule114()) {
        result.rule114_salesKm = 0;
        result.rule114_calcKm = 0;
        result.isRule114Applied = NO;
    } else {
        result.isRule114Applied = YES;
        result.rule114_salesKm = fi.getRule114SalesKm();
        result.rule114_calcKm = fi.getRule114CalcKm();

        /* stock discount (114 applied) */
        w2 = fi.getFareStockDiscount(0, notused, true);
        w3 = fi.getFareStockDiscount(1, notused, true);

        [result setFareForStockDiscountsForR114:w2 + fi.getFareForCompanyline()
                                      Discount2:w3 + fi.getFareForCompanyline()];
    }

    result.isMeihanCityStartTerminalEnable = obj_calcroute->refRouteFlag().isMeihanCityEnable();
    result.isMeihanCityStart = obj_calcroute->refRouteFlag().isStartAsCity();
    result.isMeihanCityTerminal = obj_calcroute->refRouteFlag().isArriveAsCity();

    result.isRoundtrip = obj_calcroute->refRouteFlag().isRoundTrip();

    result.isRuleAppliedEnable = obj_calcroute->getRouteFlag().rule_en();
    result.isRuleApplied = !obj_calcroute->getRouteFlag().no_rule;

    //result.isJRCentralStockEnable = obj_calcroute->getRouteFlag().jrtokaistock_enable;
    //result.isJRCentralStock = obj_calcroute->getRouteFlag().jrtokaistock_applied;

    result.isEnableLongRoute = obj_calcroute->getRouteFlag().isEnableLongRoute();
    result.isLongRoute = obj_calcroute->getRouteFlag().isLongRoute();
    result.isRule115specificTerm = obj_calcroute->getRouteFlag().isRule115specificTerm();
    result.isEnableRule115 = obj_calcroute->getRouteFlag().isEnableRule115();

    result.totalSalesKm = fi.getTotalSalesKm();
    result.jrCalcKm = fi.getJRCalcKm();
    result.jrSalesKm = fi.getJRSalesKm();

    /* 会社線有無はtotalSalesKm != jrSalesKm */

    result.companySalesKm = fi.getCompanySalesKm();
    result.salesKmForHokkaido = fi.getSalesKmForHokkaido();
    result.calcKmForHokkaido = fi.getCalcKmForHokkaido();

    result.brtSalesKm = fi.getBRTSalesKm();

    result.salesKmForShikoku = fi.getSalesKmForShikoku();
    result.calcKmForShikoku = fi.getCalcKmForShikoku();


    result.salesKmForKyusyu = fi.getSalesKmForKyusyu();
    result.calcKmForKyusyu = fi.getCalcKmForKyusyu();

    // 往復割引有無
    result.isRoundtripDiscount = fi.isRoundTripDiscount();

    // 会社線部分の運賃
    result.fareForCompanyline = fi.getFareForCompanyline();

    // 普通運賃
    result.fare = fi.getFareForDisplay();
    result.farePriorRule114 = fi.getFareForDisplayPriorRule114();

    // BRT運賃
    result.fareForBRT = fi.getFareForBRT();
    result.isBRTdiscount = fi.getIsBRT_discount();

    // 往復
    fareResult = fi.roundTripFareWithCompanyLine();
    result.roundTripFareWithCompanyLine = fareResult.fare;
    if (fi.isRule114()) {
        result.roundTripFareWithCompanyLinePriorRule114 = fi.roundTripFareWithCompanyLinePriorRule114();
    }
    // IC運賃
    result.fareForIC = fi.getFareForIC();

    // 小児運賃
    result.childFare = fi.getChildFareForDisplay();
    result.roundtripChildFare = fi.roundTripChildFareWithCompanyLine();

    // 学割運賃
    if (0 < (result.academicFare = fi.getAcademicDiscountFare())) {
        result.isAcademicFare = TRUE;
        result.roundtripAcademicFare = fi.roundTripAcademicFareWithCompanyLine();
    } else {
        result.isAcademicFare = FALSE;
        result.roundtripAcademicFare = 0;
    }
    // 有効日数
    result.ticketAvailDays = fi.getTicketAvailDays();

    // make message
    if (result.isRuleApplied &&
        fi.isUrbanArea() && !obj_calcroute->refRouteFlag().isUseBullet()) {
        if (fi.getBeginTerminalId() == fi.getEndTerminalId()) {
            //[resultMessage addObject:[NSString stringWithUTF8String:msgCantMetroTicket]];
        } else if (!obj_calcroute->refRouteFlag().isEnableRule115() ||
                   !obj_calcroute->refRouteFlag().isRule115specificTerm()) {
            if (obj_calcroute->getRouteFlag().isLongRoute()) {
                [resultMessage addObject:[NSString stringWithUTF8String:msgPossibleLowcost]];
            } else {
                [resultMessage addObject:[NSString stringWithUTF8String:msgAppliedLowcost]];
            }
        }

        // 大回り指定では115適用はみない
        if (obj_calcroute->getRouteFlag().isEnableRule115() &&
            !obj_calcroute->getRouteFlag().isEnableLongRoute()) {
            if (obj_calcroute->getRouteFlag().isRule115specificTerm()) {
                [resultMessage addObject:[NSString stringWithUTF8String:
                                        msgCanYouNeerestStation]];
            } else {
                [resultMessage addObject:[NSString stringWithUTF8String:
                                         msgCanYouSpecificTerm]];
            }
        }
    }

    result.isSpecificFare = obj_calcroute->refRouteFlag().special_fare_enable; // 私鉄競合特例運賃(大都市近郊区間)
    if (result.isSpecificFare) {
        // "特定区間割引運賃適用"
        [resultMessage addObject:[NSString stringWithUTF8String:msgSpecificFareApply]];
    }
    if (result.isResultCompanyBeginEnd) {
        [resultMessage addObject:[NSString stringWithUTF8String:"会社線発着のため一枚の乗車券として発行されない場合があります."]];
    }
    if (result.isResultCompanyMultipassed) {
        /* 2017.3 以降 ここに来ることはない */
        [resultMessage addObject:[NSString stringWithUTF8String:"複数の会社線を跨っているため乗車券は通し発券できません. 運賃額も異なります."]];
    }
    //if (result.isEnableTokaiStockSelect) {
    //    [resultMessage addObject:[NSString stringWithUTF8String:"JR東海株主優待券使用オプション選択可"]];
    //}
    if (result.isBRTdiscount) {
        [resultMessage addObject:[NSString stringWithUTF8String:"BRT乗り継ぎ割引適用"]];
    }
    if (!result.isRuleApplied && result.isSpecificFare) {
        [resultMessage addObject:[NSString stringWithUTF8String:
                                  "特定区間割引運賃を適用していません"]];
    }
    if (!result.isRuleApplied
        && obj_calcroute->getRouteFlag().isAvailableRule86()) {
        [resultMessage addObject:[NSString stringWithUTF8String:
                                  "旅客営業規則第86条を適用していません"]];
    }
    if (!result.isRuleApplied
        && obj_calcroute->getRouteFlag().isAvailableRule87()) {
        [resultMessage addObject:[NSString stringWithUTF8String:
                                  "旅客営業規則第87条を適用していません"]];
    }
    if (!result.isRuleApplied
        && obj_calcroute->getRouteFlag().isAvailableRule88()) {
        [resultMessage addObject:[NSString stringWithUTF8String:
                                  "旅客営業規則第88条を適用していません"]];
    }
    if (!result.isRuleApplied
        && obj_calcroute->getRouteFlag().isAvailableRule69()) {
        [resultMessage addObject:[NSString stringWithUTF8String:
                                  "旅客営業規則第69条を適用していません"]];
    }
    if (!result.isRuleApplied
        && obj_calcroute->getRouteFlag().isAvailableRule70()) {
        [resultMessage addObject:[NSString stringWithUTF8String:
                                  "旅客営業規則第70条を適用していません"]];
    }
    if (!result.isRuleApplied
        && obj_calcroute->getRouteFlag().isAvailableRule115()) {
        [resultMessage addObject:[NSString stringWithUTF8String:
                                  "旅客営業取扱基準規程第115条を適用していません"]];
    }
    if (obj_calcroute->getRouteFlag().isAvailableRule16_5()) {
        [resultMessage addObject:[NSString stringWithUTF8String:
                                  "この乗車券はJRで発券されません. 東京メトロでのみ発券されます"]];
    }
    if (result.isRule114Applied) {
        [resultMessage addObject:[NSString stringWithFormat:
                                @"旅客営業取扱基準規程第114条適用営業キロ計算駅:%@",           [NSString stringWithUTF8String:fi.getRule114apply_terminal_station().c_str()]]];
    }
    if (obj_calcroute->refRouteFlag().compnterm) {
        [resultMessage addObject:[NSString stringWithUTF8String:
                                  "この経路の会社線通過連絡は許可されていません."]];
    }
    result.resultMessage = [resultMessage componentsJoinedByString:@"\r\n"];

    // UI結果オプションメニュー
    result.isFareOptEnabled = result.isRuleAppliedEnable ||
                              result.isJRCentralStockEnable ||
                              result.isEnableRule115 ||
                              result.isEnableLongRoute ||
                              result.isSpecificFare;
    return result;
}


// result (for sendmail)
- (NSString*)showFare
{
    FARE_INFO fi;
    obj_calcroute->calcFare(&fi);
    return [NSString stringWithUTF8String:fi.showFare(obj_calcroute->getRouteFlag()).c_str()];
}

// 近郊区間指定経路か
- (BOOL)isEnableLongRoute
{
    return obj_calcroute->getRouteFlag().isEnableLongRoute();
}


// 旅客営業取扱基準規程第115条
- (BOOL)isEnableRule115
{
    return obj_calcroute->getRouteFlag().isEnableRule115();
}

// 旅客営業取扱基準規程第115条 特定都区市内発着適用中ではなく単駅選択中か？
- (BOOL)isRule115specificTerm
{
    return obj_calcroute->getRouteFlag().isRule115specificTerm();
}

- (void)setSpecificTermRule115:(BOOL)enabled
{
    obj_calcroute->refRouteFlag().setSpecificTermRule115(enabled);
}

//- (void)setJrTokaiStockApply:(BOOL)enabled
//{
//    return obj_calcroute->refRouteFlag().setJrTokaiStockApply(enabled);
//}

- (void)setStartAsCity
{
    obj_calcroute->refRouteFlag().setStartAsCity();
}

- (void)setArriveAsCity
{
    obj_calcroute->refRouteFlag().setArriveAsCity();
}

- (void)setLongRoute:(BOOL)flag
{
    obj_calcroute->refRouteFlag().setLongRoute(flag);
}

//-- cRouteList --
// Number of Route
- (NSInteger)getRouteCount
{
    return obj_calcroute->routeList().size();
}

// Identifier of Start Terminal
// 開始駅id
- (NSInteger)startStationId
{
    return obj_calcroute->departureStationId();
}

// route string
// 規則非適用の表示指定の経路文字列をカンマで区切って返す（archive保存用　setup_route()で復元可能）
- (NSString*)routeScript
{
    return [NSString stringWithUTF8String:obj_calcroute->route_script().c_str()];
}

- (NSInteger)lastStationId
{
    return obj_calcroute->routeList().back().stationId;
}

- (BOOL)isOsakakanDetourEnable
{
    return obj_calcroute->refRouteFlag().is_osakakan_1pass();
}

- (BOOL)isOsakakanDetour
{
    return obj_calcroute->refRouteFlag().osakakan_detour;
}

@end
/* End of CalcRoute */
