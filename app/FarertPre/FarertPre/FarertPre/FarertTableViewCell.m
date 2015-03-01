//
//  FarertTableViewCell.m
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/07/21.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "FarertTableViewCell.h"

@implementation FarertTableViewCell

@synthesize salesKm, lineName, stationName, fare;

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        // Initialization code
    }
    return self;
}

- (void)awakeFromNib
{
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
