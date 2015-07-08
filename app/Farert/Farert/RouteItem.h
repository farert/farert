//
//  RouteItem.h
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/07/21.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface RouteItem : NSObject

@property (strong, nonatomic, readonly) NSString* line;
@property (strong, nonatomic, readonly) NSString* station;
//@property (strong, nonatomic, readonly) NSString* fare;
//@property (strong, nonatomic, readonly) NSString* salesKm;

- (id)initWithName:(NSString*)line stationName:(NSString*)station;// salesKm:(NSString*)salesKm fare:(NSString*)fare;


@end
