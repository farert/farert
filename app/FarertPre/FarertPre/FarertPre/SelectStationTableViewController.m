//
//  SelectStationTableViewController.m
//  経路駅選択ビュー
//
//  Created by TAKEDA, Noriyuki on 2014/07/21.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "SelectStationTableViewController.h"
#import "RouteDataController.h"

@interface SelectStationTableViewController ()
@property (strong, nonatomic) NSArray* stations;

@end

@implementation SelectStationTableViewController

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
    
    //NSLog(@"didload begin: transit %d", self.transit_state);

    // Uncomment the following line to preserve selection between presentations.
    self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
#if 0
    UIBarButtonItem* btn = [[UIBarButtonItem alloc] initWithTitle:self.navigationItem.backBarButtonItem.title
                                                            style:UIBarButtonItemStylePlain
                                                           target:nil //self
                                                           action:nil]; // @selector(returnAction:)];
    self.navigationItem.backBarButtonItem = btn;
#endif
    AppDelegate *apd = app;
    if ((apd.context == CONTEXT_AUTOROUTE_VIEW) || (apd.context == CONTEXT_TERMINAL_VIEW)) {
        [self.navigationItem setRightBarButtonItems:nil animated:YES];
        _stations = [RouteDataController StationsWithInCompanyOrPrefectAnd:_companyOrPrefectId LineId:_lineId];
        if (apd.context == CONTEXT_AUTOROUTE_VIEW) {
            self.title = @"着駅指定";
        } else {
            self.title = @"発駅指定";
        }
    } else { //CONTEXT_ROUTESELECT_VIEW
        if (FA_TRANSIT_STA2JCT != self.transit_state) {
            _stations = [RouteDataController StationsIdsOfLineId:_lineId];
            self.title = @"着駅指定";
            self.navigationItem.rightBarButtonItem.title = @"分岐駅指定";
        } else {
            _stations = [RouteDataController JunctionIdsOfLineId:_lineId];
            self.title = @"分岐駅指定";
            self.navigationItem.rightBarButtonItem.title = @"着駅指定";
        }
    }
    //NSLog(@"didload last:transit %d", self.transit_state);
}
#if 0
- (void)returnAction
{
    UIViewController* backViewController = [[[self navigationController] viewControllers] objectAtIndex:1];
    [[self navigationController] popToViewController:backViewController animated:YES];
}


-(void) viewWillDisappear:(BOOL)animated
{
    if ([self.navigationController.viewControllers indexOfObject:self] == NSNotFound) {
        // back button was pressed.  We know this is true because self is no longer
        // in the navigation stack.
        //NSLog(@"viewWillDisaper+");

//        UIViewController* backViewController = [[[self navigationController] viewControllers] objectAtIndex:1];
//        [[self navigationController] popToViewController:backViewController animated:YES];
        int i;
        for (i = 0; i < [[[self navigationController] viewControllers] count]; i++) {
            UIViewController* vc = [[[self navigationController] viewControllers] objectAtIndex:i];
            //NSLog(@"www:%@", [vc title]);
        }
        return;
    } else {
        //NSLog(@"viewWillDisaper-");
    }
    [super viewWillDisappear:animated];
}
#endif

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    NSLog(@"memory war in SelectStation view");
}

- (IBAction)terminalBarButtonAction:(id)sender
{
    [self performSegueWithIdentifier:@"stationViewSegue" sender:sender];
}


#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return 1;
}

- (IBAction)switchStationBarButtonAction:(id)sender
{
    AppDelegate *apd = app;

    if (apd.context != CONTEXT_ROUTESELECT_VIEW) {
        return;
    }
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    if (0 < _companyOrPrefectId) {
        NSString *companyOrPrefctString = [RouteDataController CompanyOrPrefectName:_companyOrPrefectId];
        NSString *head = [companyOrPrefctString stringByAppendingString:@" - "];
        return [head stringByAppendingString:[RouteDataController LineName:_lineId]];
    } else {
        return [RouteDataController LineName:_lineId];
    }
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return [_stations count];
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = nil;
    NSInteger stationId = [[_stations objectAtIndex:[indexPath row]] intValue];
    NSInteger line_id;
    
    AppDelegate *apd = app;
    if ((apd.context == CONTEXT_ROUTESELECT_VIEW) && (_lastStationId == stationId)) {
            //インデント付き＋チェックマーク付き（起点駅)
        cell = [tableView dequeueReusableCellWithIdentifier:@"terminalStationCell2" forIndexPath:indexPath];
    } else {
        cell = [tableView dequeueReusableCellWithIdentifier:@"terminalStationCell" forIndexPath:indexPath];
    }
    
    // Configure the cell...;
    [[cell textLabel] setText:[RouteDataController StationName:stationId]];

    NSMutableArray* details = [[NSMutableArray alloc] initWithCapacity:5];
    [details addObject:@"("];
    [details addObject:[RouteDataController StationNameKana:stationId]];
    [details addObject:@") "];
    if ([RouteDataController IsJunction:stationId]) {    /* junction(lflag.bit12 on) */
        NSMutableArray* lines = [[NSMutableArray alloc] init];
        for (NSNumber* lineId in [RouteDataController LineIdsFromStation:stationId]) {
            line_id = [lineId intValue];
            if ((line_id != _lineId) && ![RouteDataController IsSpecificJunction:line_id stationId:stationId]) {
                [lines addObject:[RouteDataController LineName:line_id]];
            }
        }
        if (0 < [lines count]) {
            [lines addObject:[RouteDataController LineName:_lineId]];
            [details addObject:[lines componentsJoinedByString:@"/"]];
        }
    }
    [[cell detailTextLabel] setText:[details componentsJoinedByString:@""]];
    return cell;
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
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
    NSString* segid = [segue identifier];
    
    if ([segid isEqualToString:@"terminalSelectDoneSegue"]) {
        AppDelegate *apd = app;
        apd.selectTerminalId = 0xffff & [[_stations objectAtIndex:[[[self tableView] indexPathForSelectedRow] row]] intValue];
        apd.selectLineId = _lineId;

    } else if ([segid isEqualToString:@"stationViewSegue"]) {
        SelectStationTableViewController *thisViewController = (SelectStationTableViewController*)[segue destinationViewController];
        //NSLog(@"prepare segue: transit set %d-> ", self.transit_state);
        thisViewController.transit_state = (self.transit_state == FA_TRANSIT_STA2JCT) ? FA_TRANSIT_JCT2STA : FA_TRANSIT_STA2JCT;
        thisViewController.lineId = _lineId;
        thisViewController.companyOrPrefectId = _companyOrPrefectId;
        NSAssert(self.companyOrPrefectId == 0, @"bug");
        thisViewController.lastStationId = _lastStationId;
    }
}


@end
