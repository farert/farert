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

/*
 * getFareOption()
 */
@property (nonatomic) NSInteger calcResultFlag;
//	calcResultFlag
//     & 0x80 = 0    : 該当なし
//     & 0x80 = 0x80 : empty or non calc.
//     & 0x03 = 0 : 無し(通常)(発・着が特定都区市内駅で特定都区市内間が100/200km以下ではない)
//			 (以下、発・着が特定都区市内駅で特定都区市内間が100/200kmを越える)
//	   & 0x03 = 0x01 : 結果表示状態は{特定都区市内 -> 単駅} (「発駅を単駅に指定」と表示)
//	   & 0x03 =	0x02 : 結果表示状態は{単駅 -> 特定都区市内} (「着駅を単駅に指定」と表示)
//     & 0x0c = 0x04 : 大阪環状線1回通過(近回り)(規定)
//     & 0x0c = 0x08 : 大阪環状線1回通過(遠回り)
//     & 0x
// (System->User)
- (BOOL)isMeihanCityStartTerminalEnable;
- (BOOL)isMeihanCityStart;
- (BOOL)isMeihanCityTerminal;

// bit 2-3
- (BOOL)isOsakakanDetourEnable;

// TRUE: Detour / FALSE: Shortcut
- (BOOL)isOsakakanDetourShortcut;

// bit 4-5
- (BOOL)isRuleAppliedEnable;
- (BOOL)isRuleApplied;
- (BOOL)isFareOptEnabled;

// bit 8-9
- (BOOL)isJRCentralStockEnable;
- (BOOL)isJRCentralStock;



@property (nonatomic) NSInteger resultState;
// 1 会社線で始まっている
// 2 会社線で終わっている
// 3 会社線のみ
// 4 会社線２回以上通過
// 8 不完全経路（未使用というか前で弾く）
@property (nonatomic) BOOL isResultCompanyBeginEnd;
@property (nonatomic) BOOL isResultCompanyMultipassed;

@property (nonatomic) BOOL isEnableTokaiStockSelect;

@property (nonatomic) NSInteger beginStationId;
@property (nonatomic) NSInteger endStationId;

@property (nonatomic) BOOL isBeginInCity;
@property (nonatomic) BOOL isEndInCity;

@property (nonatomic) NSInteger availCountForFareOfStockDiscount;
@property (nonatomic) NSInteger rule114_salesKm;
@property (nonatomic) NSInteger rule114_calcKm;
@property (nonatomic) BOOL isRule114Applied;
@property (nonatomic) BOOL isArbanArea;


@property (nonatomic) NSInteger totalSalesKm;
@property (nonatomic) NSInteger jrCalcKm;
@property (nonatomic) NSInteger jrSalesKm;
/* 会社線有無はtotalSalesKm != jrSalesKm */

@property (nonatomic) NSInteger companySalesKm;
@property (nonatomic) NSInteger salesKmForHokkaido;
@property (nonatomic) NSInteger calcKmForHokkaido;


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

// 有効日数
@property (nonatomic) NSInteger ticketAvailDays;

// 経路
@property (nonatomic) NSString* routeList;



@end
