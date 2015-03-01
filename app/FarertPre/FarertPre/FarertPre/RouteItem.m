//
//  RouteItem.m
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/07/21.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "RouteItem.h"
#import "obj_alpdb.h"

@implementation RouteItem

- (id)initWithName:(NSString*)line stationName:(NSString*)station //salesKm:(NSString*)salesKm fare:(NSString*)fare
{
    self = [super init];
    if (self) {
        _line = line;
        _station = station;
//        _fare = fare;
//        _salesKm = salesKm;
		return self;
	}
	return nil;
}

@end
