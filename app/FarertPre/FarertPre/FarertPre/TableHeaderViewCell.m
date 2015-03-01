//
//  HeaderTableViewCell.m
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/07/23.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "TableHeaderViewCell.h"

@implementation TableHeaderViewCell

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
    // set up the tap gesture recognizer
    UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self
                                                                                 action:@selector (selTapped:)];
    [self addGestureRecognizer:tapGesture];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}


- (IBAction)selTapped:(id)sender
{
    if ([self.delegate respondsToSelector:@selector(tableHeaderViewTouched:)]) {
        [self.delegate tableHeaderViewTouched:self ];
    }
}


@end
