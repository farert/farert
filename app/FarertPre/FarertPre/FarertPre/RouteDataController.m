//
//  RouteDataController.m
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/07/20.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "RouteDataController.h"
#include "obj_alpdb.h"
#import "RouteItem.h"

extern int g_tax; /* main.m */

@class FareInfo;

@interface RouteDataController()

@end


@implementation RouteDataController

- (void)initializeRouteData
{
    alps = [[obj_alpdb alloc] init];
}


- (id)init {
    if (self = [super init]) {
        [self initializeRouteData];
        return self;
    }
	return nil;
}

// 複製初期化
- (id)initWithAssign:(RouteDataController*)source count:(NSInteger)count
{
    if (self = [super init]) {
        [self initializeRouteData];
        [alps assign:source->alps count:count];
        return self;
    }
    return nil;
}


+ (DbSys*)DatabaseVersion
{
    return [obj_alpdb DbVer];
}

+ (BOOL)OpenDatabase
{
    return [self OpenDatabase:[self GetDatabaseId]];
}

+ (BOOL)OpenDatabase:(NSInteger)ident
{
    NSString* dbname;
    
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
        default:
            dbname = @"jrdb2015";
            g_tax = 8;
            break;
    }
    return [obj_alpdb OpenDatabase:dbname];
}

+ (void)Close
{
    [obj_alpdb Close];
}

- (NSInteger)getRouteCount
{
    return [alps getRouteCount];
}

- (RouteItem*)getRouteItem:(NSInteger)index
{
    NSArray* aRoute = [alps routeItemAtIndex:index];
    NSString* linename = [obj_alpdb LineName:[[aRoute objectAtIndex:0] intValue]];
    NSString* stationname = [obj_alpdb StationNameEx:[[aRoute objectAtIndex:1] intValue]];
//    NSString* salesKm = [aRoute objectAtIndex:2];
//    NSString* fare = [aRoute objectAtIndex:3];
    return [[RouteItem alloc] initWithName:linename stationName:stationname]; // salesKm:salesKm fare:fare];
}

- (NSInteger)startStationId
{
    return [alps startStationId];
}

-(void)resetStartStation
{
    return [alps resetRoute];
}

- (NSInteger)addRoute:(NSInteger)stationId
{
    return [alps add:stationId];
}

- (NSInteger)addRoute:(NSInteger)line_Id stationId:(NSInteger)stationId
{
    return [alps add:line_Id stationId:stationId];
}

- (void)removeTail
{
    [alps removeTail];
}


- (NSInteger)lastStationId
{
    return [alps lastRouteItemStationId];
}

- (NSInteger)lastLineId
{
    return [alps lastRouteItemLineId];
}

// Auto route
- (NSInteger)autoRoute:(BOOL)useBullet arrive:(NSInteger)stationId
{
    [alps setEndStation:stationId];
    return [alps changeNeerest:useBullet];
}

// reverse
- (NSInteger)reverseRoute
{
    return [alps reverse];
}

// 運賃計算
- (FareInfo*)calcFare
{
    FareInfo* fi = [alps calcFare];
    if (!fi) {
        return nil;
    }
    return fi;
}

//  運賃計算オプション
//
- (void)setFareOption:(NSInteger)fareFlag availMask:(NSInteger) availMask
{
    [alps setFareOption:fareFlag availMask:availMask];
}

- (NSInteger)getFareOption
{
    return [alps getFareOption];
}

// 大阪環状線 遠回り／近回り
- (NSInteger)setDetour:(BOOL)enabled
{
    return [alps setDetour:enabled];
}

//  経路再構築（大阪環状線周り変更）
#if 0
- (NSInteger)reBuild
{
    return [alps reBuild];
}
#endif


- (NSString*)showFare
{
    return [alps showFare];
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
    return [alps setupRoute:routeString];
}

//
//
- (NSString*)routeScript
{
    return [alps routeScript];
}


+ (NSString*)fareNumStr:(NSInteger)num
{
    return [obj_alpdb fareNumStr:num];
}

+ (NSString*)kmNumStr:(NSInteger)num
{
    return  [obj_alpdb kmNumStr:num];
}

// All list of Comapny and prefects
+ (NSArray*)GetCompanyAndPrefects
{
    return [obj_alpdb Company_and_prefect];
}

// Kana from Stations
+ (NSArray*)KeyMatchStations:(NSString*)key
{
    return [obj_alpdb Match_stationIds:key];
}

+ (NSInteger)GetStationId:(NSString*)name
{
    return [obj_alpdb GetStationId:name];
}

// StationName
+ (NSString*)StationName:(NSInteger)ident
{
    return [obj_alpdb StationName:ident];
}

// StationNameEx
+ (NSString*)StationNameEx:(NSInteger)ident
{
    return [obj_alpdb StationNameEx:ident];
}

+ (NSString*)StationNameKana:(NSInteger)ident
{
    return [obj_alpdb GetKanaFromStationId:ident];
}

+ (NSString*)LineName:(NSInteger)ident
{
    return [obj_alpdb LineName:ident];
}

// Company or Prefect Name
+ (NSString*)CompanyOrPrefectName:(NSInteger)ident
{
    if (ident < 0x10000) {
        return [obj_alpdb CompanyName:ident];
    } else {
        return [obj_alpdb PrefectName:ident];
    }
}

// line from prefect or company
+ (NSArray*)LinesFromCompanyOrPrefect:(NSInteger)ident
{
    return [obj_alpdb Lines_from_company_prefect:ident];
}

//	Enum station with in company or prefect + line
+ (NSArray*)StationsWithInCompanyOrPrefectAnd:(NSInteger)ident LineId:(NSInteger)line_id
{
    return [obj_alpdb Stations_located_in_prefect_or_company:ident line:line_id];
}

#if 0
// Line list of station
+ (NSArray*)LinesFromStation:(NSInteger)ident
{
    return [obj_alpdb Lines_from_station:ident];
}

// Line list of station
+ (NSArray*)LinesFromStation:(NSInteger)ident exceptLine:(NSInteger)exceptLineId
{
    return [obj_alpdb Lines_from_station:ident exceptLine:exceptLineId];
}
#endif

+ (NSArray*)LineIdsFromStation:(NSInteger)ident
{
    return [obj_alpdb LineIds_from_station:ident];
}

+ (NSArray*)StationsIdsOfLineId:(NSInteger)lineId
{
    return [obj_alpdb StationsIdsOfLineId:lineId ];
}

+ (NSArray*)JunctionIdsOfLineId:(NSInteger)lineId
{
    return [obj_alpdb JunctionIdsOfLineId:lineId stationId:0];
}

+ (NSString*)TerminalName:(NSInteger)ident
{
    return [obj_alpdb TerminalName:ident];
}

+ (int)IsJunction:(NSInteger)station_id
{
    return [obj_alpdb IsJunction:station_id];
}

+ (int)IsSpecificJunction:(NSInteger)lineId stationId:(NSInteger)station_id
{
    return [obj_alpdb IsSpecificJunction:lineId stationId:station_id];
}

+ (NSString*)PrectNameByStation:(NSInteger)stationId
{
    return [obj_alpdb PrectNameByStation:stationId];
}



////////////////////////////////////////////////////////////////////////////////////////////

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

// Save databse index
+ (void)SaveToDatabaseId:(NSInteger)dbid
{
    /* Store */
    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithInteger:dbid] forKey:@"JrDatabaseId"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

// Retrieve database index
+ (NSInteger)GetDatabaseId
{
    return [[NSUserDefaults standardUserDefaults] integerForKey:@"JrDatabaseId"];
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
+ (void)saveToRouteArray:(NSArray*)routeList
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

#if 0
+ (void)queryaggregate
{
    [obj_alpdb queryaggregate];
}
#endif

@end
