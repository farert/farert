//
//  TerminalHistoryTableViewController.m
//  駅指定履歴ビュー
//
//  Created by TAKEDA, Noriyuki on 2014/07/31.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "TerminalHistoryTableViewController.h"
#import "RouteDataController.h"

@interface TerminalHistoryTableViewController ()
@property (weak, nonatomic) IBOutlet UIBarButtonItem *clerBarButtonItem;
@property (strong, nonatomic) NSMutableArray* historyTerms;
@end

@implementation TerminalHistoryTableViewController

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}
#if 0
- (IBAction)editButtonAction:(id)sender
{
    [[self tableView] setEditing:YES animated:YES];
    
}
#endif

- (IBAction)clearButtonAction:(id)sender
{
    NSArray *emtpyArray = [[NSArray alloc] init];
    [RouteDataController SaveToTerminalHistoryWithArray:emtpyArray];
    [_historyTerms removeAllObjects];

    [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:0]  withRowAnimation:UITableViewRowAnimationFade];

    self.navigationItem.rightBarButtonItem.enabled = NO;
    _clerBarButtonItem.enabled = NO;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Uncomment the following line to preserve selection between presentations.
    self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    self.navigationItem.rightBarButtonItem = self.editButtonItem;
    _historyTerms = [NSMutableArray arrayWithArray:[RouteDataController ReadFromTerminalHistory]];
    if ([_historyTerms count] <= 0) {
        self.navigationItem.rightBarButtonItem.enabled = NO;
        _clerBarButtonItem.enabled = NO;
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    NSLog(@"memory war in TermHist view");
}

// 戻ってきたときにセルの選択を解除
- (void) viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:NO];
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
    return [_historyTerms count];
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = nil;
    
    if (indexPath.row < [_historyTerms count]) {
        cell = [tableView dequeueReusableCellWithIdentifier:@"termHistoryCell" forIndexPath:indexPath];
        [[cell textLabel] setText:[_historyTerms objectAtIndex:indexPath.row]];
    }
    return cell;
}


- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellEditingStyleDelete;
}

// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return (0 < [_historyTerms count]) ? YES : NO ;
}



// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView beginUpdates];
        NSInteger n = [_historyTerms count];
        if (0 < n) {
            [_historyTerms removeObjectAtIndex:[indexPath row]]; // model
            [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
        }
        [tableView endUpdates];
        if (n == 1) {
            [[self tableView] setEditing:NO];
            self.navigationItem.rightBarButtonItem.enabled = NO;
            _clerBarButtonItem.enabled = NO;
            [tableView reloadData];
        }
        [RouteDataController SaveToTerminalHistoryWithArray:_historyTerms];

   } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}

#if 0
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    if ([_historyTerms count] <= 0) {
        return @"履歴がありません.";
    } else {
        return @"";
    }
}


#else

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    UITableViewCell* cell = nil;
    if ([_historyTerms count] <= 0) {
        cell = [tableView dequeueReusableCellWithIdentifier:@"termHistoryEmptyCell"];
        return [cell contentView];
    } else {
        return cell;

    }
}
#endif

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    if (section == 0) {
        UITableViewCell* cell = nil;
        
        if ([_historyTerms count] <= 0) {
            cell = [tableView dequeueReusableCellWithIdentifier:@"termHistoryEmptyCell"];
            return cell.contentView.bounds.size.height;
        }
    }
    return UITableViewAutomaticDimension;
}

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


#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
    
    if ([[segue identifier] isEqualToString:@"doneTerminalHistorySegue"]) {
        AppDelegate *apd = app;

        NSString* station = [_historyTerms objectAtIndex:[[[self tableView] indexPathForSelectedRow] row]];
        apd.selectTerminalId = [RouteDataController GetStationId:station];
    }
}


@end
