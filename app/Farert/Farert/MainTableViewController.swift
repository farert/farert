//
//  MainTableViewController.swift
//  iFarert
//
//  Created by TAKEDA, Noriyuki on 2015/04/03.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit


class MainTableViewController: UITableViewController, UIActionSheetDelegate, TableHeaderViewDelegate, UIAlertViewDelegate {
    
    // MARK: - Constant
    let WELCOME_TITLE: String = "ダウンロードありがとうございます"
    let WELCOME_MESSAGE: String = "本アプリで表示される結果は必ずしも正確な情報ではないことがありえます。実際のご旅行での費用とは異なることがありえますことをご理解の上ご利用ください。\n本アプリにより発生したあらゆる損害は作者は負いません\n本アプリ及び提供元は各鉄道会社とは関係もありません"
    // MARK: - property
    let HEADER_HEIGHT:CGFloat        = 44.0
    let FOOTER_HEIGHT:CGFloat        = 51.0
    
    let ROUTE_DUP_ERROR:Int          = 1
    let ROUTE_FINISH:Int             = 2
    let ROUTE_FARERESULT_ERR1:Int    = 3
    let ROUTE_FARERESULT_ERR2:Int    = 4
    let ROUTE_FARERESULT_FAIL:Int    = 5
    let ROUTE_SCRIPT_STATION_ERR:Int = 6
    let ROUTE_SCRIPT_LINE_ERR:Int    = 7
    let ROUTE_SCRIPT_ROUTE_ERR:Int   = 8
    let ROUTE_DUPCHG_ERROR:Int       = 9
    
    let TAG_UIACTIONSHEET_AUTOROUTE:Int         = 10
    let TAG_UIACTIONSHEET_QUERYSETUPROUTE:Int   = 11
    let TAG_UIACTIONSHEET_OSAKAKANDETOUR:Int    = 12
    
    let LPROC_REVERSE:Int = 1
    let LPROC_AUTOROUTE:Int = 2
    let LPROC_SETUPROUTE:Int = 3

    // MARK: - Global variables
    
    var viewContextMode: Int = 0;
    var routeStat: Int = 0;
    var scroll_flag: Bool = false;
    var longTermFuncMode: Int = 0
    
    // MARK: - UI variables

    @IBOutlet weak var reverseBarButton: UIBarButtonItem!
    @IBOutlet weak var replayBarButton: UIBarButtonItem!
    @IBOutlet weak var organaizerBarButton: UIBarButtonItem!
    @IBOutlet weak var actionBarButton: UIBarButtonItem!
    
    // MARK: Local variables
    
    var ds: RouteDataController!
    var fareInfo: FareInfo?
    var routeScript: String?
    var frontView: UIView!
    var indicator: UIActivityIndicatorView!

    
    // MARK: - View functions
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }
    

    //  View Load
    //
    //
    override func viewDidLoad() {
        super.viewDidLoad()

        // Uncomment the following line to preserve selection between presentations.
        self.clearsSelectionOnViewWillAppear = false;
                    
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem;
                    
        viewContextMode = 0;
                    
        //NSLog(@"MainView didLoad");
        
        //self.replayBarButton.accessibilityHint = @"経路を一つ前に戻ります";
        //self.organaizerBarButton.accessibilityHint = @"保持経路管理ビューを開きます";
        self.navigationItem.rightBarButtonItem?.enabled = true
        self.tableView.rowHeight = 44.0;
        self.actionBarButton.enabled = false;
        ds = RouteDataController()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    //  View will Appear
    // Called when the view is about to made visible. Default does nothing
    //
    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
    
        //  self.navigationController.toolbarHidden = NO;
    }
    
    
    //  View did apear
    //
    //
    override func viewDidAppear(animated: Bool) {
        super.viewDidAppear(animated);
    
        if NSUserDefaults.standardUserDefaults().boolForKey("HasLaunchedOnce") {
            // ２回目以降の起動時
        } else {
            // 初回起動時
            // AlertView(iOS7, iOS8)
            if objc_getClass("UIAlertController") != nil {
                /* iOS8 */
                var ac = UIAlertController(title: WELCOME_TITLE, message: WELCOME_MESSAGE, preferredStyle: .Alert)
                let otherAction = UIAlertAction(title: "了解", style: .Default) {
                    action in
                }
                ac.addAction(otherAction)
                self.presentViewController(ac, animated: true, completion: nil)
                
            } else {
                /* iOS7 */
                var av = UIAlertView(title: WELCOME_TITLE, message: WELCOME_MESSAGE, delegate: nil, cancelButtonTitle: "了解")
                av.show()
            }
            NSUserDefaults.standardUserDefaults().setBool(true, forKey: "HasLaunchedOnce")
            NSUserDefaults.standardUserDefaults().synchronize();
        }

        
        
        // ここから
        if viewContextMode == FGD.CONTEXT_AUTOROUTE_ACTION {
            
            let appDelegate:AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
            if let selid = appDelegate.selectTerminalId {
                self.actionSheetController(
                    ["新幹線を使う", "新幹線をつかわない（在来線のみ）"],
                    title: RouteDataController.StationName(selid) + "までの最短経路追加",
                    message: "",
                    from: TAG_UIACTIONSHEET_AUTOROUTE)
            }
    
        } else if viewContextMode == FGD.CONTEXT_ROUTESELECT_VIEW {
            /* from 経路追加 */
            scroll_flag = true
            
        } else if viewContextMode == FGD.CONTEXT_BEGIN_TERMINAL_ACTION {
            // 開始駅選択による経路破棄の確認
            self.actionSheetController(["はい", "いいえ"],
                title: "表示経路を破棄してよろしいですか？",
                message: "",
                from: TAG_UIACTIONSHEET_QUERYSETUPROUTE)
            
        } else if (viewContextMode == FGD.CONTEXT_ROUTESETUP_VIEW) {
            // from 保持経路ビュー
            showIndicate();    /* start Activity and Disable UI */
            self.navigationController!.view!.userInteractionEnabled = false;
            longTermFuncMode = LPROC_SETUPROUTE;
            let time = dispatch_time(DISPATCH_TIME_NOW, Int64(0.1 * Double(NSEC_PER_SEC)))
            dispatch_after(time, dispatch_get_main_queue(), {
                NSThread.detachNewThreadSelector(Selector("processDuringIndicatorAnimating:"), toTarget:self, withObject: self.routeScript)
            })
        }
        viewContextMode = 0;
        // ここまでは ObjCまでは、viewWillApear にあったが、こっちに写してみた

        // セルの選択を解除
        if let idx : NSIndexPath = self.tableView.indexPathForSelectedRow() {
            self.tableView.deselectRowAtIndexPath(idx, animated:false)
        }
        
        if (scroll_flag) {
            // （追加）のあとのスクロール
            scrollTableView() // Tableview scroll-up
            scroll_flag = false;
        }
    }
    
    //  長い処理
    //  longTermFuncMode != 0
    //
    func processDuringIndicatorAnimating(param: AnyObject) {
        var rc: Int = -1;
        let appDelegate:AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate

        switch (longTermFuncMode) {
        case LPROC_REVERSE:
            rc = ds.reverseRoute()
            fareInfo = ds.calcFare()
            if rc < 0 {
                //[self alertMessage:@"経路追加エラー" message:@"経路が重複している等反転できません."];
                routeStat = ROUTE_DUP_ERROR;
            }
        case LPROC_AUTOROUTE:
            // buttonIndex : 0 = 新幹線を使う
            //               1 = 新幹線を使わない
            var bullet : Int
            if let nsid : NSNumber = param as? NSNumber {
                bullet = Int(nsid)
            } else {
                bullet = 0
            }
            //let n = param as? Int
            rc = ds.autoRoute(bullet != 1, arrive: appDelegate.selectTerminalId!);
            fareInfo = ds.calcFare()
            if (rc < 0) {
                //[self alertMessage:@"経路追加エラー" message:@"経路が重複している等追加できません."];
                routeStat = ROUTE_DUP_ERROR;
            } else if (rc == 0) {
                routeStat = ROUTE_FINISH;
            }
            scroll_flag = true;
            
        case LPROC_SETUPROUTE:
            let rs : String? = param as? String
            if (nil == rs) || (rs!.isEmpty == true) {
                longTermFuncMode = 0
                hideIndicate()          /* hide Activity and enable UI */
                return
            }
            rc = ds.setupRoute(rs)
            fareInfo = ds.calcFare()
            if (rc < 0) {
                //[self alertMessage:@"経路追加エラー" message:@"経路が重複している等追加できません."];
                switch (rc) {
                case -200:
                    routeStat = ROUTE_SCRIPT_STATION_ERR;
                case -300:
                    routeStat = ROUTE_SCRIPT_LINE_ERR;
                case -2:
                    routeStat = ROUTE_SCRIPT_ROUTE_ERR;
                default:
                    routeStat = ROUTE_DUP_ERROR;    // -1: 経路重複エラー
                }
            } else { // (0 <= result)
                // 0=end, plus=continue
                if ((rc == 0) && (1 < ds.getRouteCount())) {
                    routeStat = ROUTE_FINISH;
                } else {
                    routeStat = 0;  // success
                }
                viewContextMode = FGD.CONTEXT_ROUTESELECT_VIEW;
                scroll_flag = true;
            }
        default:
            assert(false, "bug:\(__FILE__) : \(__LINE__))")
            break
        }
        fareResultSetting(rc)   /* 簡易結果(Footer section) */
        longTermFuncMode = 0
        hideIndicate()          /* hide Activity and enable UI */
    }

    // MARK: - Table view data source

    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 1
    }

    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        // Return the number of rows in the section.
        var num : Int = ds.getRouteCount()
        
        if (ds.startStationId() <= 0) {
            num = 0
        }
        if (0 < num) {
            //self.navigationItem.rightBarButtonItem.enabled = YES;
            self.replayBarButton.enabled = true;
        } else {
            //self.navigationItem.rightBarButtonItem.enabled = NO;
            self.replayBarButton.enabled = false;
        }
        if (1 < num) {
            self.reverseBarButton.enabled = true;
        } else {
            self.reverseBarButton.enabled = false;
        }
        return num;
    }

    //  TableView データ描画・設定
    //
    //
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        
        if (ds.getRouteCount() - 1) <= indexPath.row {
            /*
            * 最後の行
            */
            let cell = tableView.dequeueReusableCellWithIdentifier("LastRouteCell") as! UITableViewCell
            let lbl = cell.viewWithTag(201) as! UILabel
            if (routeStat != 0) {
                switch (routeStat) {
                case ROUTE_DUP_ERROR:
                    lbl.text = "経路が重複している等追加できません";
                    break;
                case ROUTE_FINISH:
                    lbl.text = "経路が終端に達しました";
                    break;
                case ROUTE_FARERESULT_ERR1:
                    lbl.text = "不完全ルート：経路を追加または削除してください.";
                    break;
                case ROUTE_FARERESULT_ERR2:
                    lbl.text = "会社線のみの運賃は表示できません.";
                    break;
                case ROUTE_SCRIPT_STATION_ERR:
                    lbl.text = "不正な駅名が含まれています.";
                    break;
                case ROUTE_SCRIPT_LINE_ERR:
                    lbl.text = "不正な路線名が含まれています.";
                    break;
                case ROUTE_SCRIPT_ROUTE_ERR:
                    lbl.text = "経路不正";
                    break;
                case ROUTE_DUPCHG_ERROR:
                    lbl.text = "経路が重複していますので変更できません";
                    break;
                default:
                    lbl.text = "unknown error";
                    break;
                }
                lbl.hidden = false;
            } else {
                lbl.hidden = true;
            }
            return cell
        } else {
            /* 最後の行以外の普通の行
            *
            */
            let cell = tableView.dequeueReusableCellWithIdentifier("RouteContentCell") as! FarertTableViewCell
            
            let ri = ds.getRouteItem(indexPath.row + 1)
            cell.lineName.text = ri.line;
            cell.stationName.text = ri.station;

            return cell
        }
    }

    /*
    // Override to support conditional editing of the table view.
    override func tableView(tableView: UITableView, canEditRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        // Return NO if you do not want the specified item to be editable.
        return true
    }
    */

    /*
    // Override to support editing the table view.
    override func tableView(tableView: UITableView, commitEditingStyle editingStyle: UITableViewCellEditingStyle, forRowAtIndexPath indexPath: NSIndexPath) {
        if editingStyle == .Delete {
            // Delete the row from the data source
            tableView.deleteRowsAtIndexPaths([indexPath], withRowAnimation: .Fade)
        } else if editingStyle == .Insert {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }
    }
    */

    /*
    // Override to support rearranging the table view.
    override func tableView(tableView: UITableView, moveRowAtIndexPath fromIndexPath: NSIndexPath, toIndexPath: NSIndexPath) {

    }
    */

    /*
    // Override to support conditional rearranging of the table view.
    override func tableView(tableView: UITableView, canMoveRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        // Return NO if you do not want the item to be re-orderable.
        return true
    }
    */

    //  ヘッダタイトル
    //
    //
    override func tableView(tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        let tableHeaderViewCell : HeaderTableViewCell = tableView.dequeueReusableCellWithIdentifier("HeaderCell") as! HeaderTableViewCell
        var label : UILabel?
        var title : String
        var subtitle : String

        
        if 0 < ds.startStationId() {
            /* 開始駅 */
            title = RouteDataController.StationNameEx(ds.startStationId())
            /* ひらかな */
            subtitle = RouteDataController.GetKanaFromStationId(ds.startStationId())
        } else {
            title = "";
            subtitle = "発駅を入力してください";
        }
        /* 開始駅 */
        label = tableHeaderViewCell.viewWithTag(10001) as! UILabel?
        label?.text = title;

        /* ひらかな */
        label = tableHeaderViewCell.viewWithTag(10002) as! UILabel?
        label?.text = subtitle
        
        tableHeaderViewCell.delegate = self;
        
        return tableHeaderViewCell as UIView;
    }

    //  フッタタイトル
    //
    //
    override func tableView(tableView: UITableView, viewForFooterInSection section: Int) -> UIView? {
        
        let cell : HeaderTableViewCell = tableView.dequeueReusableCellWithIdentifier("footerCell") as! HeaderTableViewCell
        let lbl1 : UILabel = cell.viewWithTag(40001) as! UILabel
        let lbl2 : UILabel = cell.viewWithTag(40002) as! UILabel
        let lbl3 : UILabel = cell.viewWithTag(40003) as! UILabel

        if (ds.getRouteCount() <= 0) || (fareInfo == nil) || (fareInfo!.result != 0) {
            lbl1.text = ""
            lbl2.text = ""
            lbl3.text = ""
        } else {
            lbl1.text = RouteDataController.fareNumStr(fareInfo!.fareForCompanyline + ((fareInfo!.rule114_salesKm != 0) ?
                fareInfo!.rule114_fare : fareInfo!.fareForJR)) + "円"
            lbl2.text = RouteDataController.kmNumStr(fareInfo!.totalSalesKm) + "km"
            lbl3.text = "\(fareInfo!.ticketAvailDays)日間"
        }
        cell.delegate = self;   // enable touch
        
        return cell;
    }
    
    //  ヘッダ高さ
    //
    //
    override func tableView(tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {

        //let cell : UITableViewCell = tableView.dequeueReusableCellWithIdentifier("HeaderCell") as! UITableViewCell
        
        if section == 0 {
            return HEADER_HEIGHT     // cell.contentView.bounds.size.height
        } else {
            return UITableViewAutomaticDimension
        }
    }
    
    //  フッタ高さ
    //
    //
    override func tableView(tableView: UITableView, heightForFooterInSection section: Int) -> CGFloat {

        if section == 0 {
            if (1 < ds.getRouteCount()) {
                return FOOTER_HEIGHT
            }
        }
        return UITableViewAutomaticDimension
    }
    
    //  行の高さ
    //
    //
    override func tableView(tableView: UITableView, estimatedHeightForRowAtIndexPath indexPath : NSIndexPath) -> CGFloat {
     
        return UITableViewAutomaticDimension
    }
    
    // MARK: - delegate

    //  Touched on tableView Header / Footer
    //
    //
    func tableHeaderViewTouched(tableHeaderViewCell : HeaderTableViewCell) {
        if (tableHeaderViewCell.tag == 100) {
            /* header */
            self.performSegueWithIdentifier("terminalSelectSegue", sender:self);
            
        } else if (tableHeaderViewCell.tag == 200) {
            /* Footer */
            let lastIndex : Int = ds.getRouteCount() - 2; // last item
            if (0 <= lastIndex) {
                scroll_flag = true; // Tableview scroll-up
                let indexPath : NSIndexPath = NSIndexPath(forRow: lastIndex, inSection: 0)
                self.tableView.selectRowAtIndexPath(indexPath, animated: false, scrollPosition: UITableViewScrollPosition.None)
                self.performSegueWithIdentifier("fareInfoDetailSegue", sender:self)
            }
        }
    }


    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        let segid : String = segue.identifier!
        let apd:AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate

        if segid == "terminalSelectSegue" {
            apd.context = FGD.CONTEXT_TERMINAL_VIEW;
            //        vc.delegate = self;
        } else if segid == "routeLineSegue" {
            /* 経路追加(駅に属する路線一覧 */
            let selectLineTblViewController : SelectLineTableViewController = segue.destinationViewController as! SelectLineTableViewController;
            selectLineTblViewController.baseStationId = ds.lastStationId()
            selectLineTblViewController.companyOrPrefectId = 0
            selectLineTblViewController.lastLineId = ds.lastLineId()
            
            apd.context = FGD.CONTEXT_ROUTESELECT_VIEW;
            
        } else if segid == "fareInfoDetailSegue" {
            /* 運賃詳細ビュー */
            let idx = self.tableView.indexPathForSelectedRow()?.row ?? 0
            let resultViewCtlr : ResultTableViewController = segue.destinationViewController as! ResultTableViewController
            resultViewCtlr.ds = RouteDataController(assign: self.ds, count: idx + 2)
            apd.context = 0;
            
        } else if segid == "routeManagerSegue" {
            /* 経路一覧ビュー */
            let naviCtlr : UINavigationController = segue.destinationViewController as! UINavigationController
            let routeMngViewCtlr : ArchiveRouteTableViewController = naviCtlr.topViewController as! ArchiveRouteTableViewController
            if ds.getRouteCount() <= 1 {
                routeMngViewCtlr.currentRouteString = "";
            } else {
                routeMngViewCtlr.currentRouteString = ds.routeScript()
            }
            apd.context = 0;
        } else if segid == "versionInfoTransitSegue" {
            //
        } else if segid == "settingsSegue" {
            /// // save Route
            self.routeScript = ds.routeScript().stringByTrimmingCharactersInSet(NSCharacterSet.whitespaceAndNewlineCharacterSet())
        } else {
            assert(false, "bug:\(__FILE__) \(__LINE__) segid=\(segid)")
        }
        //NSLog(@"prepareForSegue:%@", [segue identifier]);
    }

    // MARK: - unwind segue
    
    @IBAction func doneTerminal(segue: UIStoryboardSegue)
    {
        let seg_id : String? = segue.identifier
        
        //NSLog(@"doneTerminal:%@", seg_id);
        
        viewContextMode = 0;
        
        if seg_id == "termSelectDone" ||
           seg_id == "terminalSelectDoneSegue" ||
           seg_id == "doneTerminalHistorySegue" {
            /* from TermSelectTableViewController - Searchbar */
            /* from SelectStationTableViewController */
            /* from History View */
            var termId : Int = 0;
            
            ///UIViewController* v = (UIViewController*)[segue sourceViewController];
            let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
            //termId = [[app selectTerminalId] intValue];
            termId = apd.selectTerminalId ?? 0
            if (apd.context == FGD.CONTEXT_TERMINAL_VIEW) {
                // from 発駅
                //NSLog(@"begin station=%d, %@", termId, [RouteDataController StationName:termId]);
                //if ([_ds startStationId] == termId) {
                //    return;
                //}
                if (1 < ds.getRouteCount()) && !RouteDataController.IsRouteInStrage(ds.routeScript()) {
                    viewContextMode = FGD.CONTEXT_BEGIN_TERMINAL_ACTION;
                    // つつきは、viewDidApear:　で
                } else {
                    self.setBeginTerminal()
                }
            } else if (apd.context == FGD.CONTEXT_AUTOROUTE_VIEW) {
                // from 最短経路
                // auto route
                //NSLog(@"autoroute end station=%d, %@", termId, [RouteDataController StationName:termId]);
                self.navigationController?.popViewControllerAnimated(false)

                // AlartViewで、新幹線を使うか否かを訊いてautoroute
                //BOOL bulletUse = [self queryDialog:@"新幹線を使用しますか?"];
                viewContextMode = FGD.CONTEXT_AUTOROUTE_ACTION;
                RouteDataController.SaveToTerminalHistory(RouteDataController.StationNameEx(termId))
                // つつきは、viewWilApear:　で
                
            } else if (apd.context == FGD.CONTEXT_ROUTESELECT_VIEW) {
                // from 経路追加
                /* add route */
                let result : Int = ds.addRoute(apd.selectLineId, stationId: apd.selectTerminalId)
                
                fareInfo = ds.calcFare()
                self.fareResultSetting(result)
                
                if (result < 0) {
                    //[self alertMessage:@"経路追加エラー" message:@"経路が重複している等追加できません."];
                    routeStat = ROUTE_DUP_ERROR;
                } else { // (0 <= result)
                    // 0=end, plus=continue
                    if (result == 0) {
                        routeStat = ROUTE_FINISH;
                    }
                }
                viewContextMode = FGD.CONTEXT_ROUTESELECT_VIEW;
                self.tableView.reloadData()
            } else {
                assert(false, "bug");
            }
        } else if (seg_id == "unwindArchiveRouteSelectSegue") {
            // from 保持経路一覧
            // setup route
            let routeViewCtrl : ArchiveRouteTableViewController = segue.sourceViewController as! ArchiveRouteTableViewController
            if !routeViewCtrl.selectRouteString.isEmpty {
                viewContextMode = FGD.CONTEXT_ROUTESETUP_VIEW;
                self.routeScript = routeViewCtrl.selectRouteString;
            }
        } else {
            //NSAssert(nil, @"bug");
            println("Bug?? \(segue.identifier)")
        }
    }

    // exit(unwind) segue
    //  from: ArchiveRouteTableViewController - [Close] button
    //        TerminalHistoryTableViewController - [Close] button
    //         TermSelectTableViewController - [Close] button
    //
    @IBAction func cancelTerminal(segue: UIStoryboardSegue) {
        self.navigationController?.popViewControllerAnimated(false)

        println("canelTerminal segue=\(segue.identifier)")
    }
    
    // exit(unwind) segue
    //  from: VersionInfoViewController
    //
    @IBAction func closeModal(segue: UIStoryboardSegue) {
        println("MainView: closeModal:")
        if segue.identifier == "settingsSegue" {
            let view : SettingsTableViewController = segue.sourceViewController as! SettingsTableViewController
            if (0 < view.selectDbId) {  // is change DB
                viewContextMode = FGD.CONTEXT_ROUTESETUP_VIEW;
            }
            
            /* settingsのあとでは「while a presentation or dismiss is in progress!」警告が表示される */
        } else if (segue.identifier == "versionInfoExitSegue") {
            //if self.navigationController?.isBeingDismissed() != true {
                self.dismissViewControllerAnimated(true, completion: nil)
            //}
        } else {
            assert(false)
        }
    }

    // MARK: - Activity Indicator
    
    func showIndicate() {
        self.frontView = UIView(frame: self.navigationController!.view.bounds)
        self.frontView.backgroundColor = UIColor.clearColor()
        self.navigationController!.view!.addSubview(self.frontView)
    
        self.indicator = UIActivityIndicatorView(activityIndicatorStyle: UIActivityIndicatorViewStyle.WhiteLarge)
        self.indicator.color = UIColor.blackColor()
        self.indicator.center = self.frontView.center
        self.frontView.addSubview(self.indicator)
        self.frontView.bringSubviewToFront(self.indicator)
        self.indicator.startAnimating()
        
    }
    
    func hideIndicate() {
        self.indicator.stopAnimating()
        self.indicator.removeFromSuperview()
        self.frontView.removeFromSuperview()
        self.indicator = nil;
        self.frontView = nil;
        
        self.navigationController?.view.userInteractionEnabled = true;
        self.tableView.reloadData()
    }

    // 逆転
    @IBAction func reverseAction(sender: UIBarButtonItem) {
        if (ds.getRouteCount() < 2) {
            return;
        }
        
        self.showIndicate()    /* start Activity and Disable UI */
        self.navigationController?.view?.userInteractionEnabled = false;
        longTermFuncMode = LPROC_REVERSE
        // [self performSelector:@selector(processDuringIndicatorAnimating:) withObject:nil afterDelay:0.1];
        let time = dispatch_time(DISPATCH_TIME_NOW, Int64(0.1 * Double(NSEC_PER_SEC)))
        dispatch_after(time, dispatch_get_main_queue(), {
            NSThread.detachNewThreadSelector(Selector("processDuringIndicatorAnimating:"), toTarget:self, withObject: nil)
        })
    }
    
    // バック（1つ前に戻る)
    @IBAction func backAction(sender: UIBarButtonItem) {
        if (1 < ds.getRouteCount()) {
            ds.removeTail()
            fareInfo = ds.calcFare()
            self.fareResultSetting(1)
        } else {
            ds.resetStartStation()    // removeAll, clear start
        }
        self.tableView.reloadData()
        self.scrollTableView()
    }
    
    // action menu
    @IBAction func checkAction(sender: UIBarButtonItem) {
        let flg : Int = ds.getFareOption()
        
        if DbUtil.IsOsakakanDetourEnable(flg) {
            self.actionSheetController(DbUtil.IsOsakakanDetourShortcut(flg) ?
                ["大阪環状線 近回り"] : ["大阪環状線 遠回り"],
                title: "大阪環状線経由指定", message: "",
                from: TAG_UIACTIONSHEET_OSAKAKANDETOUR)
        }
    }
    
    
    // begin Action select menu.
    func actionSheetController(menu_list : [String], title : String, message : String, from : Int) {
        
        if nil != objc_getClass("UIAlertController") {
            // iOS8
            let ac : UIAlertController = UIAlertController(title: title, message: message, preferredStyle: .ActionSheet)
            for item : String in menu_list {
                ac.addAction(UIAlertAction(title: item, style: .Default, handler: { (action: UIAlertAction!) in self.actionSelectProcFrom(from, label: item)}))
            }
            if nil == menu_list.last!.rangeOfString("いいえ") {
                ac.addAction(UIAlertAction(title: "キャンセル", style: .Cancel, handler: {(action: UIAlertAction!) in self.actionSelectProcFrom(from, label: "キャンセル")}))
            }
            // for iPad
            ac.modalPresentationStyle = UIModalPresentationStyle.Popover
            switch (from) {
            case TAG_UIACTIONSHEET_OSAKAKANDETOUR:
                ac.popoverPresentationController?.barButtonItem = self.actionBarButton
            case TAG_UIACTIONSHEET_AUTOROUTE:
                ac.popoverPresentationController?.sourceView = self.view
                ac.popoverPresentationController?.sourceRect = self.tableView.rectForFooterInSection(0)
            case TAG_UIACTIONSHEET_QUERYSETUPROUTE :
                ac.popoverPresentationController?.sourceView = self.view
                ac.popoverPresentationController?.sourceRect = self.tableView.rectForHeaderInSection(0)
            default:
                break
            }
            // end of for iPad
            self.presentViewController(ac, animated: true, completion: nil)
            
        } else {
            // iOS7
            let actsheet : UIActionSheet = UIActionSheet()
            
            actsheet.delegate = self
            actsheet.title = title
            
            for item : String in menu_list {
                actsheet.addButtonWithTitle(item)
            }
            if nil == menu_list.last!.rangeOfString("いいえ") {
                actsheet.addButtonWithTitle("キャンセル")
                actsheet.cancelButtonIndex = actsheet.numberOfButtons - 1
            }
            actsheet.tag = from
            
            if UIDevice.currentDevice().userInterfaceIdiom == .Pad {
                self.clearsSelectionOnViewWillAppear = false
                self.preferredContentSize = CGSize(width: self.navigationController!.view!.frame.width/2, height: self.view!.frame.height)
                switch (from) {
                case TAG_UIACTIONSHEET_OSAKAKANDETOUR:
                    actsheet.showFromBarButtonItem(self.actionBarButton, animated: true)
                case TAG_UIACTIONSHEET_AUTOROUTE:
                    actsheet.showFromRect(self.tableView.rectForRowAtIndexPath(self.tableView.indexPathForSelectedRow()!), inView: self.view, animated: true)
                case TAG_UIACTIONSHEET_QUERYSETUPROUTE:
                    actsheet.showFromRect(self.tableView.rectForHeaderInSection(0), inView: self.view, animated: true)
                default:
                    assert(false)
                    break
                }
            } else {
                let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
                let win : UIWindow = apd.window!
                
                if contains(win.subviews as! [UIView], self.tableView as UIView) {
                    actsheet.showInView(self.view)
                } else {
                    actsheet.showInView(win)
                }
            }
        }
        //
    }

    //  Action Sheet
    //   - Auto route
    //   - Query clear remove by change begin terminal.
    //   - Change root for OOSAKA Kanjyou-sen
    //
    func actionSheet(actionSheet : UIActionSheet, clickedButtonAtIndex buttonIndex : Int) {
        if buttonIndex < 0 {
            actionSelectProcFrom(actionSheet.tag, label: "キャンセル")
        } else {
            actionSelectProcFrom(actionSheet.tag, label: actionSheet.buttonTitleAtIndex(buttonIndex))
        }
    }
    
    func actionSelectProcFrom(from : Int, label title : String) {

        let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
        var rc : Int
        
        if (from == TAG_UIACTIONSHEET_AUTOROUTE) {
            if (apd.context != FGD.CONTEXT_AUTOROUTE_VIEW) {
                return;
            }
            if (nil != title.rangeOfString("キャンセル")) {
                return;
            }
            /*  最短経路 doneTerminal: -> willApear: ->
            *
            */
            self.showIndicate()    /* start Activity and Disable UI */
            self.navigationController?.view?.userInteractionEnabled = false;
            longTermFuncMode = LPROC_AUTOROUTE;
            //[self performSelector:@selector(processDuringIndicatorAnimating:)
            //withObject:[NSNumber numberWithInteger:buttonIndex]
            //afterDelay:0.1];
            let selectIndex : Int = (nil != title.rangeOfString("使う")) ? 0 : 1
            let time = dispatch_time(DISPATCH_TIME_NOW, Int64(0.1 * Double(NSEC_PER_SEC)))
            dispatch_after(time, dispatch_get_main_queue(), {
                NSThread.detachNewThreadSelector(Selector("processDuringIndicatorAnimating:"), toTarget:self, withObject: selectIndex)
            })
            
        } else if (from == TAG_UIACTIONSHEET_QUERYSETUPROUTE) {
            /*  経路表示時の１行目の発駅変更(既存経路破棄→新規発駅設定)
            * doneTerminal: -> willApear
            */
//            if (selectIndex == 0) {
            if nil != title.rangeOfString("はい") {
                self.setBeginTerminal()
                self.tableView.reloadData()
            }
            
        } else if (from == TAG_UIACTIONSHEET_OSAKAKANDETOUR) {
            /*  大阪環状線 遠回り／近回り
            */
            if nil == title.rangeOfString("キャンセル") {
                if (nil != title.rangeOfString("大阪環状線")) {
                    rc = ds.setDetour(nil != title.rangeOfString("遠") ? true : false)
                    fareInfo = ds.calcFare()
                    if (rc < 0) {
                        routeStat = ROUTE_DUPCHG_ERROR;
                    } else if (rc == 0) {
                        routeStat = ROUTE_FINISH;
                    } else {
                        routeStat = 0;  /* success */
                    }
                    viewContextMode = FGD.CONTEXT_ROUTESELECT_VIEW;
                    self.tableView.reloadData()
                }
            }
        }
    }

    // MARK: - local
    
    func fareResultSetting(rc: Int) {

        var opt : Int
    
        if (0 < rc) {
            switch (fareInfo?.result ?? 0) {
            case 0:
                routeStat = 0;
            case 1:
                routeStat = ROUTE_FARERESULT_ERR1;
            case 2:
                routeStat = ROUTE_FARERESULT_ERR2;
            default:
                routeStat = ROUTE_FARERESULT_FAIL;
            }
        }
        
        opt = ds.getFareOption()
        
        if DbUtil.IsOsakakanDetourEnable(opt) {
            self.actionBarButton.enabled = true
        } else {
            self.actionBarButton.enabled = false
        }
    }
    
    //  発駅設定・表示（ヘッダタップ、最短経路
    //
    //
    func setBeginTerminal() {
        let appDelegate:AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
        
        let termId : Int = appDelegate.selectTerminalId
        ds.addRoute(termId)
        routeStat = 0;
        tableView.reloadData()
        RouteDataController.SaveToTerminalHistory(RouteDataController.StationNameEx(termId))
    }
    
    //  TableView scroll-up(追加後、削除後、最短経路、保持経路)
    //
    //
    func scrollTableView() {
        let lastIndex : Int = ds.getRouteCount() - 1;
        if (0 < lastIndex) {
            let idxpath : NSIndexPath = NSIndexPath(forRow: lastIndex, inSection:0)
            tableView.scrollToRowAtIndexPath(idxpath, atScrollPosition: UITableViewScrollPosition.Bottom, animated: true)
        }
    }
    
    func alertMessage(title : String, message : String) {
        if nil != objc_getClass("UIAlertController") {
            // iOS8
            let ac : UIAlertController = UIAlertController(title: self.title!, message: message, preferredStyle: .Alert)
            ac.addAction(UIAlertAction(title: "OK", style: .Default, handler: nil))
            self.presentViewController(ac, animated: true, completion: nil)
        } else {
            // Alart
            let alertView : UIAlertView = UIAlertView(title: title, message: message, delegate: self, cancelButtonTitle: nil, otherButtonTitles: "OK")
            alertView.alertViewStyle = UIAlertViewStyle.Default;
            alertView.show()
        }
    }
}
