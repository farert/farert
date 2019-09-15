//
//  DbSys.h
//  FarertPre
//
//  Created by TAKEDA, Noriyuki on 2015/02/25.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DbSys : NSObject

@property (nonatomic) NSString* create_date;
@property (nonatomic) NSInteger tax;
@property (nonatomic) NSString* name;

- (id)initWithName:(NSString*)name sales_tax:(NSInteger)tax Date:(NSString*)date;

@end
