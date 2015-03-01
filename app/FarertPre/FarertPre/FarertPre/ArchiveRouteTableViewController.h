//
//  ArchiveRouteTableViewController.h
//  FarertPre
//
//  Created by TAKEDA, Noriyuki on 2014/08/10.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import <UIKit/UIKit.h>

@class RouteDataController;

@interface ArchiveRouteTableViewController : UITableViewController <UIActionSheetDelegate>
@property (strong, nonatomic) NSString* currentRouteString; // in
@property (strong, nonatomic) NSString* selectRouteString;  // out

@end
