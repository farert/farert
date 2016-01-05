//
//  obj_alpdb.m
//  CP
//
//  Created by TAKEDA, Noriyuki on 2014/05/28.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "obj_alpdb.h"
#import "db.h"
#import "FareInfo.h"




@implementation obj_alpdb

- (id)init
{
    if (self = [super init]) {
        route = new Route;
    }
    return self;
}


- (void)dealloc
{
    delete route;

//    [super dealloc];
}

#pragma mark - Class method

+ (DbSys*)DbVer
{
    DBsys c_dbsys;
    
    if (Route::DbVer(&c_dbsys)) {
        DbSys* dbSys = [[DbSys alloc] initWithName:[NSString stringWithUTF8String:c_dbsys.name]
                                 sales_tax:c_dbsys.tax
                                      Date:[NSString stringWithUTF8String:c_dbsys.createdate]];
        return dbSys;
    } else {
        return nil;
    }
}

+ (BOOL)OpenDatabase:(NSString*)dbname
{
    NSString* dbpath = [[NSBundle mainBundle] pathForResource:dbname ofType:@"db"];

    return DBS::getInstance()->open([dbpath UTF8String]); // C++
}

+ (void)Close
{
    return DBS::getInstance()->close();
}

+ (NSString*)fareNumStr:(NSInteger)num
{
    return [NSString stringWithUTF8String:num_str_yen((int)num).c_str()];
}

+ (NSString*)kmNumStr:(NSInteger)num
{
    return [NSString stringWithUTF8String:num_str_km((int)num).c_str()];
}

/** 会社、都道府県配列を返す
 */
+ (NSArray*)Company_and_prefect
{
#if 0
    NSMutableArray* candps = [NSMutableArray array];
    DBO dbo = Route::Enum_company_prefect();
	if (dbo.isvalid()) {
		while (dbo.moveNext()) {
            [candps addObject:[NSDictionary dictionaryWithObject:[NSString stringWithUTF8String:dbo.getText(0).c_str()] forKey:[NSNumber numberWithInt:dbo.getInt(1)]]];
		}
	}
    return candps;
#else
    NSMutableArray* companys = [NSMutableArray arrayWithCapacity:6];      /* JR group */
    NSMutableArray* prefects = [NSMutableArray arrayWithCapacity:46];     /* prefect */
    DBO dbo = Route::Enum_company_prefect();
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
#endif
}

//  キーワードに一致した駅名を返す
//
#if 0 // not used
+ (NSArray*)Match_stations:(NSString*)station_name_sub
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = Route::Enum_station_match([station_name_sub UTF8String]);
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
#endif

//  キーワードに一致した駅名を返す
//  ひらがな昇順ソート

+ (NSArray*)Match_stationIds:(NSString*)station_name_sub
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = Route::Enum_station_match([station_name_sub UTF8String]);
    if (dbo.isvalid()) {
        while (dbo.moveNext()) {
            int station_id = dbo.getInt(1);
            [stations addObject:[NSNumber numberWithInt:station_id]];
        }
    }
    return stations;
}

//  会社/都道府県idから路線を返す
//
+ (NSArray*)Lines_from_company_prefect:(NSInteger)ident
{
    NSMutableArray* lines = [NSMutableArray array];
    DBO dbo = Route::Enum_lines_from_company_prefect((int)ident);
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

//	会社or都道府県 + 路線に属する駅を列挙
//
+ (NSArray*)Stations_located_in_prefect_or_company:(NSInteger)ident line:(NSInteger)line_id
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = Route::Enum_station_located_in_prefect_or_company_and_line((int)ident, (int)line_id);
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

//	駅のひらがな読み
//
+ (NSString*)GetKanaFromStationId:(NSInteger)ident
{
    NSString * stationName = [NSString stringWithUTF8String:Route::GetKanaFromStationId((int)ident).c_str()];
    if (!stationName) {
        return @"";
    }
    return stationName;
}

#if 0
// 駅の所属する路線を返す
//  [路線名, 路線ID, lflg]
+ (NSArray*)Lines_from_station:(NSInteger)ident
{
    NSMutableArray* lines = [NSMutableArray array];
    DBO dbo = Route::Enum_line_of_stationId(ident);
	if (dbo.isvalid()) {
		while (dbo.moveNext()) {
#if 0
            [lines addObject:[NSArray arrayWithObjects:[NSString stringWithUTF8String:dbo.getText(0).c_str()], [NSNumber numberWithInt:dbo.getInt(1)], [NSNumber numberWithInt:dbo.getInt(2)], nil]];
#else
            [lines addObject:[NSString stringWithUTF8String:dbo.getText(0).c_str()]];
#endif
        }
    }
    return lines;
}

+ (NSArray*)Lines_from_station:(NSInteger)ident exceptLine:(NSInteger)exceptLineId
{
    NSMutableArray* lines = [NSMutableArray array];
    DBO dbo = Route::Enum_line_of_stationId(ident);
	if (dbo.isvalid()) {
		while (dbo.moveNext()) {
            if (exceptLineId != dbo.getInt(1)) {
                [lines addObject:[NSString stringWithUTF8String:dbo.getText(0).c_str()]];
            }
        }
    }
    return lines;
}
#endif

//
+ (NSArray*)LineIds_from_station:(NSInteger)ident
{
    NSMutableArray* lines = [NSMutableArray array];
    DBO dbo = Route::Enum_line_of_stationId((int)ident);
	if (dbo.isvalid()) {
		while (dbo.moveNext()) {
            [lines addObject:[NSNumber numberWithInt:dbo.getInt(1)]];
        }
    }
    return lines;
}


+ (NSArray*)JunctionIdsOfLineId:(NSInteger)lineId stationId:(NSInteger)stationId
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = Route::Enum_junction_of_lineId((int)lineId, (int)stationId);
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

+ (NSArray*)StationsIdsOfLineId:(NSInteger)lineId
{
    NSMutableArray* stations = [NSMutableArray array];
    DBO dbo = Route::Enum_station_of_lineId((int)lineId);
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

#if 0 // not used
+ (NSInteger)NumOfNeerNode:(NSInteger)baseStationId
{
    return Route::NumOfNeerNode((int)baseStationId);
}
#endif

+ (NSInteger)GetStationId:(NSString*)name
{
    return Route::GetStationId([name UTF8String]);
}

+ (NSString*)StationName:(NSInteger)ident
{
    return [NSString stringWithUTF8String:Route::StationName((int)ident).c_str()];
}

+ (NSString*)StationNameEx:(NSInteger)ident
{
    return [NSString stringWithUTF8String:Route::StationNameEx((int)ident).c_str()];
}

+ (NSString*)LineName:(NSInteger)ident
{
    return [NSString stringWithUTF8String:Route::LineName((int)ident).c_str()];
}

+ (NSString*)CompanyName:(NSInteger)ident
{
    return [NSString stringWithUTF8String:Route::CompanyName((int)ident).c_str()];
}

+ (NSString*)PrefectName:(NSInteger)ident
{
    return [NSString stringWithUTF8String:Route::PrefectName((int)ident).c_str()];
}


+ (NSString*)TerminalName:(NSInteger)ident
{
    return [NSString stringWithUTF8String:Route::BeginOrEndStationName((int32_t)ident).c_str()];
}

+ (BOOL)IsJunction:(NSInteger)station_id
{
    return 0 != (Route::AttrOfStationId((int)station_id) & (1 << 12));
}

+ (BOOL)IsSpecificJunction:(NSInteger)lineId stationId:(NSInteger)station_id
{
    return 0 != (Route::AttrOfStationOnLineLine((int)lineId, (int)station_id) & (1 << 31));
}


+ (NSString*)PrectNameByStation:(NSInteger)stationId
{
    return [NSString stringWithUTF8String:Route::GetPrefectByStationId((int32_t)stationId).c_str()];
}

///////////////////////

#pragma mark - instance method
#if 0 // not used
// ルート選択[完了]か?
- (BOOL)isComplete:(NSInteger)curStationId
{
    return (0 < route->routeList().size()) && (route->endStationId() == curStationId);
}
#endif

// 開始駅id

- (NSInteger)startStationId;
{
    return route->startStationId();
}

// 終了駅(autoRouteで必要)
- (void)setEndStation:(NSInteger)ident
{
    route->setEndStationId((int)ident);
}

// 開始駅（全経路）クリア
- (void)resetRoute
{
    route->removeAll();
}

- (NSInteger)add:(NSInteger)startStationId
{
    return route->add((int)startStationId);
}

- (NSInteger)add:(NSInteger)lineId stationId:(NSInteger)stationId
{
    return route->add((int)lineId, (int)stationId);
}

#if 0 // not used
- (BOOL)checkPassStation:(NSInteger)stationId
{
    return route->checkPassStation((int)stationId);
}
#endif

#if 0 // not used
- (void)terminate:(NSInteger)stationId
{
    route->terminate((int)stationId);
}
#endif

- (void)removeTail
{
    route->removeTail();
}

#if 0 // not used
- (BOOL)isModified
{
    return route->isModified();
}
#endif

#if 0 // not used
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
#endif

- (NSArray*)routeItemAtIndex:(NSInteger)index
{
    RouteItem ri(route->routeList().at(index));
//    NSString* salesKm = ri.salesKm == 0 ? @"" : [obj_alpdb kmNumStr:ri.salesKm];
//    NSString* fareStr = ri.fare == 0 ? @"" : [obj_alpdb fareNumStr:ri.fare];
    return [NSArray arrayWithObjects:[NSNumber numberWithInt:ri.lineId],
                                     [NSNumber numberWithInt:ri.stationId],
                                     //salesKm,
                                     //fareStr,
            [NSNumber numberWithInt:ri.flag], nil];
}


- (NSInteger)lastRouteItemStationId
{
    return route->routeList().back().stationId;
}

- (NSInteger)lastRouteItemLineId
{
    return route->routeList().back().lineId;
}

- (NSInteger)changeNeerest:(BOOL)useBulletLine
{
    return route->changeNeerest(useBulletLine);
}

- (NSInteger)reverse
{
    return route->reverse();
}

// 規則非適用の表示指定の経路文字列をカンマで区切って返す（archive保存用　setup_route()で復元可能）
- (NSString*)routeScript
{
    return [NSString stringWithUTF8String:route->route_script().c_str()];
}

//	運賃計算オプションを得る
//	@return
//     & 0x80 = 0    : 該当なし
//     & 0x80 = 0x80 : empty or non calc.
//     & 0x03 = 0 : 無し(通常)(発・着が特定都区市内駅で特定都区市内間が100/200km以下ではない)
//			 (以下、発・着が特定都区市内駅で特定都区市内間が100/200kmを越える)
//	   & 0x03 = 0x01 : 結果表示状態は{特定都区市内 -> 単駅} (「発駅を単駅に指定」と表示)
//	   & 0x03 =	0x02 : 結果表示状態は{単駅 -> 特定都区市内} (「着駅を単駅に指定」と表示)
//     & 0x0c = 0x04 : 大阪環状線1回通過(近回り)(規定)
//     & 0x0c = 0x08 : 大阪環状線1回通過(遠回り)
- (NSInteger)getFareOption
{
    return route->getFareOption();
}

- (void)setFareOption:(NSInteger)setmask availMask:(NSInteger)availmask
{
    route->setFareOption(setmask, availmask);
}


- (NSInteger)setDetour:(BOOL)enabled
{
    return route->setDetour(enabled);
}

- (NSInteger)setDetour
{
    return route->setDetour();
}

#if 0
- (NSInteger)reBuild
{
    return route->reBuild();
}
#endif

- (void)assign:(obj_alpdb*)source count:(NSInteger)count
{
    route->assign(*source->route, (int)count);
}

- (FareInfo*)calcFare
{
    FARE_INFO fi;
    int result;
    int rc;
    
    rc = route->calcFare(&fi);
    if (rc == 0) {
        return nil; /* empty or fail */
    } else if (rc == -3) {  // 会社線のみ
        result = 2;
    } else if (rc == -2) { // 吉塚、西小倉における不完全ルート：この経路の片道乗車券は購入できません.
        result = 1;
    } else if (rc == 1) {
        result = 0; // normal
    } else {
        // unknown fail
        return nil;
    }
    return setFareInfo(fi, result, route->getFareOption());
}

FareInfo* setFareInfo(FARE_INFO& fi, int fare_result, int calcFlag)
{
    FareInfo* result = nil;
    string str1;
    string str2;
    string notused;
    int w1_notused;
    int w2;
    int w3;
    bool b_notused;

    result = [[FareInfo alloc] init];
    
    result.result = fare_result;        /* calculate result */
    result.calcResultFlag = calcFlag;   /* calculate flag */
    result.resultState = fi.getResultFlag();

    /* begin/end terminal */
    result.beginStationId = fi.getBeginTerminalId();
    result.endStationId = fi.getEndTerminalId();
    
    result.isBeginInCity = FARE_INFO::IsCityId(fi.getBeginTerminalId());
    result.isEndInCity = FARE_INFO::IsCityId(fi.getEndTerminalId());
    
    /* route */
    result.routeList = [NSString stringWithUTF8String:fi.getRoute_string().c_str()];
    
    /* stock discount (114 no applied) */
    w2 = fi.getFareStockDiscount(0, str1);
    w3 = fi.getFareStockDiscount(1, str2);
    [result setFareForStockDiscounts:w2 + fi.getFareForCompanyline()
                              title1:[NSString stringWithUTF8String:str1.c_str()]
                            Discount2:w3 + fi.getFareForCompanyline()
                               title2:[NSString stringWithUTF8String:str2.c_str()]];
    result.availCountForFareOfStockDiscount = fi.countOfFareStockDiscount();
    
    /* rule 114 */
    if (!fi.getRule114(&w1_notused, &w2, &w3)) {
        w1_notused = w2 = w3 = 0;
        result.rule114_salesKm = w2;
        result.rule114_calcKm = w3;
        result.isRule114Applied = NO;
    } else {
        result.isRule114Applied = YES;
        result.rule114_salesKm = w2;
        result.rule114_calcKm = w3;

        /* stock discount (114 applied) */
        w2 = fi.getFareStockDiscount(0, notused, true);
        w3 = fi.getFareStockDiscount(1, notused, true);
        
        [result setFareForStockDiscountsForR114:w2 + fi.getFareForCompanyline()
                                      Discount2:w3 + fi.getFareForCompanyline()];
    }
    
    result.isArbanArea = fi.isUrbanArea();
    
    
    result.totalSalesKm = fi.getTotalSalesKm();
    result.jrCalcKm = fi.getJRCalcKm();
    result.jrSalesKm = fi.getJRSalesKm();
    
    /* 会社線有無はtotalSalesKm != jrSalesKm */
    
    result.companySalesKm = fi.getCompanySalesKm();
    result.salesKmForHokkaido = fi.getSalesKmForHokkaido();
    result.calcKmForHokkaido = fi.getCalcKmForHokkaido();
    
    
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

    // 往復
    result.roundTripFareWithCompanyLine = fi.roundTripFareWithCompanyLine(b_notused);
    result.roundTripFareWithCompanyLinePriorRule114 = fi.roundTripFareWithCompanyLinePriorRule114();
    
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
    
    return result;
}

- (NSString*)showFare
{
    return [NSString stringWithUTF8String:route->showFare().c_str()];
}

- (NSInteger)setupRoute:(NSString*)routeString
{
    return route->setup_route([routeString UTF8String]);
}

- (NSInteger)getRouteCount
{
    return route->routeList().size();
}

#if 0
+(void)queryaggregate
{
    vector<string>::const_iterator i = sqlstr.cbegin();
    while (i != sqlstr.cend()) {
        NSLog(@"%s\n", i->c_str());
        ++i;
    }
}
#endif
@end








