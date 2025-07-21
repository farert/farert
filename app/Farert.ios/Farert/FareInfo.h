//
//  FareInfo.h
//  FarertPre
//
//  Created by TAKEDA, Noriyuki on 2014/08/14.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface FareInfo : NSObject {
    NSInteger fareForStockDiscounts[2*2];
    NSString* fareForStockDiscountNames[2];

}

-(id)init;

-(void)setFareForStockDiscounts:(NSInteger)discount1 title1:(NSString*)title1 Discount2:(NSInteger)discount2 title2:(NSString*)title2;

-(void)setFareForStockDiscountsForR114:(NSInteger)discount1 Discount2:(NSInteger)discount2;

@property (nonatomic) NSInteger result; /* 0: success, 1: KOKURA-pending, 2:Company only, -1: failure */

@property (nonatomic) BOOL isFareOptEnabled;
@property (nonatomic) BOOL isMeihanCityStartTerminalEnable;
@property (nonatomic) BOOL isMeihanCityStart;
@property (nonatomic) BOOL isMeihanCityTerminal;


@property (nonatomic) BOOL isRuleAppliedEnable;
@property (nonatomic) BOOL isRuleApplied;

@property (nonatomic) BOOL isJRCentralStockEnable;
@property (nonatomic) BOOL isJRCentralStock;

@property (nonatomic) BOOL isEnableLongRoute;
@property (nonatomic) BOOL isLongRoute;
@property (nonatomic) BOOL isRule115specificTerm;
@property (nonatomic) BOOL isEnableRule115;


@property (nonatomic) NSString* resultMessage;

@property (nonatomic) BOOL isResultCompanyBeginEnd;
@property (nonatomic) BOOL isResultCompanyMultipassed;

//@property (nonatomic) BOOL isEnableTokaiStockSelect;

@property (nonatomic) NSInteger beginStationId;
@property (nonatomic) NSInteger endStationId;

@property (nonatomic) BOOL isBeginInCity;
@property (nonatomic) BOOL isEndInCity;

@property (nonatomic) NSInteger availCountForFareOfStockDiscount;
@property (nonatomic) NSInteger rule114_salesKm;
@property (nonatomic) NSInteger rule114_calcKm;
@property (nonatomic) BOOL isRule114Applied;
@property (nonatomic) BOOL isSpecificFare;
@property (nonatomic) BOOL isCompnterm;

@property (nonatomic) NSInteger totalSalesKm;
@property (nonatomic) NSInteger jrCalcKm;
@property (nonatomic) NSInteger jrSalesKm;
/* 会社線有無はtotalSalesKm != jrSalesKm */

@property (nonatomic) NSInteger companySalesKm;
@property (nonatomic) NSInteger salesKmForHokkaido;
@property (nonatomic) NSInteger calcKmForHokkaido;

@property (nonatomic) NSInteger brtSalesKm;

@property (nonatomic) NSInteger salesKmForShikoku;
@property (nonatomic) NSInteger calcKmForShikoku;


@property (nonatomic) NSInteger salesKmForKyusyu;
@property (nonatomic) NSInteger calcKmForKyusyu;

// 往復
@property (nonatomic) BOOL isRoundtrip;

// 往復割引有無
@property (nonatomic) BOOL isRoundtripDiscount;

// 会社線部分の運賃
@property (nonatomic) NSInteger fareForCompanyline;

// 普通運賃
@property (nonatomic) NSInteger fare;
@property (nonatomic) NSInteger farePriorRule114;

// BRT運賃
@property (nonatomic) NSInteger fareForBRT;
@property (nonatomic) BOOL isBRTdiscount;

// 普通運賃は, fareForJR + fareForCompanyline;
// 往復
@property (nonatomic) NSInteger roundTripFareWithCompanyLine;
@property (nonatomic) NSInteger roundTripFareWithCompanyLinePriorRule114;

// IC運賃
@property (nonatomic) NSInteger fareForIC;
        
// 株主割引
-(NSInteger)fareForStockDiscount:(NSInteger)index;

-(NSString*)fareForStockDiscountTitle:(NSInteger)index;

// 子供運賃
@property (nonatomic) NSInteger childFare;

// 学割運賃
@property (nonatomic) BOOL isAcademicFare;
@property (nonatomic) NSInteger academicFare;
@property (nonatomic) NSInteger roundtripAcademicFare;
@property (nonatomic) NSInteger roundtripChildFare;

// 有効日数
@property (nonatomic) NSInteger ticketAvailDays;

// 経路
@property (nonatomic) NSString* routeList;

@property (nonatomic) NSString* routeListForTOICA;

@end
