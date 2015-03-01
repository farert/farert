//
//  ResultTableViewController.m
//  運賃表示ビュー
//
//  Created by TAKEDA, Noriyuki on 2014/07/21.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "ResultTableViewController.h"
#import "RouteDataController.h"
#import "FareInfo.h"
#import "alpdb.h"	


@interface ResultTableViewController ()
{
    NSInteger num_of_km;
    NSInteger num_of_fare;
    NSMutableArray* contentsForKm;
    NSMutableArray* contentsForFare;
}
// メールビュー生成
@property (strong, nonatomic) MFMailComposeViewController* mailViewCtl;

@property (strong, nonatomic) FareInfo* fareInfo;

@property (weak, nonatomic) IBOutlet UIBarButtonItem *composeBarButton;

#if 0
@property (weak, nonatomic) IBOutlet UIBarButtonItem *specificAppliedBarButton;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *areaTerminalBarButton;
#endif

@property (nonatomic, strong) UIView *frontView;
@property (nonatomic, strong) UIActivityIndicatorView *indicator;

@property (weak, nonatomic) UIDocumentInteractionController* docInterCon;

@end

@implementation ResultTableViewController

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

    //[self.tableView registerClass:[UITableViewCell class] forCellReuseIdentifier:@"Cell"];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(preferredContentSizeChanged:)
                                                 name:UIContentSizeCategoryDidChangeNotification
                                               object:nil];

    //NSLog(@"ResultView didLoad entry");
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
    
    self.navigationController.toolbarHidden = NO;

    // Inner state variables
    ///self.specificAppliedBarButton.enabled = YES;
    
    [self reCalcFareInfo:TRUE];
  
    self.tableView.sectionHeaderHeight = 40.0;
    self.tableView.sectionFooterHeight = 0.01;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    NSLog(@"memory war in result view");
}

// 戻ってきたときにセルの選択を解除
- (void) viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:NO];
}

- (void)preferredContentSizeChanged:(NSNotification *)aNotification
{
    // refresh tableView
    [self.tableView reloadData];
}

#pragma mark - Action

//  Compose button
//
- (IBAction)composeButtonAction:(id)sender
{
    UIActionSheet *actsheet = [[UIActionSheet alloc] init];
    UIWindow* window = [[[UIApplication sharedApplication] delegate] window];
    
    actsheet.delegate = self;
    actsheet.title = self.title;
#if 0
    [actsheet addButtonWithTitle:@"他アプリへ送信(AirDrop)"];
    [actsheet addButtonWithTitle:@"他アプリで開く"];
    
    if ([MFMailComposeViewController canSendMail]) {
        [actsheet addButtonWithTitle:@"メール送信"];
    } else {
        [actsheet addButtonWithTitle:@"メール送信できません"];
    }
#endif
    if (IS_RULE_APPLIED_EN(_fareInfo.calcResultFlag)) {
        if (IS_RULE_APPLIED(_fareInfo.calcResultFlag)) {
            [actsheet addButtonWithTitle:@"特例を適用しない"];
            
            if (IS_MAIHAN_CITY_START_TERMINAL_EN(_fareInfo.calcResultFlag)) {
                if (IS_MAIHAN_CITY_START(_fareInfo.calcResultFlag)) {
                    // 発駅=都区市内
                    [actsheet addButtonWithTitle:@"発駅を単駅指定"];
                } else  {
                    [actsheet addButtonWithTitle:@"着駅を単駅指定"];
                }
            }
        } else {
            [actsheet addButtonWithTitle:@"特例を適用する"];
        }
    }
    if (_fareInfo.isArbanArea) {
        [actsheet addButtonWithTitle:@"最短経路算出"];
    }
    
    if (IS_OSAKAKAN_DETOUR_EN(_fareInfo.calcResultFlag)) {
        if (IS_OSAKAKAN_DETOUR_SHORTCUT(_fareInfo.calcResultFlag)) {
            [actsheet addButtonWithTitle:@"大阪環状線 近回り"];
        } else {
            [actsheet addButtonWithTitle:@"大阪環状線 遠回り"];
        }
    }
    [actsheet addButtonWithTitle:@"キャンセル"];
    actsheet.cancelButtonIndex = [actsheet numberOfButtons] - 1;
    if (![MFMailComposeViewController canSendMail]) {
        actsheet.destructiveButtonIndex = 0;    /* Can't send mail (Red color title) */
    }
    if ([window.subviews containsObject:self.tableView]) {
        [actsheet showInView:self.tableView];
    } else {
        [actsheet showInView:window];
    }
}

// action sheet
// Send to other application.
//
- (IBAction)actionBarButtonAction:(id)sender
{
    [self ShowAirDrop];
}

//  Action Sheet (Send mail)
//
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    //NSLog(@"action select:%d", buttonIndex);
    NSString* title;
    
    if ((actionSheet.numberOfButtons - 1) == buttonIndex) {
        return; // Canceled
    }

    title = [actionSheet buttonTitleAtIndex:buttonIndex];

/*    if ([title rangeOfString:@"AirDrop"].location != NSNotFound) {
        [self ShowAirDrop];
        
    } else if ([title rangeOfString:@"開く"].location != NSNotFound) {
        // send mail
        [self ShowOtherAppOpen];

    } else if ([title rangeOfString:@"メール"].location != NSNotFound) {
        // send mail
        [self sendResultMail];
    } else */ if ([title rangeOfString:@"特例"].location != NSNotFound) {
        if ([title rangeOfString:@"しない"].location != NSNotFound) {
            // @"特例を適用しない";
            [_ds setFareOption:FAREOPT_RULE_NO_APPLIED availMask:FAREOPT_AVAIL_RULE_APPLIED];
        } else {
            // @"特例を適用する";
            [_ds setFareOption:FAREOPT_RULE_APPLIED availMask:FAREOPT_AVAIL_RULE_APPLIED];
        }
        [self reCalcFareInfo];

    } else if ([title rangeOfString:@"単駅"].location != NSNotFound) {
        if ([title rangeOfString:@"発駅"].location != NSNotFound) {
            // "発駅を単駅指定";
            [_ds setFareOption:FAREOPT_APPLIED_START availMask:FAREOPT_AVAIL_APPLIED_START_TERMINAL];
        } else {
            // "発駅を単駅指定";
            [_ds setFareOption:FAREOPT_APPLIED_TERMINAL availMask:FAREOPT_AVAIL_APPLIED_START_TERMINAL];
        }
        [self reCalcFareInfo];
        [self.tableView reloadData];

    } else if ([title rangeOfString:@"最短経路"].location != NSNotFound) {
        [self showIndicate];
        self.navigationController.view.userInteractionEnabled = NO;
        [self performSelector:@selector(processDuringIndicatorAnimating:)
                   withObject:Nil
                   afterDelay:0.1];
        
    } else if ([title rangeOfString:@"回り"].location != NSNotFound) {
        NSInteger dir;
        NSInteger rc;
        
        if ([title rangeOfString:@"遠"].location != NSNotFound) {
            dir = YES;
        } else {
            dir = NO;
        }
        rc = [_ds setDetour:dir];
        if (0 <= rc) {
            [self reCalcFareInfo];
            [self.tableView reloadData];
        } else {
            [self ShowAlertView:@"エラー" message:@"経路が重複するため指定できません"];
        }
    } else {
        // cancel
    }
}

//  長い処理
//
- (void)processDuringIndicatorAnimating:(id)param
{
    NSInteger rc;
    NSInteger begin_id;
    NSInteger end_id;
    
    begin_id = [_ds startStationId];
    end_id = [_ds lastStationId];
    [_ds resetStartStation];
    [_ds addRoute:begin_id];
    rc = [_ds autoRoute:FALSE arrive:end_id];
    if (0 <= rc) {
        [self reCalcFareInfo];
        [self.tableView reloadData];
    }
    [self hideIndicate];    /* hide Activity and enable UI */
}


#pragma mark - local

// FareInfo 再計算
- (void)reCalcFareInfo
{
    [self reCalcFareInfo:FALSE];
}

- (void)reCalcFareInfo:(BOOL)initial
{
    if (!initial && (!_fareInfo || _fareInfo.result != 0)) {
        return;   // Error message
    }
    _fareInfo = [_ds calcFare];
		
    self.navigationItem.title = [RouteDataController TerminalName:_fareInfo.endStationId];
    self.navigationItem.prompt = [[RouteDataController TerminalName:_fareInfo.beginStationId] stringByAppendingString:@" → "];

    // refresh tableView
    [self setupDispContent];
    if (!initial) {
        [self.tableView reloadData];
    }
    if (IS_FAREOPT_ENABLED(_fareInfo.calcResultFlag) || _fareInfo.isArbanArea) {
        self.composeBarButton.enabled = YES;
    } else {
        self.composeBarButton.enabled = NO;
    }
}



/* viewDidLoad */

- (void)setupDispContent
{
    int i;
    NSString* str = nil;
    
    
    /* KM */
    
    num_of_km = 1;
    if (_fareInfo.jrCalcKm == _fareInfo.jrSalesKm) {
        contentsForKm = [@[@{@"salesKm" : [NSString stringWithFormat:@"%@km",
                                           [RouteDataController kmNumStr:_fareInfo.totalSalesKm]]}] mutableCopy];
    } else {
        if (_fareInfo.companySalesKm != 0) {
            str = @"計算キロ(JR)";
        } else {
            str = @"計算キロ";
        }
        contentsForKm = [@[@{@"salesKm" : [NSString stringWithFormat:@"%@km",
                                           [RouteDataController kmNumStr:_fareInfo.totalSalesKm]],
                             @"calcKm": [NSString stringWithFormat:@"%@km",
                                         [RouteDataController kmNumStr:_fareInfo.jrCalcKm]],
                             @"subTitle" : str}] mutableCopy];
    }
    
    if (_fareInfo.salesKmForHokkaido != 0) {
        num_of_km++;
        if (_fareInfo.calcKmForHokkaido == _fareInfo.salesKmForHokkaido) {
            [contentsForKm addObject:@{@"title" : @"JR北海道",
                                       @"salesKm" : [NSString stringWithFormat:@"%@km",
                                                     [RouteDataController kmNumStr:_fareInfo.salesKmForHokkaido]]}];
        } else {
            [contentsForKm addObject:@{@"title" : @"JR北海道",
                                       @"salesKm" : [NSString stringWithFormat:@"%@km",
                                                     [RouteDataController kmNumStr:_fareInfo.salesKmForHokkaido]],
                                       @"calcKm" : [NSString stringWithFormat:@"%@km",
                                                    [RouteDataController kmNumStr:_fareInfo.calcKmForHokkaido]]}];
        }
    }
    if (_fareInfo.salesKmForKyusyu != 0) {
        num_of_km++;
        if (_fareInfo.calcKmForKyusyu == _fareInfo.salesKmForKyusyu) {
            [contentsForKm addObject:@{@"title" : @"JR九州",
                                       @"salesKm" : [NSString stringWithFormat:@"%@km",
                                                     [RouteDataController kmNumStr:_fareInfo.salesKmForKyusyu]]}];
        } else {
            [contentsForKm addObject:@{@"title" : @"JR九州",
                                       @"salesKm" : [NSString stringWithFormat:@"%@km",
                                                     [RouteDataController kmNumStr:_fareInfo.salesKmForKyusyu]],
                                       @"calcKm": [NSString stringWithFormat:@"%@km",
                                                   [RouteDataController kmNumStr:_fareInfo.calcKmForKyusyu]]}];
        }
    }
    if (_fareInfo.salesKmForShikoku != 0) {
        num_of_km++;
        if (_fareInfo.calcKmForShikoku == _fareInfo.salesKmForShikoku) {
            [contentsForKm addObject:@{@"title" : @"JR四国",
                                       @"salesKm" : [NSString stringWithFormat:@"%@km",
                                                     [RouteDataController kmNumStr:_fareInfo.salesKmForShikoku]]}];
        } else {
            [contentsForKm addObject:@{@"title" : @"JR四国",
                                       @"salesKm" : [NSString stringWithFormat:@"%@km",
                                                     [RouteDataController kmNumStr:_fareInfo.salesKmForShikoku]],
                                       @"calcKm": [NSString stringWithFormat:@"%@km",
                                                   [RouteDataController kmNumStr:_fareInfo.calcKmForShikoku]]}];
        }
    }
    if (_fareInfo.rule114_salesKm != 0) {
        num_of_km++;
        [contentsForKm addObject:@{@"title" : @"規程114条適用",
                                   @"salesKm" : [NSString stringWithFormat:@"%@km",
                                                 [RouteDataController kmNumStr:_fareInfo.rule114_salesKm]],
                                   @"calcKm": [NSString stringWithFormat:@"%@km",
                                               [RouteDataController kmNumStr:_fareInfo.rule114_calcKm]]}];
    }
    if (_fareInfo.companySalesKm != 0) {
        num_of_km++;
        [contentsForKm addObject:@{@"Company_salesKm" : [NSString stringWithFormat:@"%@km",
                                                         [RouteDataController kmNumStr:_fareInfo.companySalesKm]],
                                   @"JR_salesKm" : [NSString stringWithFormat:@"%@km",
                                                    [RouteDataController kmNumStr:_fareInfo.jrSalesKm]]}];
    }
    
    
    /* FARE */
    
    num_of_fare = 2;    /* normal + round */
    
    /* 1行目 普通＋会社 or 普通 + IC */
    /* 2行目 (往復）同上 */
    
    if (_fareInfo.isRoundtripDiscount) {
        str = @" (割引)";
    } else {
        str = @"";
    }
    
    if (_fareInfo.fareForCompanyline != 0) {
        /* 1: 普通運賃＋会社線 */
        contentsForFare = [NSMutableArray arrayWithObject:@{@"fare" : [NSString stringWithFormat:@"¥%@",
                                                                       [RouteDataController fareNumStr:_fareInfo.fareForJR + _fareInfo.fareForCompanyline]],
                                                            @"subTitle" : @"うち会社線",
                                                            @"subFare" : [NSString stringWithFormat:@"¥%@",
                                                                          [RouteDataController fareNumStr:_fareInfo.fareForCompanyline]]}];
        /* 2: 往復運賃(割引可否) ＋ 会社線往復 */
        [contentsForFare addObject:@{@"fare" : [[NSString stringWithFormat:@"¥%@",
                                                 [RouteDataController fareNumStr:_fareInfo.roundTripFareWithComapnyLine]]
                                                stringByAppendingString:str],
                                     @"subFare" : [NSString stringWithFormat:@"¥%@",
                                                   [RouteDataController fareNumStr:_fareInfo.fareForCompanyline * 2]]}];
    } else if (_fareInfo.fareForIC != 0) {
        /* 1: 普通運賃 ＋ IC運賃 */
        contentsForFare = [NSMutableArray arrayWithObject:@{@"fare" : [NSString stringWithFormat:@"¥%@",
                                                                       [RouteDataController fareNumStr:_fareInfo.fareForJR + _fareInfo.fareForCompanyline]],
                                                            @"subTitle" : @"IC運賃",
                                                            @"subFare" : [NSString stringWithFormat:@"¥%@",
                                                                          [RouteDataController fareNumStr:_fareInfo.fareForIC]]}];
        /* 2: 往復運賃 ＋ IC往復運賃 (割引無し) */
        [contentsForFare addObject:@{@"fare" : [[NSString stringWithFormat:@"¥%@",
                                                 [RouteDataController fareNumStr:_fareInfo.roundTripFareWithComapnyLine]]
                                                stringByAppendingString:str],
                                     @"subFare" : [NSString stringWithFormat:@"¥%@",
                                                   [RouteDataController fareNumStr:_fareInfo.fareForIC* 2]]}];
    } else {
        /* 1: 普通運賃 */
        contentsForFare = [NSMutableArray arrayWithObject:@{@"fare" : [NSString stringWithFormat:@"¥%@",
                                                                       [RouteDataController fareNumStr:_fareInfo.fareForJR + _fareInfo.fareForCompanyline]],
                                                            @"subTitle" : @"",
                                                            @"subFare" : @""}];
        /* 2: 往復運賃(割引可否) */
        [contentsForFare addObject:@{@"fare" : [[NSString stringWithFormat:@"¥%@",
                                                 [RouteDataController fareNumStr:_fareInfo.roundTripFareWithComapnyLine]]
                                                stringByAppendingString:str],
                                     @"subFare" : @""}];
    }
    
    /* 114 exception */
    if (_fareInfo.rule114_salesKm != 0) {
        num_of_fare++;
        [contentsForFare addObject:@{@"title" : @"規程114条適用運賃",
                                     @"fare" : [NSString stringWithFormat:@"¥%@",
                                                [RouteDataController fareNumStr:_fareInfo.rule114_fare]]}];
    }
    
    /* stock discount */
    num_of_fare += _fareInfo.availCountForFareOfStockDiscount;
    
    for (i = 0; i < _fareInfo.availCountForFareOfStockDiscount; i++) {
        [contentsForFare addObject:@{@"title" : [_fareInfo fareForStockDiscountTitle:i],
                                     @"fare" : [NSString stringWithFormat:@"¥%@",
                                                [RouteDataController fareNumStr:[_fareInfo fareForStockDiscount:i]]]}];
    }
}




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
///////////////////////////////////////////////
//  send mail
//
- (void)sendResultMail
{
    if (![MFMailComposeViewController canSendMail]) {
        return;
    }
    /// NSInteger cookedFlag;
    NSString * body =nil;
    NSString* subject = [self resultTitle];
    
    if (!_fareInfo || _fareInfo.result != 0) {
        return;   // Error message
    }
    
    body = [self resultMessage:subject];
    
    _mailViewCtl.mailComposeDelegate = self;
    
    // メール本文
    [_mailViewCtl setMessageBody:body isHTML:NO];
    
    // メール件名
    [_mailViewCtl setSubject:subject];
#if 0
    // 添付画像
    NSData *myData = [[NSData alloc] initWithData:UIImageJPEGRepresentation([UIImage imageNamed:@"xxx.jpg"], 1)];
    [mailViewCtl addAttachmentData:myData mimeType:@"image/jpeg" fileName:@"image"];
#endif
    
    // メールビュー表示
    [self presentViewController:_mailViewCtl animated:YES completion:nil];
}


- (void)ShowAirDrop
{
    if (!_fareInfo || _fareInfo.result != 0) {
        return;   // Error message
    }
    NSString* subject = [self resultTitle];
    NSString *shareText = [self resultMessage:subject];
    NSArray *activityItems = @[shareText];
    NSArray *excludeActivities = @[UIActivityTypePostToWeibo];
    
    UIActivityViewController *activityController = [[UIActivityViewController alloc] initWithActivityItems:activityItems applicationActivities:nil];
    
    // 除外サービスを指定
    activityController.excludedActivityTypes = excludeActivities;
    
    [activityController setValue:subject
                          forKey:@"subject"];
    
    // modalで表示
    [self presentViewController:activityController animated:YES completion:^{
        NSLog(@"Activity complete!!");
    }];
}

- (void)ShowOtherAppOpen
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDir = [paths objectAtIndex:0];
    
    NSString* dirPath = [NSString stringWithFormat:@"%@/cache", docDir];
    NSFileManager* fileManager = [NSFileManager defaultManager];
    // ディレクトリが存在するか確認する
    if (![fileManager fileExistsAtPath:dirPath])
    {
        // 存在しなければ、ディレクトリを作成する
        [fileManager createDirectoryAtPath:dirPath
               withIntermediateDirectories:YES
                                attributes:nil error:nil];
    }
  
    // データの書き込み
    NSString* filePath = [NSString stringWithFormat:@"%@/farert_result.txt", dirPath];
    
    [[self resultMessage:[self resultTitle]] writeToFile:filePath atomically:YES encoding:NSUnicodeStringEncoding error:nil];

    NSURL *url = [NSURL fileURLWithPath:filePath];

    self.docInterCon = [UIDocumentInteractionController interactionControllerWithURL:url];
    
    self.docInterCon.delegate = self;
    
    BOOL isValid;
    isValid = [self.docInterCon presentOpenInMenuFromRect:self.view.frame inView:self.view animated:YES];
    if (!isValid) {
        NSLog(@"データを開けるアプリケーションが見つかりません。");
    }
}

- (NSString*)resultTitle
{
    return [NSString stringWithFormat:@"運賃詳細(%@ - %@)",
     [RouteDataController TerminalName:_fareInfo.beginStationId],
     [RouteDataController TerminalName:_fareInfo.endStationId]];
}

- (NSString*)resultMessage:(NSString*)subject
{
    NSString *body = [NSString stringWithFormat:@"%@\n\n%@\n", subject, [_ds showFare]];
    body = [body stringByReplacingOccurrencesOfString:@"\\" withString:@"¥"];
    
    // メールビュー生成
    _mailViewCtl = [[MFMailComposeViewController alloc] init];
    
    if (IS_RULE_APPLIED_EN(_fareInfo.calcResultFlag)) {
        body = [body stringByAppendingString:@"(特例適用)\n"];
    } else {
        body = [body stringByAppendingString:@"(特例未適用)\n"];
    }
    body = [body stringByAppendingString:@"\n[指定経路]\n"];
    body = [body stringByAppendingString:[_ds routeScript]];

    return body;
}

#pragma mark - delegate

// アプリ内メーラーのデリゲートメソッド
// (メーラが終了した後に来る)
//
- (void)mailComposeController:(MFMailComposeViewController *)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError *)error
{
    switch (result) {
        case MFMailComposeResultCancelled:
            // キャンセル
            
            break;
        case MFMailComposeResultSaved:
            // 保存 (ここでアラート表示するなど何らかの処理を行う)
            
            break;
        case MFMailComposeResultSent:
            // 送信成功 (ここでアラート表示するなど何らかの処理を行う)
            
            break;
        case MFMailComposeResultFailed:
            // 送信失敗 (ここでアラート表示するなど何らかの処理を行う)
            
            break;
        default:
            break;
    }
    
    [self dismissViewControllerAnimated:YES completion:nil];
    
    _mailViewCtl = nil;
}


- (void)ShowAlertView:(NSString*)title message:(NSString*) message
{
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:title
                                                    message:message
                                                   delegate:self
                                          cancelButtonTitle:nil
                                          otherButtonTitles:@"OK", nil];
    [alertView show];

}

#pragma mark - Table view data source

//  Table section
//
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    if (_fareInfo && _fareInfo.result == 0) {
        return 5;
    } else {
        return 1;
    }
}

//  Table section
//
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    if (!_fareInfo || _fareInfo.result != 0) {
        return 1;   // Error message
    }
    switch (section) {
        case 0:
            return 1; /* section */
            break;
        case 1:
            return num_of_km; /* sales, calc km */
        case 2:
            return num_of_fare; /* Fare */
            break;
        case 3:         /* avail days */
        case 4:         /* route list */
            return 1;
            break;
        default:
            break;
    }
    return 0;
}

//
//
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    if (!_fareInfo || _fareInfo.result != 0) {
        return @"無効な経路";   // Error message
    }
    switch (section) {
        case 0:
            return @"区間"; /* section */
            break;
        case 1:
            return @"キロ程"; /* sales, calc km */
        case 2:
            return @"運賃"; /* Fare */
            break;
        case 3:
            return @"有効日数";
            break;
        case 4:
            return @"経由";  /* route list */
            break;
        default:
            break;
    }
    return nil;
}

//
//
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell* cell = nil;
    UILabel* lbl = nil;
    NSDictionary* dic = nil;
    NSString* str = nil;

    if (!_fareInfo || (_fareInfo.result != 0)) {    // error
        cell = [tableView dequeueReusableCellWithIdentifier:@"rsRouteListCell" forIndexPath:indexPath];
        //lbl = (UILabel*)[cell viewWithTag:1];
        //lbl.text = [_fareInfo routeList];
        cell.textLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
        cell.textLabel.numberOfLines = 0;
        if (!_fareInfo) {
            cell.textLabel.text = @"経路が空";
        } else {
            switch (_fareInfo.result) {
            case 1:
                cell.textLabel.text = @"経路が不完全のため運賃は表示できません";
                break;
            case 2:
                cell.textLabel.text = @"会社線のみ運賃は表示できません";
                break;
            default:
                cell.textLabel.text = @"運賃は表示できません";
                break;
            }
        }
        cell.textLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
        cell.textLabel.numberOfLines = 0;
        return cell;   // Error message
    }

    switch ([indexPath section]) {
        case 0:
            /* section */
            cell = [tableView dequeueReusableCellWithIdentifier:@"rsTitleCell" forIndexPath:indexPath];
            lbl = (UILabel*)[cell viewWithTag:1];
            lbl.text = [NSString stringWithFormat:@"%@ → %@", [RouteDataController TerminalName:_fareInfo.beginStationId],[RouteDataController TerminalName:_fareInfo.endStationId]];
            break;
        case 1:
            /* KM */
            dic = [contentsForKm objectAtIndex:indexPath.row];
            
            if ([indexPath row] == 0) {
                cell = [tableView dequeueReusableCellWithIdentifier:@"rsKmCell1" forIndexPath:indexPath];
                lbl = (UILabel*)[cell viewWithTag:2];
                lbl.text = dic[@"salesKm"];
                str = dic[@"calcKm"];
                if (0 < [str length]) {
                    lbl = (UILabel*)[cell viewWithTag:3];
                    lbl.text = dic[@"subTitle"];
                    lbl = (UILabel*)[cell viewWithTag:4];
                    lbl.text = str;
                } else {
                    lbl = (UILabel*)[cell viewWithTag:3];
                    lbl.text = @"";
                    lbl = (UILabel*)[cell viewWithTag:4];
                    lbl.text = @"";
               }
            } else {
                if (!dic[@"Company_salesKm"]) {
                    cell = [tableView dequeueReusableCellWithIdentifier:@"rsKmCell2" forIndexPath:indexPath];
                    lbl = (UILabel*)[cell viewWithTag:1];
                    lbl.text = dic[@"title"];
                    lbl = (UILabel*)[cell viewWithTag:2];
                    lbl.text = dic[@"salesKm"];
                    str = dic[@"calcKm"];
                    lbl = (UILabel*)[cell viewWithTag:4];
                    if (0 < [str length]) {
                        lbl.text = str;
                    } else {
                        lbl.text = @"";
                    }
                } else {
                    cell = [tableView dequeueReusableCellWithIdentifier:@"rsKmCell3" forIndexPath:indexPath];
                    lbl = (UILabel*)[cell viewWithTag:2];
                    lbl.text = dic[@"JR_salesKm"];
                    lbl = (UILabel*)[cell viewWithTag:4];
                    lbl.text = dic[@"Company_salesKm"];
                }
            }
            break;
        case 2:
            /* FARE */
            dic = [contentsForFare objectAtIndex:indexPath.row];

            if ([indexPath row] == 0) {
                cell = [tableView dequeueReusableCellWithIdentifier:@"rsFareCell" forIndexPath:indexPath];
                lbl = (UILabel*)[cell viewWithTag:2];
                lbl.text = dic[@"fare"];
                lbl = (UILabel*)[cell viewWithTag:3];
                lbl.text = dic[@"subTitle"];
                lbl = (UILabel*)[cell viewWithTag:4];
                lbl.text = dic[@"subFare"]; /* company or IC */
                
            } else if ([indexPath row] == 1) {
                cell = [tableView dequeueReusableCellWithIdentifier:@"rsFareCell2" forIndexPath:indexPath];
                lbl = (UILabel*)[cell viewWithTag:2];
                lbl.text = dic[@"fare"];
                lbl = (UILabel*)[cell viewWithTag:4];
                lbl.text = dic[@"subFare"];
            } else {
                cell = [tableView dequeueReusableCellWithIdentifier:@"rsDiscountFareCell" forIndexPath:indexPath];
                lbl = (UILabel*)[cell viewWithTag:1];
                lbl.text = dic[@"title"];
                lbl = (UILabel*)[cell viewWithTag:2];
                lbl.text = dic[@"fare"];
            }
            break;
        case 3:
            /* avail days */
            if (!_fareInfo.isArbanArea) {
                cell = [tableView dequeueReusableCellWithIdentifier:@"rsAvailDaysCell" forIndexPath:indexPath];
                lbl = (UILabel*)[cell viewWithTag:1];
                lbl.text = [NSString stringWithFormat:@"%ld日間", (long)_fareInfo.ticketAvailDays];
                lbl = (UILabel*)[cell viewWithTag:2];
                if (1 < _fareInfo.ticketAvailDays) {
                    if (_fareInfo.isBeginInCiry && _fareInfo.isEndInCiry) {
                        str = @"(発着駅都区市内の駅を除き";
                    } else if (_fareInfo.isBeginInCiry) {
                        str = @"(発駅都区市内の駅を除き";
                    } else if (_fareInfo.isEndInCiry) {
                        str = @"(着駅都区市内の駅を除き";
                    } else {
                        str = @"(";
                    }
                    lbl.text = [str stringByAppendingString:@"途中下車可能)"];
                } else {
                    lbl.text = @"(途中下車前途無効)";
                }
            } else {
                cell = [tableView dequeueReusableCellWithIdentifier:@"rsMetroAvailDaysCell" forIndexPath:indexPath];
            }
            break;
        case 4:
            /* ROUTE */
            cell = [tableView dequeueReusableCellWithIdentifier:@"rsRouteListCell" forIndexPath:indexPath];
            //lbl = (UILabel*)[cell viewWithTag:1];
            //lbl.text = [_fareInfo routeList];
            cell.textLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
            cell.textLabel.numberOfLines = 0;
            cell.textLabel.text = [_fareInfo routeList];
            break;
        default:
            break;
    }    
    return cell;
}



/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/


#pragma mark - Table view delegate
#if 0
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return 30.0;
}
#endif

//  行の高さ
//
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSString* ident = nil;
    NSString* text = nil;
    UIFont *nameLabelFont = nil;
    CGFloat PADDING_OUTER = 30;
    CGRect totalRect;
    NSDictionary* dic = nil;
    
    switch ([indexPath section]) {
        case 0:
            /* section */
            ident = @"rsTitleCell";
            break;
        case 1:
            /* KM */
            if ([indexPath row] == 0) {
                ident = @"rsKmCell1";
            } else {
                dic = [contentsForKm objectAtIndex:indexPath.row];
                if (!dic[@"Company_salesKm"]) {
                    ident = @"rsKmCell2";
                } else {
                    ident = @"rsKmCell3";
                }
            }
            break;
        case 2:
            /* FARE */
            if ([indexPath row] == 0) {
                ident = @"rsFareCell";
            } else if ([indexPath row] == 1) {
                ident = @"rsFareCell2";
            } else {
                ident = @"rsDiscountFareCell";
            }
            break;
        case 3:
            /* avail days */
            if (!_fareInfo.isArbanArea) {
                ident = @"rsAvailDaysCell";
            } else {
                ident = @"rsMetroAvailDaysCell";
            }
            break;
        case 4:
            /* ROUTE */
            text = [_fareInfo routeList];
            nameLabelFont = [UIFont preferredFontForTextStyle:UIFontTextStyleHeadline];
            
            totalRect = [text boundingRectWithSize:CGSizeMake(self.tableView.frame.size.width, CGFLOAT_MAX)
                                                  options:(NSStringDrawingUsesLineFragmentOrigin|NSStringDrawingUsesFontLeading)
                                               attributes:[NSDictionary dictionaryWithObject:nameLabelFont forKey:NSFontAttributeName]
                                                  context:nil];
            return totalRect.size.height + PADDING_OUTER;   // >>>>>>>>>>>>>>>>>>>>>
            break;
        default:
            break;
    }
    if (ident) {
        UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:ident];
        //NSLog(@"%@ %u %u %f, %f", ident, indexPath.section, indexPath.row, cell.frame.size.height, cell.bounds.size.height);
        return cell.frame.size.height;
    }
    return [super tableView:tableView heightForRowAtIndexPath:indexPath];
}



@end
