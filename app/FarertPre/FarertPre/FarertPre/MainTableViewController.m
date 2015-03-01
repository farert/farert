//
//  MainTableViewController.m
//  経路指定（メイン）ビュー
//
//  Created by TAKEDA, Noriyuki on 2014/07/14.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "MainTableViewController.h"
#import "RouteDataController.h"
#import "FarertTableViewCell.h"
#import "TermSelectTableViewController.h"
#import "SelectLineTableViewController.h"
#import "ResultTableViewController.h"
#import "ArchiveRouteTableViewController.h"
#import "SettingsTableViewController.h"
#import "RouteItem.h"
#include "obj_alpdb.h"
#import "FareInfo.h"
#import "alpdb.h"   // IS_OSAKAKAN_DETOUR_SHORTCUT / FAREOPT_OSAKAKAN_xx


#define ROUTE_DUP_ERROR          1
#define ROUTE_FINISH             2
#define ROUTE_FARERESULT_ERR1    3
#define ROUTE_FARERESULT_ERR2    4
#define ROUTE_FARERESULT_FAIL    5
#define ROUTE_SCRIPT_STATION_ERR 6
#define ROUTE_SCRIPT_LINE_ERR    7
#define ROUTE_SCRIPT_ROUTE_ERR   8
#define ROUTE_DUPCHG_ERROR       9

#define TAG_UIACTIONSHEET_AUTOROUTE         10
#define TAG_UIACTIONSHEET_QUERYSETUPROUTE   11
#define TAG_UIACTIONSHEET_OSAKAKANDETOUR    12

enum { LPROC_REVERSE = 1, LPROC_AUTOROUTE, LPROC_SETUPROUTE };

@interface MainTableViewController()
{
    NSInteger viewContextMode;
    NSInteger routeStat;
    BOOL scroll_flag;
    NSInteger longTermFuncMode;
}
@property (weak, nonatomic) IBOutlet UIBarButtonItem *actionBarItem;

@property (weak, nonatomic) IBOutlet UIBarButtonItem *reverseBarButton; // reverse
@property (weak, nonatomic) IBOutlet UIBarButtonItem *replayBarButton;  // back
@property (strong, nonatomic) RouteDataController *ds;
@property (strong, nonatomic) FareInfo* fareInfo;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *organaizerBarButton;  // archive of route
@property (strong, nonatomic) NSString* routeScript;

@property (nonatomic, strong) UIView *frontView;
@property (nonatomic, strong) UIActivityIndicatorView *indicator;

@end

/* -- delegate for data transfer to parent from child view.
@protocol MyAddViewControllerDelegate <NSObject>
- (void)addViewControllerDidCancel:(MyAddViewController *)controller;
- (void)addViewControllerDidFinish:(MyAddViewController *)controller data:(NSString *)item;
@end
*/

@implementation MainTableViewController

//  load from Storyboard
//
- (void)awakeFromNib
{
    [super awakeFromNib];
}

//  View Initialize
//
//
- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
        scroll_flag = NO;
        longTermFuncMode = 0;
    }
    return self;
}

//  View Load
//
//
- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Uncomment the following line to preserve selection between presentations.
    self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
    
    viewContextMode = 0;
    
    //NSLog(@"MainView didLoad");
    _ds = [[RouteDataController alloc] init];

    //self.replayBarButton.accessibilityHint = @"経路を一つ前に戻ります";
    //self.organaizerBarButton.accessibilityHint = @"保持経路管理ビューを開きます";
    self.navigationItem.rightBarButtonItem.enabled = YES;
    
    self.tableView.rowHeight = 44.0;
    self.actionBarItem.enabled = NO;
}

//  Memory
//
//
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    NSLog(@"memory war in main view");

    // Dispose of any resources that can be recreated.
}

//  View will Appear
//
//
- (void)viewWillAppear:(BOOL)animated    // Called when the view is about to made visible. Default does nothing
{
    [super viewWillAppear:animated];

    if (viewContextMode == CONTEXT_AUTOROUTE_ACTION) {
        AppDelegate* apd = app;
        NSString* title = [RouteDataController StationName:apd.selectTerminalId];
        title = [title stringByAppendingString:@"までの最短経路追加"];
        UIActionSheet *actsheet = [[UIActionSheet alloc] initWithTitle:title
                                                              delegate:self
                                                     cancelButtonTitle:nil
                                                destructiveButtonTitle:nil
                                                     otherButtonTitles:@"新幹線を使う", @"新幹線をつかわない（在来線のみ）", nil];
        actsheet.tag = TAG_UIACTIONSHEET_AUTOROUTE;
        UIWindow* window = [[[UIApplication sharedApplication] delegate] window];
        if ([window.subviews containsObject:self.tableView]) {
            [actsheet showInView:self.tableView];
        } else {
            [actsheet showInView:window];
        }
    } else if (viewContextMode == CONTEXT_ROUTESELECT_VIEW) {
        /* from 経路追加 */
        scroll_flag = YES;
        
    } else if (viewContextMode == CONTEXT_BEGIN_TERMINAL_ACTION) {
        // 開始駅選択による経路破棄の確認
        UIActionSheet* actsheet = [[UIActionSheet alloc] initWithTitle:@"表示経路を破棄してよろしいですか？"
                                                              delegate:self
                                                     cancelButtonTitle:@"いいえ"
                                                destructiveButtonTitle:nil
                                                     otherButtonTitles:@"はい", nil];
        actsheet.tag = TAG_UIACTIONSHEET_QUERYSETUPROUTE;
        UIWindow* window = [[[UIApplication sharedApplication] delegate] window];
        if ([window.subviews containsObject:self.tableView]) {
            [actsheet showInView:self.tableView];
        } else {
            [actsheet showInView:window];
        }
    } else if (viewContextMode == CONTEXT_ROUTESETUP_VIEW) {
        // from 保持経路ビュー
        [self showIndicate];    /* start Activity and Disable UI */
        self.navigationController.view.userInteractionEnabled = NO;
        longTermFuncMode = LPROC_SETUPROUTE;
        [self performSelector:@selector(processDuringIndicatorAnimating:)
                   withObject:_routeScript
                   afterDelay:0.1];
    }
    viewContextMode = 0;
    //  self.navigationController.toolbarHidden = NO;
    
}

//  View did apear
//
//
- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];

    // 戻ってきたときにセルの選択を解除
    [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:NO];

    if (scroll_flag) {
        // （追加）のあとのスクロール
        [self scrollTableView]; // Tableview scroll-up
        scroll_flag = NO;
    }
}


//  長い処理
//  longTermFuncMode != 0
//
- (void)processDuringIndicatorAnimating:(id)param
{
    NSInteger rc = -1;
    AppDelegate *apd = app;

    switch (longTermFuncMode) {
        case LPROC_REVERSE:
            rc = [_ds reverseRoute];
            _fareInfo = [_ds calcFare];
            if (rc < 0) {
                //[self alertMessage:@"経路追加エラー" message:@"経路が重複している等反転できません."];
                routeStat = ROUTE_DUP_ERROR;
            }
            break;
        case LPROC_AUTOROUTE:
            // buttonIndex : 0 = 新幹線を使う
            //               1 = 新幹線を使わない
            rc = [_ds autoRoute:[param intValue] != 1 arrive:apd.selectTerminalId];
            _fareInfo = [_ds calcFare];
            if (rc < 0) {
                //[self alertMessage:@"経路追加エラー" message:@"経路が重複している等追加できません."];
                routeStat = ROUTE_DUP_ERROR;
            } else if (rc == 0) {
                routeStat = ROUTE_FINISH;
            }
            scroll_flag = YES;
            break;
        case LPROC_SETUPROUTE:
            rc = [_ds setupRoute:param];
            _fareInfo = [_ds calcFare];
            if (rc < 0) {
                //[self alertMessage:@"経路追加エラー" message:@"経路が重複している等追加できません."];
                switch (rc) {
                    case -200:
                        routeStat = ROUTE_SCRIPT_STATION_ERR;
                        break;
                    case -300:
                        routeStat = ROUTE_SCRIPT_LINE_ERR;
                        break;
                    case -2:
                        routeStat = ROUTE_SCRIPT_ROUTE_ERR;
                        break;
                    default:
                        routeStat = ROUTE_DUP_ERROR;    // -1: 経路重複エラー
                        break;
                }
            } else { // (0 <= result)
                // 0=end, plus=continue
                if ((rc == 0) && (1 < [_ds getRouteCount])) {
                    routeStat = ROUTE_FINISH;
                } else {
                    routeStat = 0;  // success
                }
                viewContextMode = CONTEXT_ROUTESELECT_VIEW;
                scroll_flag = YES;
            }
            break;
        default:
            NSAssert(FALSE, @"bug:%s:%d", __FILE__, __LINE__);
            break;
    }
    [self fareResultSetting:rc];   /* 簡易結果(Footer section) */
    longTermFuncMode = 0;
    [self hideIndicate];    /* hide Activity and enable UI */
}

#pragma mark - Table view data source
//
//
//
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return 1;
}

//
//
//
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    NSInteger num = [_ds getRouteCount];

    //NSLog(@"Route count=%d", num);

    if ([_ds startStationId] <= 0) {
        num = 0;
    }
    if (0 < num) {
        //self.navigationItem.rightBarButtonItem.enabled = YES;
        self.replayBarButton.enabled = YES;
    } else {
        //self.navigationItem.rightBarButtonItem.enabled = NO;
        self.replayBarButton.enabled = NO;
    }
    if (1 < num) {
        self.reverseBarButton.enabled = YES;        
    } else {
        self.reverseBarButton.enabled = NO;
    }

    return num;
}

//
//
//
- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    TableHeaderViewCell* tableHeaderViewCell = [tableView dequeueReusableCellWithIdentifier:@"HeaderCell"];
    UILabel* label;
    NSString* title = nil;
    NSString* subtitle = nil;

    if (0 < [_ds startStationId]) {
        /* 開始駅 */
        title = [RouteDataController StationNameEx:[_ds startStationId]];
        /* ひらかな */
        subtitle = [obj_alpdb GetKanaFromStationId:[_ds startStationId]];
    } else {
        title = @"";
        subtitle = @"";
    }
    /* 開始駅 */
    label = (UILabel*)[tableHeaderViewCell viewWithTag:10001];
    label.text = title;
    /* ひらかな */
    label = (UILabel*)[tableHeaderViewCell viewWithTag:10002];
    label.text = subtitle;

    tableHeaderViewCell.delegate = self;

    return tableHeaderViewCell;
}

//
//
//
- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section
{
    //UITableViewCell* cell = [tableView dequeueReusableHeaderFooterViewWithIdentifier:@"footerCell"];
    TableHeaderViewCell* cell = [tableView dequeueReusableCellWithIdentifier:@"footerCell"];;
    
    if ((1 < [_ds getRouteCount]) && (_fareInfo) && (_fareInfo.result == 0)) {
        UIView* v = [cell viewWithTag:201];
        v.hidden = YES;

        UILabel* lbl = (UILabel*)[cell viewWithTag:40001];
        lbl.text = [NSString stringWithFormat:@"¥%@", [RouteDataController fareNumStr:(_fareInfo.fareForCompanyline + _fareInfo.fareForJR)]];
        lbl = (UILabel*)[cell viewWithTag:40002];
        lbl.text = [NSString stringWithFormat:@"%@km", [RouteDataController kmNumStr:_fareInfo.totalSalesKm]];
        lbl = (UILabel*)[cell viewWithTag:40003];
        lbl.text = [NSString stringWithFormat:@"%u日間", (int)_fareInfo.ticketAvailDays];
        
        cell.delegate = self;   // enable touch
        
        return cell;

    } else {
        return nil;
    }
}

//
//
//
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"HeaderCell"];
    if (section == 0) {
        return cell.contentView.bounds.size.height;
    } else {
        return UITableViewAutomaticDimension;
    }
}

//  Return Footer height.
//
//
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"footerCell"];
    if (section == 0) {
        if (1 < [_ds getRouteCount]) {
            return cell.contentView.bounds.size.height;
        } else {
            return 0.1;
        }
    } else {
        return UITableViewAutomaticDimension;
    }
}

//
//
//
- (CGFloat)tableView:(UITableView *)tableView estimatedHeightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewAutomaticDimension;    
}


#if 0
- (CGFloat)tableView:(UITableView *)tableView estimatedHeightForHeaderInSection:(NSInteger)section
{
    return [self tableView:tableView heightForHeaderInSection:section];
}

- (CGFloat)tableView:(UITableView *)tableView estimatedHeightForFooterInSection:(NSInteger)section
{
    return [self tableView:tableView heightForFooterInSection:section];
}
#endif

//  Touched on tableView Header / Footer
//
//
- (void)tableHeaderViewTouched:(TableHeaderViewCell *)tableHeaderViewCell
{
    //TermSelectTableViewController  *view = (TermSelectTableViewController*)[self.storyboard instantiateViewControllerWithIdentifier:@"TermSelectViewSegue"];
    //[self presentViewController:view animated:YES completion:^{[self termSelectViewEnd ];}];
#if 0
    TermSelectTableViewController* termSelViewController = [self.storyboard instantiateViewControllerWithIdentifier:@"TermSelectTableViewController"];
    [self presentViewController:termSelViewController animated:YES completion:nil];
#else
    if (tableHeaderViewCell.tag == 100) {
        /* header */
        [self performSegueWithIdentifier:@"terminalSelectSegue" sender:self];
    
    } else if (tableHeaderViewCell.tag == 200) {
        /* Footer */
        NSInteger lastIndex = [_ds getRouteCount] - 2; // last item
        if (0 <= lastIndex) {
            scroll_flag = YES; // Tableview scroll-up
            NSIndexPath* indexPath = [NSIndexPath indexPathForRow:lastIndex inSection:0];
            [[self tableView] selectRowAtIndexPath:indexPath animated:NO scrollPosition:UITableViewScrollPositionNone];
            [self performSegueWithIdentifier:@"fareInfoDetailSegue" sender:self];
        }
    }
#endif
}

//  TableView データ描画・設定
//
//
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    FarertTableViewCell *cell = nil;
    
    if (([_ds getRouteCount] - 1) <= [indexPath row]) {
        /* 
         * 最後の行
         */
        cell = [tableView dequeueReusableCellWithIdentifier:@"LastRouteCell"];
        UILabel* lbl = (UILabel*)[cell viewWithTag:201];
        if (routeStat != 0) {
            switch (routeStat) {
                case ROUTE_DUP_ERROR:
                    lbl.text = @"経路が重複している等追加できません";
                    break;
                case ROUTE_FINISH:
                    lbl.text = @"経路が終端に達しました";
                    break;
                case ROUTE_FARERESULT_ERR1:
                    lbl.text = @"不完全ルート：この経路の片道乗車券は購入できません.";
                    break;
                case ROUTE_FARERESULT_ERR2:
                    lbl.text = @"会社線のみの運賃は表示できません.";
                    break;
                case ROUTE_SCRIPT_STATION_ERR:
                    lbl.text = @"不正な駅名が含まれています.";
                    break;
                case ROUTE_SCRIPT_LINE_ERR:
                    lbl.text = @"不正な路線名が含まれています.";
                    break;
                case ROUTE_SCRIPT_ROUTE_ERR:
                    lbl.text = @"経路不正";
                    break;
                case ROUTE_DUPCHG_ERROR:
                    lbl.text = @"経路が重複していますので変更できません";
                    break;
                default:
                    lbl.text = @"unknown error";
                    break;
            }
            lbl.hidden = NO;
        } else {
            lbl.hidden = YES;
        }
    } else {
        /* 最後の行以外の普通の行
         *
         */
        cell = [tableView dequeueReusableCellWithIdentifier:@"RouteContentCell"];

        RouteItem* ri = [_ds getRouteItem:indexPath.row + 1];
        //cell.fare.text = ri.fare;
        cell.lineName.text = ri.line;
        cell.stationName.text = ri.station;
        //cell.salesKm.text = ri.salesKm;
    }
    return cell;
}

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return NO;
}
*/
/* -- delegate for data transfer to parent from child view.
 
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
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the item to be re-orderable.
    return NO;
}
*/


#pragma mark - local

- (void)fareResultSetting:(NSInteger)rc
{
    NSInteger opt;
    
    if (0 < rc) {
        switch (_fareInfo.result) {
            case 0:
                routeStat = 0;
                break;
            case 1:
                routeStat = ROUTE_FARERESULT_ERR1;
                break;
            case 2:
                routeStat = ROUTE_FARERESULT_ERR2;
                break;
            default:
                routeStat = ROUTE_FARERESULT_FAIL;
                break;
        }
    }
    
    opt = [_ds getFareOption];

    if (IS_OSAKAKAN_DETOUR_EN(opt)) {
        _actionBarItem.enabled = YES;
    } else {
        _actionBarItem.enabled = NO;
    }
}

//  発駅設定・表示（ヘッダタップ、最短経路
//
//
- (void)setBeginTerminal
{
    AppDelegate *apd = app;
    
    NSInteger termId = apd.selectTerminalId;
    [_ds addRoute:termId];
    routeStat = 0;
    [[self tableView] reloadData];
    [RouteDataController SaveToTerminalHistory:[RouteDataController StationNameEx:termId]];
}

//  TableView scroll-up(追加後、削除後、最短経路、保持経路)
//
//
- (void)scrollTableView
{
    NSInteger lastIndex = [_ds getRouteCount] - 1;
    if (0 < lastIndex) {
        NSIndexPath* idxpath = [NSIndexPath indexPathForRow:lastIndex inSection:0];
        [[self tableView] scrollToRowAtIndexPath:idxpath atScrollPosition:UITableViewScrollPositionBottom animated:YES];
    }
}

#pragma mark - Action

// 削除(経路戻り)ボタン
- (IBAction)replayButtonPushed:(id)sender
{
    if (1 < [_ds getRouteCount]) {
        [_ds removeTail];
        _fareInfo = [_ds calcFare];
        [self fareResultSetting:1];
    } else {
        [_ds resetStartStation];    // removeAll, clear start
    }
    [[self tableView] reloadData];
    [self scrollTableView];
}


// Toolbar : リバースボタン
- (IBAction)reverseButtonTapped:(id)sender
{
    if ([_ds getRouteCount] < 2) {
        return;
    }
    [self showIndicate];    /* start Activity and Disable UI */
    self.navigationController.view.userInteractionEnabled = NO;
    longTermFuncMode = LPROC_REVERSE;
    [self performSelector:@selector(processDuringIndicatorAnimating:) withObject:nil afterDelay:0.1];
}

//  Action menu
//
//
- (IBAction)actionBarItemTapped:(id)sender
{
    NSInteger flg = [_ds getFareOption];
    
    if (IS_OSAKAKAN_DETOUR_EN(flg)) {
        UIActionSheet *actsheet = [[UIActionSheet alloc] init];
        UIWindow* window = [[[UIApplication sharedApplication] delegate] window];
        
        actsheet.tag = TAG_UIACTIONSHEET_OSAKAKANDETOUR;
        
        actsheet.delegate = self;
        actsheet.title = @"大阪環状線経由指定";
        
        if (IS_OSAKAKAN_DETOUR_SHORTCUT(flg)) {
            [actsheet addButtonWithTitle:@"大阪環状線 近回り"];
        } else {
            [actsheet addButtonWithTitle:@"大阪環状線 遠回り"];
        }
        [actsheet addButtonWithTitle:@"キャンセル"];
        actsheet.cancelButtonIndex = 1;
        if ([window.subviews containsObject:self.tableView]) {
            [actsheet showInView:self.tableView];
        } else {
            [actsheet showInView:window];
        }
    }
}

//  Action Sheet
//   - Auto route
//   - Query clear remove by change begin terminal.
//   - Change root for OOSAKA Kanjyou-sen
//
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    AppDelegate *apd = app;
    NSInteger rc;
    BOOL dir;

    if (actionSheet.tag == TAG_UIACTIONSHEET_AUTOROUTE) {
        if (apd.context != CONTEXT_AUTOROUTE_VIEW) {
            return;
        }
        /*  最短経路 doneTerminal: -> willApear: ->
         *
         */
        [self showIndicate];    /* start Activity and Disable UI */
        self.navigationController.view.userInteractionEnabled = NO;
        longTermFuncMode = LPROC_AUTOROUTE;
        [self performSelector:@selector(processDuringIndicatorAnimating:)
                   withObject:[NSNumber numberWithInteger:buttonIndex]
                   afterDelay:0.1];
        
    } else if (actionSheet.tag == TAG_UIACTIONSHEET_QUERYSETUPROUTE) {
        /*  経路表示時の１行目の発駅変更(既存経路破棄→新規発駅設定)
         * doneTerminal: -> willApear
         */
        if (buttonIndex == 0) {
            [self setBeginTerminal];
        }
        
        [[self tableView] reloadData];
        
    } else if (actionSheet.tag == TAG_UIACTIONSHEET_OSAKAKANDETOUR) {
        /*  大阪環状線 遠回り／近回り
         */
        if (buttonIndex == 0) {
            if ([[actionSheet buttonTitleAtIndex:buttonIndex] rangeOfString:@"遠回り"].location != NSNotFound) {
                dir = YES;
            } else {
                dir = NO;
            }
            rc = [_ds setDetour:dir];
            _fareInfo = [_ds calcFare];
            if (rc < 0) {
                routeStat = ROUTE_DUPCHG_ERROR;
            } else if (rc == 0) {
                routeStat = ROUTE_FINISH;
            } else {
                routeStat = 0;  /* success */
            }
            viewContextMode = CONTEXT_ROUTESELECT_VIEW;
            [[self tableView] reloadData];
        }
    }
}


#pragma mark - Navigation

//  Execute Segue
//
// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
    
    NSString* segid = [segue identifier];
    
	if ([segid isEqualToString:@"terminalSelectSegue"]) {
        ///UIViewController* vc = [segue destinationViewController];
        AppDelegate *apd = app;
        apd.context = CONTEXT_TERMINAL_VIEW;
//        vc.delegate = self;
	} else if ([segid isEqualToString:@"routeLineSegue"]) {
        /* 経路追加(駅に属する路線一覧 */
        SelectLineTableViewController* selectLineTblViewController = [segue destinationViewController];
        selectLineTblViewController.baseStationId = [_ds lastStationId];
        selectLineTblViewController.companyOrPrefectId = 0;
        selectLineTblViewController.lastLineId = [_ds lastLineId];

        AppDelegate *apd = app;
        apd.context = CONTEXT_ROUTESELECT_VIEW;
        
    } else if ([segid isEqualToString:@"fareInfoDetailSegue"]) {
        /* 運賃詳細ビュー */
        NSInteger idx = [[[self tableView] indexPathForSelectedRow] row];
        ResultTableViewController* resultViewCtlr = [segue destinationViewController];
        resultViewCtlr.ds = [[RouteDataController alloc] initWithAssign:_ds count:idx + 2];
        AppDelegate *apd = app;
        apd.context = 0;

    } else if ([segid isEqualToString:@"routeManagerSegue"]) {
        /* 経路一覧ビュー */
        UINavigationController* naviCtlr = [segue destinationViewController];
        ArchiveRouteTableViewController* routeMngViewCtlr = (ArchiveRouteTableViewController*)[naviCtlr topViewController];
        if ([_ds getRouteCount] <= 1) {
            routeMngViewCtlr.currentRouteString = @"";
        } else {
            routeMngViewCtlr.currentRouteString = [_ds routeScript];
        }
        AppDelegate *apd = app;
        apd.context = 0;
    } else if ([segid isEqualToString:@"versionInfoTransitSegue"]) {
        //
    } else if ([segid isEqualToString:@"settingsSegue"]) {
        /// // save Route
        self.routeScript = [[_ds routeScript]
                            stringByTrimmingCharactersInSet:
                            [NSCharacterSet whitespaceAndNewlineCharacterSet]];
    } else {
        NSAssert(FALSE, @"bug:%s:%d, %@", __FILE__, __LINE__, segid);
    }
    //NSLog(@"prepareForSegue:%@", [segue identifier]);
}

/*
- (void)addViewControllerDidCancel:(MyAddViewController *)controller
{
    [self dismissViewControllerAnimated:YES completion:NULL];
}

- (void)addViewControllerDidFinish:(MyAddViewController *)controller data:(NSString *)item
{
    if ([item length]) {
        [self.dataController addData:item];
        [[self tableView] reloadData];
    }
    [self dismissViewControllerAnimated:YES completion:NULL];
}
*/


//  Retuened child view (Enable select)
//
- (IBAction)doneTerminal:(UIStoryboardSegue *)segue
{
    NSString* seg_id = [segue identifier];
    
    //NSLog(@"doneTerminal:%@", seg_id);

    viewContextMode = 0;
    
    if ([seg_id isEqualToString:@"termSelectDone"] ||
        [seg_id isEqualToString:@"terminalSelectDoneSegue"] ||
        [seg_id isEqualToString:@"doneTerminalHistorySegue"]) {
		/* from TermSelectTableViewController - Searchbar */
        /* from SelectStationTableViewController */
        /* from History View */
        NSInteger termId = 0;

        ///UIViewController* v = (UIViewController*)[segue sourceViewController];
        AppDelegate *apd = app;
        //termId = [[app selectTerminalId] intValue];
        termId = apd.selectTerminalId;
        if (apd.context == CONTEXT_TERMINAL_VIEW) {
            // from 発駅
            //NSLog(@"begin station=%d, %@", termId, [RouteDataController StationName:termId]);
            //if ([_ds startStationId] == termId) {
            //    return;
            //}
            if ((1 < [_ds getRouteCount]) && ![RouteDataController IsRouteInStrage:[_ds routeScript]]) {
                viewContextMode = CONTEXT_BEGIN_TERMINAL_ACTION;
                // つつきは、viewDidApear:　で
            } else {
                [self setBeginTerminal];
            }
        } else if (apd.context == CONTEXT_AUTOROUTE_VIEW) {
            // from 最短経路
            // auto route
            //NSLog(@"autoroute end station=%d, %@", termId, [RouteDataController StationName:termId]);
            
            // AlartViewで、新幹線を使うか否かを訊いてautoroute
            //BOOL bulletUse = [self queryDialog:@"新幹線を使用しますか?"];
            viewContextMode = CONTEXT_AUTOROUTE_ACTION;
            [RouteDataController SaveToTerminalHistory:[RouteDataController StationNameEx:termId]];
            // つつきは、viewWilApear:　で
            
        } else if (apd.context == CONTEXT_ROUTESELECT_VIEW) {
            // from 経路追加
            /* add route */
            NSInteger result = [_ds addRoute:apd.selectLineId stationId:apd.selectTerminalId];

            _fareInfo = [_ds calcFare];
            [self fareResultSetting:result];

            if (result < 0) {
                //[self alertMessage:@"経路追加エラー" message:@"経路が重複している等追加できません."];
                routeStat = ROUTE_DUP_ERROR;
            } else { // (0 <= result)
                // 0=end, plus=continue
                if (result == 0) {
                    routeStat = ROUTE_FINISH;
                }
            }
            viewContextMode = CONTEXT_ROUTESELECT_VIEW;
            [[self tableView] reloadData];
        } else {
            NSAssert(nil, @"bug");
            
        }
    } else if ([seg_id isEqualToString:@"unwindArchiveRouteSelectSegue"]) {
        // from 保持経路一覧
        // setup route
        ArchiveRouteTableViewController* routeViewCtrl = [segue sourceViewController];
        if (routeViewCtrl.selectRouteString) {
            viewContextMode = CONTEXT_ROUTESETUP_VIEW;
            self.routeScript = routeViewCtrl.selectRouteString;
        }
    } else {
        //NSAssert(nil, @"bug");
        NSLog(@"Bug?? %@", [segue identifier]);
    }
}

// exit(unwind) segue
//  from: ArchiveRouteTableViewController - [Close] button
//        TerminalHistoryTableViewController - [Close] button
//         TermSelectTableViewController - [Close] button
//
- (IBAction)cancelTerminal:(UIStoryboardSegue *)segue
{
    NSLog(@"cancelTerminal:%@", [segue identifier]);
}

// exit(unwind) segue
//  from: VersionInfoViewController
//
- (IBAction)closeModal:(UIStoryboardSegue *)segue
{
    NSLog(@"MainView: closeModal:");
    [self dismissViewControllerAnimated:YES completion:NULL];
    
    if ([[segue identifier] isEqualToString:@"settingsSegue"]) {
        SettingsTableViewController* view = [segue sourceViewController];
        if (0 < view.selectDbId) {  // is change DB
            viewContextMode = CONTEXT_ROUTESETUP_VIEW;
        }
    }
}


- (void)alertMessage:(NSString*)title message:(NSString*)message
{
    // Alart
    UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:title message:message delegate:self cancelButtonTitle:nil otherButtonTitles:@"OK", nil];
    alertView.delegate = self;
    alertView.alertViewStyle = UIAlertViewStyleDefault;
    [alertView show];
}

#if 0
- (BOOL)queryDialog:(NSString*)title
{
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil
                                                    message:title
                                                   delegate:self
                                          cancelButtonTitle:@"No"
                                          otherButtonTitles:@"Yes", nil];
    alert.delegate = self;
    [alert show];
    
    // ボタンが押されるまで待つ
    alertButtonIndex = -1;
    while (alertButtonIndex == -1) {
        [[NSRunLoop currentRunLoop]
         runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.5f]]; // 0.5秒
    }
    
    return alertButtonIndex == 1 ? TRUE : FALSE;
}
#endif


- (void)showIndicate
{
    self.frontView = [[UIView alloc] initWithFrame:self.navigationController.view.bounds];
    self.frontView.backgroundColor = [UIColor clearColor];
    [self.navigationController.view addSubview:[self frontView]];
    
    self.indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
    self.indicator.color = [UIColor blackColor];
    self.indicator.center = self.frontView.center;
    [self.frontView addSubview:self.indicator];
    [self.frontView bringSubviewToFront:self.indicator];
    [self.indicator startAnimating];
   
}

- (void)hideIndicate
{
    [self.indicator stopAnimating];
    [self.indicator removeFromSuperview];
    [self.frontView removeFromSuperview];
    self.indicator = nil;
    self.frontView = nil;

    self.navigationController.view.userInteractionEnabled = YES;
    [[self tableView] reloadData];
}



#if 0
/*
当初 TerminalSelectTableViewの行選択で、どこからでもここへ来れるように
Delegateコールを考えてたが、AppDelegate + Exit segue 方式に戻した。
こちらのほうが柔軟で使い勝っても良いのだが,Delegate用に前準備(自ポインタを渡したり）
が必要だったり、Exit segueの手軽さ、AppDelegateのシンプルさのほうが
コード量がすくないので採用した。
*/
- (void)terminalSelectTablebleViewDone:(NSInteger)terminalId
{
    NSLog(@"Enter: begin station:%d", terminalId);
}
#endif
@end
