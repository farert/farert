//
//  RouteDataController.h
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/07/20.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import <Foundation/Foundation.h>
@class obj_alpdb;
@class RouteItem;
@class FareInfo;
@class DbSys;

@interface RouteDataController : NSObject {
@private obj_alpdb* alps;
@private NSMutableArray* fareShortInfo;
}

- (id)initWithAssign:(RouteDataController*)source count:(NSInteger)count;

+ (DbSys*)DatabaseVersion;
+ (BOOL)OpenDatabase;
+ (BOOL)OpenDatabase:(NSInteger)ident;
+ (void)Close;

+ (NSString*)fareNumStr:(NSInteger)num;
+ (NSString*)kmNumStr:(NSInteger)num;
+ (void)SaveToTerminalHistory:(NSString*)terminalName;
+ (void)SaveToTerminalHistoryWithArray:(NSArray*)historyAry;
+ (NSArray*)ReadFromTerminalHistory;

// Number of Route
- (NSInteger)getRouteCount;

// Retrieve route item
- (RouteItem*)getRouteItem:(NSInteger)index;

// Identifier of Start Terminal
- (NSInteger)startStationId;

-(void)resetStartStation;   // removeAll() with start

// set route Start
- (NSInteger)addRoute:(NSInteger)stationId;

// apeend to route
- (NSInteger)addRoute:(NSInteger)line_Id stationId:(NSInteger)stationId;

- (void)removeTail;

- (NSInteger)lastStationId;
- (NSInteger)lastLineId;

// Auto route
- (NSInteger)autoRoute:(BOOL)useBullet arrive:(NSInteger)stationId;

// reverse
- (NSInteger)reverseRoute;

// route string
- (NSString*)routeScript;

// fare calc
- (FareInfo*)calcFare;


// result (for sendmail)
- (NSString*)showFare;
- (NSInteger)setupRoute:(NSString*)routeString;

// fare option
- (void)setFareOption:(NSInteger)optMask availMask:(NSInteger) availMask;
- (NSInteger)getFareOption;
- (NSInteger)setDetour:(BOOL)enabled;

//- (NSInteger)reBuild;

// All list of Comapny and prefects
+ (NSArray*)GetCompanyAndPrefects;

// Kana from Stations
+ (NSArray*)KeyMatchStations:(NSString*)key;

+ (NSInteger)GetStationId:(NSString*)name;
+ (NSString*)StationName:(NSInteger)ident;
+ (NSString*)StationNameEx:(NSInteger)ident;
+ (NSString*)StationNameKana:(NSInteger)ident;
+ (NSString*)LineName:(NSInteger)ident;
+ (NSString*)CompanyOrPrefectName:(NSInteger)ident;

// line from prefect or company
+ (NSArray*)LinesFromCompanyOrPrefect:(NSInteger)ident;
//	Enum station with in company or prefect + line
+ (NSArray*)StationsWithInCompanyOrPrefectAnd:(NSInteger)ident LineId:(NSInteger)line_id;

+ (NSArray*)LineIdsFromStation:(NSInteger)ident;
#if 0
+ (NSArray*)LinesFromStation:(NSInteger)ident;
+ (NSArray*)LinesFromStation:(NSInteger)ident exceptLine:(NSInteger)exceptLineId;
#endif

// enum stations by lineId
+ (NSArray*)StationsIdsOfLineId:(NSInteger)lineId;
// enum junction by lineId
+ (NSArray*)JunctionIdsOfLineId:(NSInteger)lineId;

+ (NSString*)TerminalName:(NSInteger)ident;
+ (int)IsJunction:(NSInteger)station_id;
+ (int)IsSpecificJunction:(NSInteger)lineId stationId:(NSInteger)station_id;
+ (NSString*)PrectNameByStation:(NSInteger)stationId;


+ (NSArray*)loadStrageRoute;
+ (BOOL)IsRouteInStrage:(NSString*)routeString;

+ (void)saveToRouteArray:(NSArray*)routeList;

// Save databse index
+ (void)SaveToDatabaseId:(NSInteger)dbid;

// Retrieve database index
+ (NSInteger)GetDatabaseId;


#if 0
+ (void)queryaggregate;
#endif

@end


enum DBIDX {
    DB_MIN_ID = 10,
    DB_TAX5 = DB_MIN_ID,
    DB_2014,
    DB_MAX_ID,
    DB_2015 = DB_MAX_ID,
};
