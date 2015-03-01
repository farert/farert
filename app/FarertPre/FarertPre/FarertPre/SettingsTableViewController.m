//
//  SettingsTableViewController.m
//  FarertPre 設定ビュー
//
//  Created by TAKEDA, Noriyuki on 2015/02/17.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

#import "SettingsTableViewController.h"
#import "RouteDataController.h"



@interface SettingsTableViewController ()
{
    NSInteger before_dbid_idx;
}


@property (weak, nonatomic) IBOutlet UISegmentedControl *sgmDataVer;


@end

@implementation SettingsTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    self.navigationController.toolbarHidden = NO;

    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
    
    before_dbid_idx = [RouteDataController GetDatabaseId];
    if ((before_dbid_idx < DB_MIN_ID) || (DB_MAX_ID < before_dbid_idx)) {
        before_dbid_idx = DB_MAX_ID;
    }
    
    _selectDbId = -1;   /* is no select */
    
    _sgmDataVer.selectedSegmentIndex = before_dbid_idx - DB_MIN_ID;
}



- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)segmentDataVersion:(id)sender
{
#if 0
    NSInteger sel_dbidx;
    
    UISegmentedControl* segctl = (UISegmentedControl*)sender;
    sel_dbidx = segctl.selectedSegmentIndex;
#endif
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:YES];
    NSLog(@"viewWillAppear");
}


- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:YES];
    NSLog(@"viewDidAppear");
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:YES];
    NSLog(@"viewWillDisappear");
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:YES];
    NSLog(@"viewDidDisappear");
}

// return to MainTableView
//
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    if ([[segue identifier] isEqualToString:@"settingsSegue"]) {
        NSInteger dbid = _sgmDataVer.selectedSegmentIndex + DB_MIN_ID;
        if ((dbid < DB_MIN_ID) || (DB_MAX_ID < dbid)) {
            dbid = DB_MAX_ID;
        }
        if (before_dbid_idx != dbid) {
            [RouteDataController SaveToDatabaseId:dbid];
            [RouteDataController Close];
            [RouteDataController OpenDatabase:dbid];
            _selectDbId = dbid;
        } else {
            _selectDbId = -1;   /* no change */
        }
    }
}

#pragma mark - Table view data source
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    if (section == 0) {
        return @"データソース";
    }
    return @"";
}

@end
