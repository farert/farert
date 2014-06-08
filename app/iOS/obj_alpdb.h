//
//  obj_alpdb.h
//  CP
//
//  Created by TAKEDA, Noriyuki on 2014/05/28.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import <Foundation/Foundation.h>

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



@interface obj_alpdb : NSObject {
    Route* route;
}

- (id)init;
- (NSArray*)company_and_prefect;
- (NSArray*)match_stations:(NSString*)station_name_sub;
+ (NSArray*)Lines_from_company_prefect:(NSInteger)ident;
+ (NSArray*)Stations_located_in_prefect_or_company:(NSInteger)ident line:(NSInteger)line_id;

+ (NSString*)GetKanaFromStationId:(NSInteger)ident;
+ (NSArray*)Lines_from_station:(NSInteger)ident;

- (BOOL)isComplete:(NSInteger)cur_stationId;
- (NSInteger)startStation;
- (void)setEndStation:(NSInteger)ident;
- (NSInteger)endStationId;

+ (NSInteger)NumOfNeerNode:(NSInteger)baseStationId;
- (NSInteger)add:(NSInteger)startStationId;
- (NSInteger)add:(NSInteger)lineId stationId:(NSInteger)stationId;

- (BOOL)checkPassStation:(NSInteger)stationId;
- (void)terminate:(NSInteger)stationId;
- (void)removeTail;
- (BOOL)isModified;
- (NSArray*)routeItems;
- (NSInteger)lastRouteItemStationId:(NSInteger)stationId;
- (NSInteger)lastRouteItemLineId:(NSInteger)lineId;
- (void)end;
- (NSInteger)changeNeerest:(BOOL)useBulletLine;
- (NSArray*)junctionsOfLineId:(NSInteger)lineId stationId:(NSInteger)stationId;
- (NSArray*)stationsOfLineId:(NSInteger)lineId;
+ (NSString*)StationName:(NSInteger)ident;
+ (NSString*)LineName:(NSInteger)ident;
- (NSInteger)reverse;
- (NSString*)routeScript;
- (NSInteger)fareCalcOption;
- (NSString*)shoaFare:(NSInteger)flag;
- (NSInteger)setupRoute:(NSString*)routeString;


@end







































