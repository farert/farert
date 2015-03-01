//
//  DbSys.m
//  FarertPre
//
//  Created by TAKEDA, Noriyuki on 2015/02/25.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

#import "DbSys.h"

@implementation DbSys


- (id)initWithName:(NSString*)name sales_tax:(NSInteger)tax Date:(NSString*)date
{
    self = [super init];
    if (self) {
        _name = name;
        _tax = tax;
        _create_date = date;
        return self;
    }
    return nil;
}

@end
