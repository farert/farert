//
//  obj_alpdb.h
//  CP
//
//  Created by TAKEDA, Noriyuki on 2014/05/28.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DbSys.h"

// declare CWrapped class for .mm (Obj-C++) files
#ifdef __cplusplus
#include "alpdb.h"
class Route;
#endif
// declare CWrapped class for .m (Obj-C) files (do not use the "class" keyword.)
#ifdef __OBJC__
#ifndef __cplusplus
typedef void Route;
#endif
#endif

@class FareInfo;

//---------------------------------------------
@interface alps_database : NSObject
+ (alps_database*)sharedManager;
@end
//---------------------------------------------

@interface obj_alpdb : NSObject {
    Route *route;   // void* by Objective-C as un-necessery for route.h 
}

- (id)init;

+ (BOOL)OpenDatabase:(NSString*)dbname;
+ (void)Close;


+ (DbSys*)DbVer;

+ (NSString*)fareNumStr:(NSInteger)num;
+ (NSString*)kmNumStr:(NSInteger)num;

+ (NSArray*)Company_and_prefect;
// not used + (NSArray*)Match_stations:(NSString*)station_name_sub;
+ (NSArray*)Match_stationIds:(NSString*)station_name_sub;

+ (NSArray*)Lines_from_company_prefect:(NSInteger)ident;
+ (NSArray*)Stations_located_in_prefect_or_company:(NSInteger)ident line:(NSInteger)line_id;

+ (NSString*)GetKanaFromStationId:(NSInteger)ident;
//+ (NSArray*)Lines_from_station:(NSInteger)ident;
//+ (NSArray*)Lines_from_station:(NSInteger)ident exceptLine:(NSInteger)exceptLineId;
+ (NSArray*)LineIds_from_station:(NSInteger)ident;
+ (NSArray*)JunctionIdsOfLineId:(NSInteger)lineId stationId:(NSInteger)stationId;
+ (NSArray*)StationsIdsOfLineId:(NSInteger)lineId;

// not used + (NSInteger)NumOfNeerNode:(NSInteger)baseStationId;

+ (NSInteger)GetStationId:(NSString*)name;
+ (NSString*)StationName:(NSInteger)ident;
+ (NSString*)StationNameEx:(NSInteger)ident;
+ (NSString*)LineName:(NSInteger)ident;
+ (NSString*)CompanyName:(NSInteger)ident;
+ (NSString*)PrefectName:(NSInteger)ident;
+ (NSString*)TerminalName:(NSInteger)ident;
+ (BOOL)IsJunction:(NSInteger)station_id;
+ (BOOL)IsSpecificJunction:(NSInteger)lineId stationId:(NSInteger)station_id;
+ (NSString*)PrectNameByStation:(NSInteger)stationId;


// not used - (BOOL)isComplete:(NSInteger)cur_stationId;
- (NSInteger)startStationId;
- (void)setEndStation:(NSInteger)ident;

// 開始駅（全経路）クリア
- (void)resetRoute;


- (NSInteger)add:(NSInteger)startStationId;
- (NSInteger)add:(NSInteger)lineId stationId:(NSInteger)stationId;

// not used - (BOOL)checkPassStation:(NSInteger)stationId;
// not used - (void)terminate:(NSInteger)stationId;
- (void)removeTail;
// not used - (BOOL)isModified;
// not used - (NSArray*)routeItems;
- (NSInteger)lastRouteItemStationId;
- (NSInteger)lastRouteItemLineId;
- (NSInteger)changeNeerest:(BOOL)useBulletLine;

- (NSInteger)reverse;
- (NSString*)routeScript;

- (void)setFareOption:(NSInteger)setmask availMask:(NSInteger)availmask;
- (NSInteger)getFareOption;
- (NSInteger)setDetour:(BOOL)enabled;
- (NSInteger)setDetour;

//- (NSInteger)reBuild;

- (void)assign:(obj_alpdb*)source count:(NSInteger)count;

- (FareInfo*)calcFare;
- (NSString*)showFare;



- (NSInteger)setupRoute:(NSString*)routeString;

- (NSInteger)getRouteCount;
- (NSArray*)routeItemAtIndex:(NSInteger)index;

#if 0
+ (void)queryaggregate;
#endif

@end







































