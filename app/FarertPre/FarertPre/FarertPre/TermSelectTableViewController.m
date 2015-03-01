//
//  TermSelectTableViewController.m
//  発着駅選択ビュー
//
//  Created by TAKEDA, Noriyuki on 2014/07/23.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "TermSelectTableViewController.h"
#import "RouteDataController.h"
#import "SelectLineTableViewController.h"
#import "CSTableViewController.h"


@interface TermSelectTableViewController ()
@property (strong, nonatomic) NSMutableArray* termFilteredArray;
@property (strong, nonatomic) NSArray* termCompanyPrefectArray;
@property IBOutlet UISearchBar* termSearchBar;

@end

@implementation TermSelectTableViewController


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

    self.navigationController.toolbarHidden = NO;

    AppDelegate *apd = app;
    if (apd.context == CONTEXT_AUTOROUTE_VIEW) {
        //self.navigationController.title = @"着駅指定(最短経路)";
        self.title = @"着駅指定(最短経路)";
    } else {
        //self.navigationController.title = @"発駅指定";
        self.title = @"発駅指定";
    }
    // Uncomment the following line to preserve selection between presentations.
    self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;

    _termCompanyPrefectArray = [RouteDataController GetCompanyAndPrefects];
    //UIBarButtonItem *backButton = [[UIBarButtonItem alloc] initWithTitle:@"戻る" style:UIBarButtonItemStylePlain target:nil action:nil];
    //self.navigationItem.backBarButtonItem = backButton;
    //self.navigationItem.backBarButtonItem.title = @"戻る"; //これはよいけど（って効かないからだめだが）上は幅広バーになっちゃう
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];

    // 最初に表示されたときに検索バーを非表示に
//    [self.tableView setContentOffset:CGPointMake(0.0f, self.searchDisplayController.searchBar.frame.size.height)];

    // 戻ってきたときにセルの選択を解除
    [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:NO];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    NSLog(@"memory war in TermSel view");
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    if (tableView == self.searchDisplayController.searchResultsTableView) {
        return 1;
    } else {
        return 2;   /* Company, Prefect */
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    NSArray* arys = nil;

    if (tableView == self.searchDisplayController.searchResultsTableView) {
        return [_termFilteredArray count];
    } else {
        if ((section == 0) || (section == 1)) {
            arys = [_termCompanyPrefectArray objectAtIndex:section];
            return [arys count];
        }
    }
    return 0;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    if (tableView != self.searchDisplayController.searchResultsTableView) {
        switch (section) {
        case 0:
            return @"会社";
            break;
        case 1:
            return @"都道府県";
        default:
            break;
        }
    } else {
        if (5 < [_termFilteredArray count]) {
            return [NSString stringWithFormat:@"一致件数：%ld件", (long)[_termFilteredArray count]];
        }
    }
    return nil;
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = nil;
    NSArray* ary = nil;
    NSInteger ident;
    NSString* cell_identifier = @"termSearchbarCell";
 
    if (tableView == self.searchDisplayController.searchResultsTableView) {
        cell = [tableView dequeueReusableCellWithIdentifier:cell_identifier];
        if (cell == nil) {
            cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:cell_identifier];
        }
        ident = [[_termFilteredArray objectAtIndex:[indexPath row]] intValue];
        [[cell textLabel] setText:[RouteDataController StationNameEx:ident]];
        [[cell detailTextLabel] setText:[NSString stringWithFormat:@"%@(%@)",
                                         [RouteDataController StationNameKana:ident],
                                         [RouteDataController PrectNameByStation:ident]]];

    } else {
        cell = [tableView dequeueReusableCellWithIdentifier:@"termCompanyAndPrefectCell"];
        ary = [_termCompanyPrefectArray objectAtIndex:indexPath.section];
        ident = [[ary objectAtIndex:indexPath.row] intValue];
        [[cell textLabel] setText:[RouteDataController CompanyOrPrefectName:ident]];
    }
  
 // Configure the cell...
    
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


#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
    NSString* segid = [segue identifier];
    
	if ([segid isEqualToString:@"termLineSegue"]) {
        // 都道府県または会社に属する路線一覧
        SelectLineTableViewController* selectLineTblViewController = [segue destinationViewController];

        NSArray* iDs = [_termCompanyPrefectArray objectAtIndex:[[[self tableView] indexPathForSelectedRow] section]];
        selectLineTblViewController.companyOrPrefectId = [[iDs objectAtIndex:[[[self tableView] indexPathForSelectedRow] row]] intValue];
        selectLineTblViewController.baseStationId = 0;
        selectLineTblViewController.lastLineId = 0;

	} else if ([segid isEqualToString:@"toHistorySegue"]) {
        // 過去指定 履歴駅一覧
        CSTableViewController* dvc = [segue destinationViewController];
        dvc.transit_state = FA_TRANSIT_HISTORY;
        
    } /* else termSelectDone(unwind)(検索結果から駅選択) */
      /* else termCancelSegue(unwind)(return to Main) */
}



#pragma mark Content Filtering
-(void)filterContentForSearchText:(NSString*)searchText scope:(NSString*)scope
{
    // Update the filtered array based on the search text and scope.
    // Remove all objects from the filtered search array
    [self.termFilteredArray removeAllObjects];
    
    if ((searchText != nil) && (0 < [searchText length])) {
        _termFilteredArray = [NSMutableArray arrayWithArray:[RouteDataController KeyMatchStations:searchText]];
    }
}

#pragma mark - UISearchDisplayController Delegate Methods
-(BOOL)searchDisplayController:(UISearchDisplayController *)controller shouldReloadTableForSearchString:(NSString *)searchString
{
    // Tells the table data source to reload when text changes
    [self filterContentForSearchText:searchString scope:
     [[self.searchDisplayController.searchBar scopeButtonTitles] objectAtIndex:[self.searchDisplayController.searchBar selectedScopeButtonIndex]]];
    
    // Return YES to cause the search result table view to be reloaded.
    return YES;
}

-(BOOL)searchDisplayController:(UISearchDisplayController *)controller shouldReloadTableForSearchScope:(NSInteger)searchOption
{
    // Tells the table data source to reload when scope bar selection changes
    [self filterContentForSearchText:self.searchDisplayController.searchBar.text scope:
     [[self.searchDisplayController.searchBar scopeButtonTitles] objectAtIndex:searchOption]];
    // Return YES to cause the search result table view to be reloaded.
    return YES;
}


-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (tableView == self.searchDisplayController.searchResultsTableView) {
#if 0
        if ([self.delegate respondsToSelector:@selector(terminalSelectTablebleViewDone:)]) {
            [self.delegate terminalSelectTablebleViewDone:[[termFilteredArray objectAtIndex:[indexPath row]] intValue]];
        }
        self.terminalId = [NSNumber numberWithInt:indexPath.row];
        [self dismissViewControllerAnimated:YES completion:NULL];
#endif
        AppDelegate *apd = app;
        apd.selectTerminalId = [[_termFilteredArray objectAtIndex:[indexPath row]] intValue];

        [self performSegueWithIdentifier:@"termSelectDone" sender:self];
    } else {
        // @"Select comapny or prefect");
    }
}

@end
