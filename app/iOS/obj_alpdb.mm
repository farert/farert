//
//  obj_alpdb.m
//  CP
//
//  Created by TAKEDA, Noriyuki on 2014/05/28.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "obj_alpdb.h"
#import "db.h"

@implementation obj_alpdb

- (id)init
{
    if (self = [super init]) {
        route = new Route();
    }
    return self;
}

#if 0
- (void)dealloc
{
    delete route;
    [super dealloc];
}
#endif


/** 会社、都道府県配列を返す
 */
- (NSArray*)company_and_prefect
{
    NSMutableArray* candps = [NSMutableArray array];
    DBO dbo = route->Enum_company_prefect();
	if (dbo.isvalid()) {
		while (dbo.moveNext()) {
            [candps addObject:[NSDictionary dictionaryWithObject:[NSString stringWithUTF8String:dbo.getText(0).c_str()] forKey:[NSNumber numberWithInt:dbo.getInt(1)]]];
		}
	}
    return candps;
}

//  ひらがなに一致した駅名を返す
//
- (NSArray*)match_stations:(NSString*)station_name_sub
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = route->Enum_station_match([station_name_sub UTF8String]);
    if (dbo.isvalid()) {
        while (dbo.moveNext()) {
            string station(dbo.getText(0));
            int station_id = dbo.getInt(1);
            string same(dbo.getText(2));
            if (!same.empty()) {
                station += same + "[";
                station += Route::GetPrefectByStationId(station_id);
                station += "]";
            }
            [stations addObject:[NSDictionary dictionaryWithObject:[NSString stringWithUTF8String:station.c_str()] forKey:[NSNumber numberWithInt:station_id]]];
       }
    }
    return stations;
}

//  会社/都道府県idから路線を返す
//
+ (NSArray*)Lines_from_company_prefect:(NSInteger)ident
{
    NSMutableArray* lines = [NSMutableArray array];
    DBO dbo = Route::Enum_lines_from_company_prefect(ident);
	if (dbo.isvalid()) {
		while (dbo.moveNext()) {
            [lines addObject:[NSDictionary dictionaryWithObject:[NSString stringWithUTF8String:dbo.getText(0).c_str()] forKey:[NSNumber numberWithInt:dbo.getInt(1)]]];
		}
	}
    return lines;
}

//	会社or都道府県 + 路線に属する駅を列挙
//
+ (NSArray*)Stations_located_in_prefect_or_company:(NSInteger)ident line:(NSInteger)line_id
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = Route::Enum_station_located_in_prefect_or_company_and_line(ident, line_id);
	if (dbo.isvalid()) {
		while (dbo.moveNext()) {
            [stations addObject:[NSDictionary dictionaryWithObject:[NSString stringWithUTF8String:dbo.getText(0).c_str()] forKey:[NSNumber numberWithInt:dbo.getInt(1)]]];
		}
	}
    return stations;
}

//	駅の所属路線を得る
//
+ (NSString*)GetKanaFromStationId:(NSInteger)ident
{
    NSString * stationName = [NSString stringWithUTF8String:Route::GetKanaFromStationId(ident).c_str()];
    return stationName;
}

// 駅の所属する路線を返す
//  [路線名, 路線ID, lflg]
+ (NSArray*)Lines_from_station:(NSInteger)ident
{
    NSMutableArray* lines = [NSMutableArray array];
    DBO dbo = Route::Enum_line_of_stationId(ident);
	if (!dbo.isvalid()) {
		[lines addObject:[NSArray arrayWithObjects:[NSString stringWithUTF8String:dbo.getText(0).c_str()], [NSNumber numberWithInt:dbo.getInt(1)], [NSNumber numberWithInt:dbo.getInt(2)], nil]];
	}
    return lines;
}

///////////////////////

// ルート選択[完了]か?
- (BOOL)isComplete:(NSInteger)curStationId
{
    return (0 < route->routeList().size()) && (route->endStationId() == curStationId);
}

- (NSInteger)startStation;
{
    return route->startStationId();
}

- (void)setEndStation:(NSInteger)ident
{
    route->setEndStationId(ident);
}

- (NSInteger)endStationId
{
    return route->endStationId();
}


+ (NSInteger)NumOfNeerNode:(NSInteger)baseStationId
{
    return Route::NumOfNeerNode(baseStationId);
}

- (NSInteger)add:(NSInteger)startStationId
{
    return route->add(startStationId);
}

- (NSInteger)add:(NSInteger)lineId stationId:(NSInteger)stationId
{
    return route->add(lineId, stationId);
}

- (BOOL)checkPassStation:(NSInteger)stationId
{
    return route->checkPassStation(stationId);
}

- (void)terminate:(NSInteger)stationId
{
    route->terminate(stationId);
}

- (void)removeTail
{
    route->removeTail();
}

- (BOOL)isModified
{
    return route->isModified();
}

- (NSArray*)routeItems
{
    NSMutableArray* items = [NSMutableArray array];
	vector<RouteItem>::const_iterator pos = route->routeList().cbegin();
	ASSERT((pos->lineId == 0) && (pos->stationId == route->startStationId()));
	pos++;
	for (; pos != route->routeList().cend(); pos++) {
        [items addObject:[NSArray arrayWithObjects:
                          [NSNumber numberWithInt:pos->lineId],
                          [NSNumber numberWithInt:pos->stationId], nil]];
	}
    return items;
}

- (NSInteger)lastRouteItemStationId:(NSInteger)stationId
{
    return route->routeList().back().stationId;
}

- (NSInteger)lastRouteItemLineId:(NSInteger)lineId
{
    return route->routeList().back().lineId;
}

- (void)end
{
    route->end();
}

- (NSInteger)changeNeerest:(BOOL)useBulletLine
{
    return route->changeNeerest(useBulletLine);
}

- (NSArray*)junctionsOfLineId:(NSInteger)lineId stationId:(NSInteger)stationId
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = Route::Enum_junction_of_lineId(lineId, stationId);
    while (dbo.moveNext()) {
        [stations addObject:[NSArray arrayWithObjects:[NSString stringWithUTF8String:dbo.getText(0).c_str()],
                             [NSNumber numberWithInt:dbo.getInt(1)],
                             [NSNumber numberWithInt:dbo.getInt(2)], nil]];
	}
    return stations;
}

- (NSArray*)stationsOfLineId:(NSInteger)lineId
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = Route::Enum_station_of_lineId(lineId);
    while (dbo.moveNext()) {
        [stations addObject:[NSArray arrayWithObjects:[NSString stringWithUTF8String:dbo.getText(0).c_str()],
                             [NSNumber numberWithInt:dbo.getInt(1)],
                             [NSNumber numberWithInt:dbo.getInt(2)], nil]];
	}
    return stations;
}

+ (NSString*)StationName:(NSInteger)ident
{
    return [NSString stringWithUTF8String:Route::StationName(ident).c_str()];
}

+ (NSString*)LineName:(NSInteger)ident
{
    return [NSString stringWithUTF8String:Route::LineName(ident).c_str()];
}

- (NSInteger)reverse
{
    return route->reverse();
}

- (NSString*)routeScript
{
    return [NSString stringWithUTF8String:route->route_script().c_str()];
}

- (NSInteger)fareCalcOption
{
    return route->fareCalcOption();
}

- (NSString*)shoaFare:(NSInteger)flag
{
    return [NSString stringWithUTF8String:route->showFare(flag).c_str()];
}

- (NSInteger)setupRoute:(NSString*)routeString
{
    return route->setup_route([routeString UTF8String]);
}


@end








