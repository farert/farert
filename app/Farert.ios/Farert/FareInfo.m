//
//  FareInfo.m
//  FarertPre
//
//  Created by TAKEDA, Noriyuki on 2014/08/14.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "FareInfo.h"


@implementation FareInfo


-(id)init
{
    if (self = [super init]) {
        
    }
    return self;
}

-(void)setFareForStockDiscounts:(NSInteger)discount1 title1:(NSString*)title1 Discount2:(NSInteger)discount2 title2:(NSString*)title2
{
    if (discount1 <= 0) {
        self.availCountForFareOfStockDiscount = 0;
        fareForStockDiscountNames[0] = nil;
        fareForStockDiscountNames[1] = nil;
        fareForStockDiscounts[0] = 0;
        fareForStockDiscounts[1] = 0;
    } else {
        if (discount2 <= 0) {
            self.availCountForFareOfStockDiscount = 1;
            fareForStockDiscounts[1] = 0;
            fareForStockDiscountNames[1] = nil;
        } else {
            self.availCountForFareOfStockDiscount = 2;
            fareForStockDiscounts[1] = discount2;
            fareForStockDiscountNames[1] = title2;
        }
        fareForStockDiscounts[0] = discount1;
        fareForStockDiscountNames[0] = title1;
    }
    fareForStockDiscounts[2] = 0;
    fareForStockDiscounts[3] = 0;
}

-(void)setFareForStockDiscountsForR114:(NSInteger)discount1 Discount2:(NSInteger)discount2
{
    fareForStockDiscounts[2] = discount1;
    fareForStockDiscounts[3] = discount2;
}

// 株主割引
-(NSInteger)fareForStockDiscount:(NSInteger)index
{
    if (sizeof(fareForStockDiscountNames) <= index) {
        return 0;
    }
    return fareForStockDiscounts[index];
}

-(NSString*)fareForStockDiscountTitle:(NSInteger)index
{
    if (1 < index) {
        return @"";
    }
    return fareForStockDiscountNames[index];
}


@end
