/* c_route.h*/
#ifndef _c_rotute_h_
#define _c_rotute_h_

#import <Foundation/Foundation.h>
#import "alpdb.h"

#ifdef __cplusplus
class Route;
class RouteList;
class CalcRoute;
#endif
// declare CWrapped class for .m (Obj-C) files (do not use the "class" keyword.)
#ifdef __OBJC__
#ifndef __cplusplus
typedef void Route;
typedef void RouteList;
typedef void CalcRoute;
#endif
#endif


@class cRouteItem;
@class FareInfo;
@class DbSys;

typedef NS_ENUM(NSInteger, DB) {
    DB_MIN_ID = 10,
    DB_TAX5 = DB_MIN_ID,
    DB_2014,
    DB_2015,
    DB_2017,
    DB_2019_8,
    DB_2019_10,
    DB_MAX_ID = DB_2019_10,
};

#define MAX_HISTORY              20
#define MAX_ARCHIVE_ROUTE        100
#define MAX_HOLDER               60

@interface cRouteItem : NSObject

@property (strong, nonatomic, readonly) NSString* line;
@property (strong, nonatomic, readonly) NSString* station;
//@property (strong, nonatomic, readonly) NSString* fare;
//@property (strong, nonatomic, readonly) NSString* salesKm;

- (id)initWithName:(NSString*)line stationName:(NSString*)station;// salesKm:(NSString*)salesKm fare:(NSString*)fare;


@end


@interface cRouteUtil : NSObject

+ (DbSys*)DatabaseVersion;
+ (BOOL)OpenDatabase;
+ (BOOL)OpenDatabase:(enum DB)ident;
+ (void)CloseDatabase;

+ (NSString*)fareNumStr:(NSInteger)num;
+ (NSString*)kmNumStr:(NSInteger)num;

// All list of Comapny and prefects
+ (NSArray*)GetCompanyAndPrefects;

// Kana from Stations
+ (NSArray*)KeyMatchStations:(NSString*)key;

+ (NSInteger)GetStationId:(NSString*)name;
+ (NSString*)StationName:(NSInteger)ident;
+ (NSString*)StationNameEx:(NSInteger)ident;
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
+ (NSArray*)JunctionIdsOfLineId:(NSInteger)lineId stationId:(NSInteger)station_id;

+ (NSString*)TerminalName:(NSInteger)ident;
+ (BOOL)IsJunction:(NSInteger)station_id;
+ (BOOL)IsSpecificJunction:(NSInteger)lineId stationId:(NSInteger)station_id;
+ (NSString*)PrectNameByStation:(NSInteger)stationId;
+ (NSString*)GetKanaFromStationId:(NSInteger)ident;

+ (NSInteger)saveToRouteArray:(NSArray*)routeList;
+ (NSString*)scriptFromRouteArray:(NSArray*)routeList;
+ (NSString*)scriptFromRouteArray;
+ (NSInteger*)parseScript:(NSString*)route_script;

// Save databse index
+ (void)SaveToDatabaseId:(NSInteger)dbid;
+ (void)SaveToDatabaseId:(NSInteger)dbid sync:(BOOL) sync;

// Retrieve database index
+ (enum DB)GetDatabaseId;

+ (NSArray*)loadStrageRoute;
+ (BOOL)IsRouteInStrage:(NSString*)routeString;

+ (void)SaveToTerminalHistory:(NSString*)terminalName;
+ (void)SaveToTerminalHistoryWithArray:(NSArray*)historyAry;
+ (NSArray*)ReadFromTerminalHistory;
+ (NSString* )ReadFromKey:(NSString*)name;
+ (void)SaveToKey:(NSString*)key Value:(NSString*) value sync:(BOOL) sync;

@end
/* End of RouteUtil */

@class cRoute;
//////////////////////////////////////////////////////////////
@interface cRouteList : NSObject
{
@public
    RouteList * obj_route_list;
}

- (id)initWithRoute:(cRoute*)source;
//- (id)initWithRouteList:(cRouteList*)source;
// Identifier of Start Terminal
- (NSInteger)startStationId;
- (NSInteger)lastStationId;
- (NSString*)routeScript;

@end
/* End of RouteList */

@class cCalcRoute;
//////////////////////////////////////////////////////////////
@interface cRoute : NSObject
{
@public
    Route * obj_route;
}

- (id)init;
- (id)initWithRoute:(cRoute*)source;
- (id)initWithRouteList:(cRouteList*)source;
- (id)initWithRoute:(cRoute*)source count:(NSInteger)count;
- (void)sync:(cCalcRoute*)source;
- (void)assign:(cRouteList*)source;

-(void)removeAll;   // removeAll() with start

// set route Start
- (NSInteger)addRoute:(NSInteger)stationId;

// apeend to route
- (NSInteger)addRoute:(NSInteger)line_Id stationId:(NSInteger)stationId;

- (void)removeTail;

// Auto route
- (NSInteger)autoRoute:(NSInteger)useLine arrive:(NSInteger)stationId;

//
- (NSInteger)typeOfPassedLine:(NSInteger)offset;

// reverse
- (NSInteger)reverseRoute;

- (NSInteger)setupRoute:(NSString*)routeString;

- (NSInteger)setDetour:(BOOL)enabled;
- (void)setNoRule:(BOOL)enabled;

//- 小倉博多間新幹線在来線別線扱い
- (void)setNotSameKokuraHakataShinZai:(BOOL)enabled;
- (BOOL)isNotSameKokuraHakataShinZai;

// Number of Route
- (NSInteger)getRouteCount;

// Retrieve route item
- (cRouteItem*)getRouteItem:(NSInteger)index;

// Identifier of Start Terminal
- (NSInteger)startStationId;

// route string
- (NSString*)routeScript;

- (NSInteger)lastStationId;
- (NSInteger)lastLineId;

- (BOOL)isReverseAllow;

- (BOOL)isEnd;

- (BOOL)isOsakakanDetourEnable;
- (BOOL)isOsakakanDetourShortcut;

@end
/* End of cRoute */

//////////////////////////////////////////////////////////////
@interface cCalcRoute : NSObject
{
@public
    CalcRoute * obj_calcroute;
}

- (id)initWithRoute:(cRoute*)source;
- (id)initWithRoute:(cRoute*)source count:(NSInteger)count;
- (id)initWithRouteList:(cRouteList*)source;

- (void)sync:(cRoute*)source;
- (void)sync:(cRoute*)source count:(NSInteger)count;


// fare calc
- (FareInfo*)calcFare;

// result (for sendmail)
- (NSString*)showFare;

- (BOOL)isEnableLongRoute;
- (BOOL)isRule115specificTerm;
- (void)setSpecificTermRule115:(BOOL)enable;

//- (void)setJrTokaiStockApply:(BOOL)enabled;


- (void)setStartAsCity;
- (void)setArriveAsCity;

- (void)setLongRoute:(BOOL)flag;


//-- cRouteList --
// Number of Route
- (NSInteger)getRouteCount;

// Identifier of Start Terminal
- (NSInteger)startStationId;
- (NSInteger)lastStationId;
// route string
- (NSString*)routeScript;

- (BOOL)isOsakakanDetourEnable;
- (BOOL)isOsakakanDetour;

@end
/* End of cCalcRoute */

#endif /* _c_rotute_h_ */
