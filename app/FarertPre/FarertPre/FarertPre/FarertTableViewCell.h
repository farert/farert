//
//  FarertTableViewCell.h
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/07/21.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface FarertTableViewCell : UITableViewCell
@property (weak, nonatomic) IBOutlet UILabel *salesKm;
@property (weak, nonatomic) IBOutlet UILabel *lineName;
@property (weak, nonatomic) IBOutlet UILabel *stationName;
@property (weak, nonatomic) IBOutlet UILabel *fare;

@end
