//
//  TermSelectTableViewController.h
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/07/23.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CSTableViewController.h"

//@protocol TerminalSelectTableViewDelegate;

@interface TermSelectTableViewController : CSTableViewController /*UITableViewController*/ <UISearchBarDelegate, UISearchDisplayDelegate>

//@property (nonatomic, weak) IBOutlet id <TerminalSelectTableViewDelegate> delegate;


@end

//@protocol TerminalSelectTableViewDelegate <NSObject>

//@optional

//- (void)terminalSelectTablebleViewDone:(NSInteger)terminalId;

//@end