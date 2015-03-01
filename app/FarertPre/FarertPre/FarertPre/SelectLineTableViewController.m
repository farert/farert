//
//  SelectLineTableViewController.m
//  路線選択ビュー
//
//  Created by TAKEDA, Noriyuki on 2014/07/27.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "SelectLineTableViewController.h"
#import "RouteDataController.h"
#import "SelectStationTableViewController.h"

@interface SelectLineTableViewController ()
@property (strong, nonatomic) NSArray* lineList;
@end

@implementation SelectLineTableViewController

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Uncomment the following line to preserve selection between presentations.
    self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;

    AppDelegate *apd = app;
    if ((apd.context == CONTEXT_AUTOROUTE_VIEW) || (apd.context == CONTEXT_TERMINAL_VIEW)) {
        [self.navigationItem setRightBarButtonItems:nil animated:YES];
    }
    if (apd.context == CONTEXT_ROUTESELECT_VIEW) {
        _lineList = [RouteDataController LineIdsFromStation:_baseStationId];
    } else { /* TAG_TERMINAL_VIEW */
        _lineList = [RouteDataController LinesFromCompanyOrPrefect:_companyOrPrefectId];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    NSLog(@"memory war in Select line view");
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return [_lineList count];
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = nil;
    NSInteger lineId = [[_lineList objectAtIndex:indexPath.row] intValue];
    AppDelegate *apd = app;
    
    if ((apd.context == CONTEXT_ROUTESELECT_VIEW) && (_lastLineId == lineId)) {
        cell = [tableView dequeueReusableCellWithIdentifier:@"termLineCell2" forIndexPath:indexPath];
    } else {
        cell = [tableView dequeueReusableCellWithIdentifier:@"termLineCell" forIndexPath:indexPath];
    }
    [[cell textLabel] setText:[RouteDataController LineName:lineId]];
    return cell;
}


- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    AppDelegate *apd = app;
    
    if (apd.context == CONTEXT_ROUTESELECT_VIEW) {
        return [RouteDataController StationName:_baseStationId];
    } else {
        return [RouteDataController CompanyOrPrefectName:self.companyOrPrefectId];
    }
}

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath
{
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

// 戻ってきたときにセルの選択を解除
- (void) viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:NO];
}


#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    NSString* segid = [segue identifier];
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
    if ([segid isEqualToString:@"termLineToStationSegue"]) {
        // 駅一覧テーブルビュー(to SelectStationTableView)
        SelectStationTableViewController* selStationViewController = [segue destinationViewController];
        selStationViewController.companyOrPrefectId = self.companyOrPrefectId;
        selStationViewController.lineId = [[_lineList objectAtIndex:
                                           self.tableView.indexPathForSelectedRow.row] intValue];
        selStationViewController.lastStationId = _baseStationId;
        selStationViewController.transit_state = FA_TRANSIT_STA2JCT;
    } else if ([segid isEqualToString:@"autoRouteSegue"]) {
        // 最短経路ボタン(to TermSelectTableView)

        AppDelegate *apd = app;
        apd.context = CONTEXT_AUTOROUTE_VIEW;
        //UINavigationController* nvc = [segue destinationViewController];
        //CSTableViewController* vc = [nvc.viewControllers  objectAtIndex:0];
        //CSTableViewController* vc = [segue destinationViewController];
        //vc.transit_state = FA_TRANSIT_AUTOROUTE;
    }
}


@end
