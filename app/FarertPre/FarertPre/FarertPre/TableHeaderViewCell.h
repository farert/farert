//
//  HeaderTableViewCell.h
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/07/23.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol TableHeaderViewDelegate;


@interface TableHeaderViewCell : UITableViewCell
@property (nonatomic, weak) IBOutlet id <TableHeaderViewDelegate> delegate;

@end

@protocol TableHeaderViewDelegate <NSObject>

@optional
- (void)tableHeaderViewTouched:(TableHeaderViewCell *)tableHeaderViewCell;
@end
