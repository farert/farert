//
//  ArchiveRouteTableViewController.m
//  FarertPre 経路保存ビュー
//
//  Created by TAKEDA, Noriyuki on 2014/08/10.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "ArchiveRouteTableViewController.h"
#import "RouteDataController.h"

@interface ArchiveRouteTableViewController ()

@property (strong, nonatomic) NSMutableArray* routeList;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *clearBarButton;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *saveBarButton;
@property BOOL saved;

@end

@implementation ArchiveRouteTableViewController

//  Initialize (Not paased)
//
- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

//  Load view
//
//
- (void)viewDidLoad
{
    [super viewDidLoad];

    self.navigationController.toolbarHidden = NO;

    // Uncomment the following line to preserve selection between presentations.
    self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    self.navigationItem.rightBarButtonItem = self.editButtonItem;
    
    // Straged route read from filesystem
    _routeList = [NSMutableArray arrayWithArray:[RouteDataController loadStrageRoute]];
    if (0 < [_currentRouteString length]) {
        if ([_routeList containsObject:_currentRouteString]) {      // is exist ?
            [_routeList removeObject:_currentRouteString];          // 入れ替え
            [_routeList insertObject:_currentRouteString atIndex:0];
            [RouteDataController saveToRouteArray:_routeList];      // して保存
            _saveBarButton.enabled = NO;
            _saved = true;
        } else {                                            // non exist
            [_routeList insertObject:_currentRouteString atIndex:0];
            _saveBarButton.enabled = YES;
            _saved = false;
        }
    } else {
        _saveBarButton.enabled = NO;
        _saved = true;
    }
    if ([_routeList count] <= 0) {
        self.navigationItem.rightBarButtonItem.enabled = NO;
        _clearBarButton.enabled = NO;
    }
}

//  Memory empty
//
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    NSLog(@"memory war in archive view");
}

// Show View
//  戻ってきたときにセルの選択を解除
//
- (void) viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:NO];
}

//  Clear button
//
- (IBAction)clearBarButtonPushed:(id)sender
{
    NSArray *emtpyArray = [[NSArray alloc] init];
    [RouteDataController saveToRouteArray:emtpyArray];
    [_routeList removeAllObjects];
    
    [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:0]  withRowAnimation:UITableViewRowAnimationFade];
    
    self.navigationItem.rightBarButtonItem.enabled = NO;
    _saveBarButton.enabled = NO;
    _clearBarButton.enabled = NO;
}

//　Save button
//
- (IBAction)saveBarButtonPushed:(id)sender
{
    [RouteDataController saveToRouteArray:_routeList];
    _saved = true;
    _saveBarButton.enabled = NO;
    [[self tableView] reloadData];
}


#pragma mark - Table view data source

//  count of section = 1
//
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return 1;
}

//  Lengh of table contents.
//
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return [_routeList count];
}

//  TableView Set & Draw
//
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = nil;
    
    if (indexPath.row < [_routeList count]) {
        cell = [tableView dequeueReusableCellWithIdentifier:@"routeListCell" forIndexPath:indexPath];
        if ((indexPath.row == 0) && [_routeList.firstObject isEqualToString:_currentRouteString]) {
            if (_saved) {
                cell.textLabel.textColor = [UIColor darkGrayColor];
            } else {
                cell.textLabel.textColor = [UIColor redColor];
            }
        }
        cell.textLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleFootnote];
        cell.textLabel.numberOfLines = 0;
        [[cell textLabel] setText:[_routeList objectAtIndex:indexPath.row]];

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
    return (0 < [_routeList count]) ? YES : NO ;
}



// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        [tableView beginUpdates];
        NSInteger c = [_routeList count];
        if (0 < c) {
            // Delete the row from the data source
            [_routeList removeObjectAtIndex:[indexPath row]];       // model
            [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
        }
        [tableView endUpdates];
        if (c == 1) {   // last deleted
            [[self tableView] setEditing:NO];
            self.navigationItem.rightBarButtonItem.enabled = NO;
            _clearBarButton.enabled = NO;
            _saveBarButton.enabled = NO;
            [tableView reloadData];
        }
        [RouteDataController saveToRouteArray:_routeList];
        
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
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

//  Height Row
//
//
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSString* text = nil;
    UIFont *nameLabelFont = nil;
    CGFloat PADDING_OUTER = 10;
    CGRect totalRect;

    if (0 < [_routeList count]) {
        /* ROUTE */
        text = [_routeList objectAtIndex:indexPath.row];
        nameLabelFont = [UIFont preferredFontForTextStyle:UIFontTextStyleFootnote];
        totalRect = [text boundingRectWithSize:CGSizeMake(self.tableView.frame.size.width, CGFLOAT_MAX)
                                           options:(NSStringDrawingUsesLineFragmentOrigin|NSStringDrawingUsesFontLeading)
                                        attributes:[NSDictionary dictionaryWithObject:nameLabelFont forKey:NSFontAttributeName]
                                           context:nil];
        return totalRect.size.height + PADDING_OUTER;   // >>>>>>>>>>>>>>>>>>>>>
    } else {
        return [super tableView:tableView heightForRowAtIndexPath:indexPath];
    }
}

//  Header
//
//
- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    UITableViewCell* cell = nil;
    if ([_routeList count] <= 0) {
        cell = [tableView dequeueReusableCellWithIdentifier:@"routeListEmptyCell"];
        return [cell contentView];
    } else {
        return cell;
    }
}

//  Height Header for Empty
//
//
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    if (section == 0) {
        UITableViewCell* cell = nil;
        
        if ([_routeList count] <= 0) {
            cell = [tableView dequeueReusableCellWithIdentifier:@"routeListEmptyCell"];
            return cell.contentView.bounds.size.height;
        }
    }
    return UITableViewAutomaticDimension;
}


#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
    if ([[segue identifier] isEqualToString:@"unwindArchiveRouteSelectSegue"]) {
        // セル選択
        NSString* str = [_routeList objectAtIndex:[[[self tableView] indexPathForSelectedRow] row]];
        _selectRouteString = [str stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    } else {
        // 閉じる(上端左側バーボタン
        //@"unwindArchiveRouteBackSegue"
    }
}

//      Perform Segue (Select tableview row)
//
//
- (BOOL)shouldPerformSegueWithIdentifier:(NSString *)identifier sender:(id)sender
{
    if ([identifier isEqualToString:@"unwindArchiveRouteSelectSegue"]) {
        if (_saved || ([[[self tableView] indexPathForSelectedRow] row] == 0)) {
            return YES;
        } else {
            UIActionSheet *actsheet = [[UIActionSheet alloc] initWithTitle:@"経路は保存されていません."
                                                                  delegate:self
                                                         cancelButtonTitle:@"キャンセル"
                                                    destructiveButtonTitle:nil
                                                         otherButtonTitles:@"選択した経路を表示", @"元の画面に戻る", nil];
            UIWindow* window = [[[UIApplication sharedApplication] delegate] window];
            if ([window.subviews containsObject:self.tableView]) {
                [actsheet showInView:self.tableView];
            } else {
                [actsheet showInView:window];
            }
            return NO;
        }
    } else {
        // unwindArchiveRouteBackSegue
    }
    return YES;
}

//  On select Action sheet.
//
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
//    NSLog(@"Archive Action Select:%d", buttonIndex);
    // 0: update : 更新
    // 1: return : なにもしないで画面も戻る
    // 2: cancel : なにもしない
    if (buttonIndex == 0) {
        [self performSegueWithIdentifier:@"unwindArchiveRouteSelectSegue" sender:self];
    } else if (buttonIndex == 1) {
        [self dismissViewControllerAnimated:YES completion:NULL];
    }
    // cancel
    [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:NO];

}

@end
