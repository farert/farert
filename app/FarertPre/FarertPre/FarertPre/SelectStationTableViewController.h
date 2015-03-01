//
//  SelectStationTableViewController.h
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/07/21.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CSTableViewController.h"

@interface SelectStationTableViewController : CSTableViewController // UITableViewController
// in
@property NSInteger companyOrPrefectId;
@property NSInteger lineId;
@property NSInteger lastStationId;

@end
