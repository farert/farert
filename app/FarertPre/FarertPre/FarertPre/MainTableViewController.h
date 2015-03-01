//
//  MainTableViewController.h
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/07/14.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "TableHeaderViewCell.h"
#import "TermSelectTableViewController.h"


@interface MainTableViewController : UITableViewController <TableHeaderViewDelegate/*TerminalSelectTableViewDelegate*/, UIActionSheetDelegate>

- (IBAction)doneTerminal:(UIStoryboardSegue *)segue;
- (IBAction)cancelTerminal:(UIStoryboardSegue *)segue;
- (IBAction)closeModal:(UIStoryboardSegue *)segue;


@end
