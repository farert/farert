//
//  MainTableViewController.swift
//  iFarert Main first look View
//
//  Created by TAKEDA, Noriyuki on 2015/04/03.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit


class MainTableViewController: UITableViewController, UIActionSheetDelegate, TableHeaderViewDelegate, UIAlertViewDelegate, MainTableViewControllerDelegate {
    
    // MARK: - Constant
    let WELCOME_TITLE: String = "ダウンロードありがとうございます"
    let WELCOME_MESSAGE: String = "本アプリで表示される結果は必ずしも正確な情報ではないことがありえます。実際のご旅行での費用とは異なることがありえますことをご理解の上ご利用ください。\n本アプリにより発生したあらゆる損害は作者は負いません\n本アプリ及び提供元は各鉄道会社とは関係もありません"
    // MARK: - property
    let HEADER_HEIGHT:CGFloat        = 44.0
    let FOOTER_HEIGHT:CGFloat        = 51.0
    
    enum ROUTE {
        case OK
        case DUP_ERROR
        case FINISH
        case FARERESULT_ERR1
        case FARERESULT_ERR2
        case FARERESULT_FAIL
        case SCRIPT_STATION_ERR
        case SCRIPT_LINE_ERR
        case SCRIPT_ROUTE_ERR
        case DUPCHG_ERROR
        case AUTO_ROUTE
        case AUTO_NOTENOUGH
        case COMPANYPASS_ERROR
        case UNKNOWN
    }
    
    let TAG_UIACTIONSHEET_AUTOROUTE:Int         = 10
    let TAG_UIACTIONSHEET_QUERYSETUPROUTE:Int   = 11
    let TAG_UIACTIONSHEET_OSAKAKANDETOUR:Int    = 12
    let TAG_UIACTIONSHEET_ROUTEHOLDER:Int       = 13
  
    enum LPROC {
        case UNKNOWN
        case REVERSE
        case AUTOROUTE
        case SETUPROUTE
    }

    // MARK: - Global variables
    
    var viewContextMode: Int = 0;
    var routeStat: ROUTE = .OK
    var scroll_flag: Bool = false;
    var longTermFuncMode: LPROC = .UNKNOWN
    
    // MARK: - UI variables

    @IBOutlet weak var reverseBarButton: UIBarButtonItem!
    @IBOutlet weak var replayBarButton: UIBarButtonItem!
    @IBOutlet weak var organaizerBarButton: UIBarButtonItem!
    @IBOutlet weak var actionBarButton: UIBarButtonItem!
    
    // MARK: Local variables
    
    var ds: cRoute!
    var fareInfo: FareInfo?
    var routeScript: String?
    var frontView: UIView!
    var indicator: UIActivityIndicatorView!
    var dsPre: cRouteList? //左画面から受信、適用前の仮格納庫
    
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
        self.navigationItem.rightBarButtonItem?.isEnabled = true
        self.tableView.rowHeight = 44.0;
        self.actionBarButton.isEnabled = false;
        ds = cRoute()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    //  View will Appear
    // Called when the view is about to made visible. Default does nothing
    //
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)

        self.addLeftBarButtonWithImage(UIImage(named: "ic_menu_black_24dp")!)

        self.slideMenuController()?.removeLeftGestures()
        self.slideMenuController()?.addLeftGestures()

        let lvd = self.slideMenuController()?.leftViewController as! LeftTableViewController
        lvd.delegate = self as MainTableViewControllerDelegate
        //  self.navigationController.toolbarHidden = NO;
    }
    
    
    //  View did apear
    //
    //
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated);
    
        // remove old(before ver 15.10)
        UserDefaults.standard.removeObject(forKey: "HasLaunchedOnce")
        
        let cnt = UserDefaults.standard.integer(forKey: "hasLaunched")
        if 0x1603 <= cnt {
        
            // ２回目以降の起動時
        } else {
            // 初回起動時
            // AlertView(iOS7, iOS8)
            if #available(iOS 8, OSX 10.10, *) {
                /* iOS8 */
                let ac = UIAlertController(title: WELCOME_TITLE, message: WELCOME_MESSAGE, preferredStyle: .alert)
                let otherAction = UIAlertAction(title: "了解", style: .default) {
                    action in
                }
                ac.addAction(otherAction)
                self.present(ac, animated: true, completion: nil)
                
            } else {
                /* iOS7 */
                let av = UIAlertView(title: WELCOME_TITLE, message: WELCOME_MESSAGE, delegate: nil, cancelButtonTitle: "了解")
                av.show()
            }
            UserDefaults.standard.set(0x1603, forKey: "hasLaunched")
            cRouteUtil.save(toDatabaseId: DbId.DB_MAX_ID, sync: false)
            UserDefaults.standard.synchronize();
        }

        
        
        // ここから
        switch viewContextMode {
        case FGD.CONTEXT_AUTOROUTE_ACTION:
            let appDelegate:AppDelegate = UIApplication.shared.delegate as! AppDelegate
            if let selid = appDelegate.selectTerminalId {
                self.actionSheetController(
                    ["新幹線を使う", "新幹線をつかわない（在来線のみ）"],
                    title: cRouteUtil.stationName(selid) + "までの最短経路追加",
                    message: "",
                    from: TAG_UIACTIONSHEET_AUTOROUTE)
            }
    
        case FGD.CONTEXT_ROUTESELECT_VIEW:
            /* from 経路追加 */
            scroll_flag = true
            
        case FGD.CONTEXT_BEGIN_TERMINAL_ACTION:
            // 開始駅選択による経路破棄の確認
            self.actionSheetController(["はい", "いいえ"],
                title: "表示経路を破棄してよろしいですか？",
                message: "",
                from: TAG_UIACTIONSHEET_QUERYSETUPROUTE)
            
        case FGD.CONTEXT_TICKETHOLDER_VIEW:
            // ticker holder : from changeRoute()
            self.actionSheetController(["はい", "いいえ"],
                                       title: "表示経路を破棄してよろしいですか？",
                                       message: "",
                                       from: TAG_UIACTIONSHEET_ROUTEHOLDER)
            // to actionSheetController()
            
        case FGD.CONTEXT_ROUTESETUP_VIEW:
            // from 保持経路ビュー(ArchiveRouteTableView)
            showIndicate();    /* start Activity and Disable UI */
            self.navigationController!.view!.isUserInteractionEnabled = false;
            longTermFuncMode = .SETUPROUTE;
            let time = DispatchTime.now() + Double(Int64(0.1 * Double(NSEC_PER_SEC))) / Double(NSEC_PER_SEC)
            DispatchQueue.main.asyncAfter(deadline: time, execute: {
                //NSThread.detachNewThreadSelector(Selector("processDuringIndicatorAnimating:"), toTarget:self, withObject: self.routeScript)
                self.processDuringIndicatorAnimating(self.routeScript as AnyObject? ?? "" as AnyObject)
            })
        default:
            break
        }
    
        viewContextMode = 0;
        // ここまでは ObjCまでは、viewWillApear にあったが、こっちに写してみた

        // セルの選択を解除
        if let idx : IndexPath = self.tableView.indexPathForSelectedRow {
            self.tableView.deselectRow(at: idx, animated:false)
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
    func processDuringIndicatorAnimating(_ param: AnyObject) {
        var rc: Int = -1;
        let appDelegate:AppDelegate = UIApplication.shared.delegate as! AppDelegate

        switch (longTermFuncMode) {
        case .REVERSE:
            // reverse route
            rc = ds.reverse()
            let cds : cCalcRoute = cCalcRoute(route: ds)
            fareInfo = cds.calcFare()
            if rc < 0 {
                //[self alertMessage:@"経路追加エラー" message:@"経路が重複している等反転できません."];
                routeStat = .DUP_ERROR;
            }
        case .AUTOROUTE:
            // auto route
            // buttonIndex : 0 = 新幹線を使う
            //               1 = 新幹線を使わない
            var bullet : Int
            if let nsid : NSNumber = param as? NSNumber {
                bullet = Int(nsid)
            } else {
                bullet = 0
            }
            //let n = param as? Int
            let saveRoute = cRouteList(route: ds)

            rc = ds.autoRoute(bullet != 1, arrive: appDelegate.selectTerminalId!);
            if (rc < 0) {
                //[self alertMessage:@"経路追加エラー" message:@"経路が重複している等追加できません."];
                routeStat = .AUTO_ROUTE;
                ds.assign(saveRoute)
            } else if (1 == rc) {
                /* Do nothing, success */
            } else {
                if (rc == 4) {    /* 0, 4 or 5 */
                    /* already routed *//* such as 代々木 新大久保 -> 代々木 */
                    ds.assign(saveRoute)
                    routeStat = .AUTO_NOTENOUGH
                } else {
                    routeStat = .FINISH;   /* 経路が終端に達しました */
                }
            }
            if let cds = cCalcRoute(route: ds) {
                fareInfo = cds.calcFare()
            } else {
                fareInfo = FareInfo()
            }
            scroll_flag = true;
            
        case .SETUPROUTE:
            // 保持経路のロード
            let rs : String? = param as? String
            if (nil == rs) || (rs!.isEmpty == true) {
                longTermFuncMode = .UNKNOWN
                hideIndicate()          /* hide Activity and enable UI */
                return
            }
            rc = ds.setupRoute(rs)
            if (rc < 0) {
                //[self alertMessage:@"経路追加エラー" message:@"経路が重複している等追加できません."];
                switch (rc) {
                case -200:
                    routeStat = .SCRIPT_STATION_ERR;
                case -300:
                    routeStat = .SCRIPT_LINE_ERR;
                case -2:
                    routeStat = .SCRIPT_ROUTE_ERR;
                case -4:
                    routeStat = .COMPANYPASS_ERROR
                default:
                    routeStat = .DUP_ERROR;    // -1: 経路重複エラー
                }
            } else { // (0 <= result)
                // 0=end, plus=continue
                if ((rc == 0) && (1 < ds.getCount())) {
                    routeStat = .FINISH;
                } else {
                    routeStat = .OK  // success
                }
            }
            if let cds = cCalcRoute(route: ds) {
                fareInfo = cds.calcFare()
            } else {
                fareInfo = FareInfo()
            }
        default:
            assert(false, "bug:\(#file) : \(#line))")
            break
        }
        fareResultSetting(rc)   /* 簡易結果(Footer section) */
        longTermFuncMode = .UNKNOWN
        hideIndicate()          /* hide Activity and enable UI */
    }

    // MARK: - Table view data source

    override func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        // Return the number of rows in the section.
        var num : Int = ds.getCount()
        
        if (ds.startStationId() <= 0) {
            num = 0
        }
        if (0 < num) {
            //self.navigationItem.rightBarButtonItem.enabled = YES;
            self.replayBarButton.isEnabled = true;
        } else {
            //self.navigationItem.rightBarButtonItem.enabled = NO;
            self.replayBarButton.isEnabled = false;
        }
        if (1 < num) {
            self.reverseBarButton.isEnabled = true;
        } else {
            self.reverseBarButton.isEnabled = false;
        }
        return num;
    }

    //  TableView データ描画・設定
    //
    //
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        if (ds.getCount() - 1) <= indexPath.row {
            /*
            * 最後の行
            */
            let cell = tableView.dequeueReusableCell(withIdentifier: "LastRouteCell") as UITableViewCell!
            let lbl = cell?.viewWithTag(201) as! UILabel

            if (routeStat != .OK) {
                switch (routeStat) {
                case ROUTE.COMPANYPASS_ERROR:
                    lbl.text = "許可されていない会社線通過です";
                    break;
                case ROUTE.DUP_ERROR:
                    lbl.text = "経路が重複しているため追加できません";
                    break;
                case ROUTE.FINISH:
                    lbl.text = "経路が終端に達しました";
                    break;
                case ROUTE.FARERESULT_ERR1:
                    lbl.text = "続けて経路を追加または削除してください.";
                    break;
                case ROUTE.FARERESULT_ERR2:
                    lbl.text = "会社線のみの運賃は表示できません.";
                    break;
                case ROUTE.SCRIPT_STATION_ERR:
                    lbl.text = "不正な駅名が含まれています.";
                    break;
                case ROUTE.SCRIPT_LINE_ERR:
                    lbl.text = "不正な路線名が含まれています.";
                    break;
                case ROUTE.SCRIPT_ROUTE_ERR:
                    lbl.text = "経路不正";
                    break;
                case ROUTE.DUPCHG_ERROR:
                    lbl.text = "経路が重複していますので変更できません";
                    break;
                case ROUTE.AUTO_ROUTE:
                    lbl.text = "自動経路算出不可(条件を変えて試してみてください)"
                case ROUTE.AUTO_NOTENOUGH:
                    lbl.text = "開始駅へ戻るにはもう少し経路を指定してからにしてください"
                default:
                    lbl.text = "処理に失敗しました(サポートへ連絡してください)";
                    break;
                }
                lbl.isHidden = false;
            } else {
                lbl.isHidden = true;
            }
            return cell!
        } else {
            /* 最後の行以外の普通の行
            *
            */
            let cell = tableView.dequeueReusableCell(withIdentifier: "RouteContentCell") as! FarertTableViewCell
            
            let ri = ds.getItem(indexPath.row + 1)
            cell.lineName.text = ri?.line;
            cell.stationName.text = ri?.station;

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
    override func tableView(_ tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        let tableHeaderViewCell : HeaderTableViewCell = tableView.dequeueReusableCell(withIdentifier: "HeaderCell") as! HeaderTableViewCell
        var label : UILabel?
        var title : String
        var subtitle : String

        
        if 0 < ds.startStationId() {
            /* 開始駅 */
            title = cRouteUtil.stationNameEx(ds.startStationId())
            /* ひらかな */
            subtitle = cRouteUtil.getKanaFromStationId(ds.startStationId())
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
    override func tableView(_ tableView: UITableView, viewForFooterInSection section: Int) -> UIView? {
        
        let cell : HeaderTableViewCell = tableView.dequeueReusableCell(withIdentifier: "footerCell") as! HeaderTableViewCell
        let lbl1 : UILabel = cell.viewWithTag(40001) as! UILabel
        let lbl2 : UILabel = cell.viewWithTag(40002) as! UILabel
        let lbl3 : UILabel = cell.viewWithTag(40003) as! UILabel

        if (ds.getCount() <= 0) || (fareInfo == nil) || (fareInfo!.result != 0) {
            lbl1.text = ""
            lbl2.text = ""
            lbl3.text = ""
        } else {
            lbl1.text = cRouteUtil.fareNumStr(fareInfo!.fare) + "円"
            lbl2.text = cRouteUtil.kmNumStr(fareInfo!.totalSalesKm) + "km"
            lbl3.text = "\(fareInfo!.ticketAvailDays)日間"
        }
        cell.delegate = self;   // enable touch
        
        return cell;
    }
    
    //  ヘッダ高さ
    //
    //
    override func tableView(_ tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {

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
    override func tableView(_ tableView: UITableView, heightForFooterInSection section: Int) -> CGFloat {

        if section == 0 {
            if (1 < ds.getCount()) {
                return FOOTER_HEIGHT
            }
        }
        return UITableViewAutomaticDimension
    }
    
    //  行の高さ
    //
    //
    override func tableView(_ tableView: UITableView, estimatedHeightForRowAt indexPath : IndexPath) -> CGFloat {
     
        return UITableViewAutomaticDimension
    }
    
    // MARK: - delegate

    //  Touched on tableView Header / Footer
    //
    //
    func tableHeaderViewTouched(_ tableHeaderViewCell : HeaderTableViewCell) {
        if (tableHeaderViewCell.tag == 100) {
            /* header */
            self.performSegue(withIdentifier: "terminalSelectSegue", sender:self);
            
        } else if (tableHeaderViewCell.tag == 200) {
            /* Footer */
            let lastIndex : Int = ds.getCount() - 2; // last item
            if (0 <= lastIndex) {
                scroll_flag = true; // Tableview scroll-up
                let indexPath : IndexPath = IndexPath(row: lastIndex, section: 0)
                self.tableView.selectRow(at: indexPath, animated: false, scrollPosition: UITableViewScrollPosition.none)
                self.performSegue(withIdentifier: "fareInfoDetailSegue", sender:self)
            }
        }
    }

    // delegate function of LeftTableViewController add button
    func getRoute() -> cRouteList? {
        if 2 <= ds.getCount() {
            if let r : cRouteList = cRouteList(route: self.ds) {
                return r
            }
        }
        return nil
    }
    
    // delegate from LeftView
    
    // Selected at leftView row
    //  from tableView:didSelectRowAt in LeftTableView
    //
    func changeRoute(route: cRouteList) {
        let curscr = ds.routeScript()
        let newscr = route.routeScript()
        if (curscr != newscr) {
            if (ds.getCount() <= 1) || cRouteUtil.isRoute(inStrage: newscr) {
                // すぐやる
                setRouteList(routeList: route)
                dsPre = nil
                viewContextMode = FGD.CONTEXT_ROUTESELECT_VIEW
            } else {
                // 聞いてからやる
                dsPre = route
                viewContextMode = FGD.CONTEXT_TICKETHOLDER_VIEW
            }
        }
        // to: viewDidAppear()
    }

    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    // each view before dispatch
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        let segid : String = segue.identifier!
        let apd:AppDelegate = UIApplication.shared.delegate as! AppDelegate

        viewContextMode = 0

        if segid == "terminalSelectSegue" {
            apd.context = FGD.CONTEXT_TERMINAL_VIEW;
            //        vc.delegate = self;
        } else if segid == "routeLineSegue" {
            /* 経路追加(駅に属する路線一覧 */
            let selectLineTblViewController : SelectLineTableViewController = segue.destination as! SelectLineTableViewController;
            selectLineTblViewController.baseStationId = ds.lastStationId()
            selectLineTblViewController.companyOrPrefectId = 0
            selectLineTblViewController.lastLineId = ds.lastLineId()
            
            apd.context = FGD.CONTEXT_ROUTESELECT_VIEW;
            
        } else if segid == "fareInfoDetailSegue" {
            /* 運賃詳細ビュー */
            let idx = self.tableView.indexPathForSelectedRow?.row ?? 0
            let resultViewCtlr : ResultTableViewController = segue.destination as! ResultTableViewController
            resultViewCtlr.ds = cCalcRoute(route: self.ds, count: idx + 2)
            apd.context = 0;

        } else if segid == "routeManagerSegue" {
            /* 経路一覧ビュー */
            let naviCtlr : UINavigationController = segue.destination as! UINavigationController
            let routeMngViewCtlr : ArchiveRouteTableViewController = naviCtlr.topViewController as! ArchiveRouteTableViewController
            if ds.getCount() <= 1 {
                routeMngViewCtlr.currentRouteString = "";
            } else {
                routeMngViewCtlr.currentRouteString = ds.routeScript()
            }
            apd.context = 0;
        } else if segid == "versionInfoTransitSegue" {
            // バージョン情報
        } else if segid == "settingsSegue" {
            // 設定(データベース切り替え)
            /// // save Route
            self.routeScript = ds.routeScript().trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)
        } else {
            assert(false, "bug:\(#file) \(#line) segid=\(segid)")
        }
        //NSLog(@"prepareForSegue:%@", [segue identifier]);
    }

    // MARK: - unwind segue
    
    @IBAction func doneTerminal(_ segue: UIStoryboardSegue)
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
            let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate
            //termId = [[app selectTerminalId] intValue];
            termId = apd.selectTerminalId ?? 0
            if (apd.context == FGD.CONTEXT_TERMINAL_VIEW) {
                // from 発駅
                //NSLog(@"begin station=%d, %@", termId, [cRouteUtil StationName:termId]);
                //if ([_ds startStationId] == termId) {
                //    return;
                //}
                if (1 < ds.getCount()) && !cRouteUtil.isRoute(inStrage: ds.routeScript()) {
                    viewContextMode = FGD.CONTEXT_BEGIN_TERMINAL_ACTION;
                    // つつきは、viewDidApear:　で
                } else {
                    self.setBeginTerminal()
                }
            } else if (apd.context == FGD.CONTEXT_AUTOROUTE_VIEW) {
                // from 最短経路
                // auto route
                //NSLog(@"autoroute end station=%d, %@", termId, [cRouteUtil StationName:termId]);
                _ = self.navigationController?.popViewController(animated: false)

                // AlartViewで、新幹線を使うか否かを訊いてautoroute
                //BOOL bulletUse = [self queryDialog:@"新幹線を使用しますか?"];
                viewContextMode = FGD.CONTEXT_AUTOROUTE_ACTION;
                cRouteUtil.save(toTerminalHistory: cRouteUtil.stationNameEx(termId))
                // つつきは、viewWilApear:　で
                
            } else if (apd.context == FGD.CONTEXT_ROUTESELECT_VIEW) {
                // from 経路追加
                /* add route */
                let result : Int = ds.add(apd.selectLineId, stationId: apd.selectTerminalId)
                if let cds = cCalcRoute(route: ds) {
                    fareInfo = cds.calcFare()
                } else {
                    fareInfo = FareInfo()
                }
                self.fareResultSetting(result)
                if result == -4 {
                    routeStat = ROUTE.COMPANYPASS_ERROR
                } else if result == -2 {
                    routeStat = ROUTE.SCRIPT_STATION_ERR;
                } else if (result == 0) || (result == 5) || (result == 4) {
                    routeStat = ROUTE.FINISH
                } else if (result == 1) {
                    routeStat = .OK
                } else if (result == -1) {
                    routeStat = ROUTE.DUP_ERROR
                } else {
                    assert(false, "bug!!!!!!")
                    routeStat = .UNKNOWN
                }
                viewContextMode = FGD.CONTEXT_ROUTESELECT_VIEW;
                self.tableView.reloadData()
            } else {
                assert(false, "bug");
            }
        } else if (seg_id == "unwindArchiveRouteSelectSegue") {
            // from 保持経路一覧(ArchiveRouteTableViewTabel)
            // setup route
            let routeViewCtrl : ArchiveRouteTableViewController = segue.source as! ArchiveRouteTableViewController
            if !routeViewCtrl.selectRouteString.isEmpty {
                viewContextMode = FGD.CONTEXT_ROUTESETUP_VIEW;
                self.routeScript = routeViewCtrl.selectRouteString;
            }
        } else {
            //NSAssert(nil, @"bug");
            print("Bug?? \(segue.identifier ?? "(null)")")
        }
    }

    // exit(unwind) segue
    //  from: ArchiveRouteTableViewController - [Close] button
    //        TerminalHistoryTableViewController - [Close] button
    //         TermSelectTableViewController - [Close] button
    //
    @IBAction func cancelTerminal(_ segue: UIStoryboardSegue) {
        _ = self.navigationController?.popViewController(animated: false)

        viewContextMode = 0
        print("canelTerminal segue=\(segue.identifier ?? "(null)")")
    }
    
    // exit(unwind) segue
    //  from: VersionInfoViewController
    //
    @IBAction func closeModal(_ segue: UIStoryboardSegue) {
        print("MainView: closeModal:")
        viewContextMode = 0
        
        if segue.identifier == "settingsSegue" {
            let view : SettingsTableViewController = segue.source as! SettingsTableViewController
            if (0 < view.selectDbId) {  // is change DB
                viewContextMode = FGD.CONTEXT_ROUTESETUP_VIEW;
            }
            
            /* settingsのあとでは「while a presentation or dismiss is in progress!」警告が表示される */
        } else if (segue.identifier == "versionInfoExitSegue") {
            //if self.navigationController?.isBeingDismissed() != true {
                self.dismiss(animated: true, completion: nil)
            //}
        } else {
            assert(false)
        }
    }

    // MARK: - Activity Indicator
    
    func showIndicate() {
        self.frontView = UIView(frame: self.navigationController!.view.bounds)
        self.frontView.backgroundColor = UIColor.clear
        self.navigationController!.view!.addSubview(self.frontView)
    
        self.indicator = UIActivityIndicatorView(activityIndicatorStyle: UIActivityIndicatorViewStyle.whiteLarge)
        self.indicator.color = UIColor.black
        self.indicator.center = self.frontView.center
        self.frontView.addSubview(self.indicator)
        self.frontView.bringSubview(toFront: self.indicator)
        self.indicator.startAnimating()
        
    }
    
    func hideIndicate() {
        self.indicator.stopAnimating()
        self.indicator.removeFromSuperview()
        self.frontView.removeFromSuperview()
        self.indicator = nil;
        self.frontView = nil;
        
        self.navigationController?.view.isUserInteractionEnabled = true;
        self.tableView.reloadData()
    }

    // 逆転
    @IBAction func reverseAction(_ sender: UIBarButtonItem) {
        if (ds.getCount() < 2) {
            return;
        }
        
        self.showIndicate()    /* start Activity and Disable UI */
        self.navigationController?.view?.isUserInteractionEnabled = false;
        longTermFuncMode = .REVERSE
        // [self performSelector:@selector(processDuringIndicatorAnimating:) withObject:nil afterDelay:0.1];
        let time = DispatchTime.now() + Double(Int64(0.1 * Double(NSEC_PER_SEC))) / Double(NSEC_PER_SEC)
        DispatchQueue.main.asyncAfter(deadline: time, execute: {
            //NSThread.detachNewThreadSelector(Selector("processDuringIndicatorAnimating:"), toTarget:self, withObject: nil)
            self.processDuringIndicatorAnimating(NSNull.self)
        })
    }
    
    // バック（1つ前に戻る)
    @IBAction func backAction(_ sender: UIBarButtonItem) {
        if (1 < ds.getCount()) {
            ds.removeTail()
            if let cds = cCalcRoute(route: ds) {
                fareInfo = cds.calcFare()
            } else {
                fareInfo = FareInfo()
            }
            self.fareResultSetting(1)
        } else {
            ds.removeAll()    // removeAll, clear start
        }
        self.tableView.reloadData()
        self.scrollTableView()
    }
    
    // action menu
    @IBAction func checkAction(_ sender: UIBarButtonItem) {
 
        if let cds = cCalcRoute(route: ds) {
            if cds.isOsakakanDetourEnable() {
                self.actionSheetController(cds.isOsakakanDetourShortcut() ?
                    ["大阪環状線 近回り"] : ["大阪環状線 遠回り"],
                                           title: "大阪環状線経由指定", message: "",
                                           from: TAG_UIACTIONSHEET_OSAKAKANDETOUR)
            }
        }
    }
    
    
    // begin Action select menu.
    func actionSheetController(_ menu_list : [String], title : String, message : String, from : Int) {
        
        if #available(iOS 8, OSX 10.10, *) {
            // iOS8
            let ac : UIAlertController = UIAlertController(title: title, message: message, preferredStyle: .actionSheet)
            for item : String in menu_list {
                ac.addAction(UIAlertAction(title: item, style: .default, handler: { (action: UIAlertAction) in self.actionSelectProcFrom(from, label: item)}))
            }
            if nil == menu_list.last!.range(of: "いいえ") {
                ac.addAction(UIAlertAction(title: "キャンセル", style: .cancel, handler: {(action: UIAlertAction) in self.actionSelectProcFrom(from, label: "キャンセル")}))
            }
            // for iPad
            ac.modalPresentationStyle = UIModalPresentationStyle.popover
            switch (from) {
            case TAG_UIACTIONSHEET_OSAKAKANDETOUR:
                ac.popoverPresentationController?.barButtonItem = self.actionBarButton
            case TAG_UIACTIONSHEET_AUTOROUTE:
                ac.popoverPresentationController?.sourceView = self.view
                ac.popoverPresentationController?.sourceRect = self.tableView.rectForFooter(inSection: 0)
            case TAG_UIACTIONSHEET_QUERYSETUPROUTE, TAG_UIACTIONSHEET_ROUTEHOLDER :
                ac.popoverPresentationController?.sourceView = self.view
                ac.popoverPresentationController?.sourceRect = self.tableView.rectForHeader(inSection: 0)
            default:
                break
            }
            // end of for iPad
            self.present(ac, animated: true, completion: nil)
            
        } else {
            // iOS7
            let actsheet : UIActionSheet = UIActionSheet()
            
            actsheet.delegate = self
            actsheet.title = title
            
            for item : String in menu_list {
                actsheet.addButton(withTitle: item)
            }
            if nil == menu_list.last!.range(of: "いいえ") {
                actsheet.addButton(withTitle: "キャンセル")
                actsheet.cancelButtonIndex = actsheet.numberOfButtons - 1
            }
            actsheet.tag = from
            
            if UIDevice.current.userInterfaceIdiom == .pad {
                self.clearsSelectionOnViewWillAppear = false
                self.preferredContentSize = CGSize(width: self.navigationController!.view!.frame.width/2, height: self.view!.frame.height)
                switch (from) {
                case TAG_UIACTIONSHEET_OSAKAKANDETOUR:
                    actsheet.show(from: self.actionBarButton, animated: true)
                case TAG_UIACTIONSHEET_AUTOROUTE:
                    actsheet.show(from: self.tableView.rectForRow(at: self.tableView.indexPathForSelectedRow!), in: self.view, animated: true)
                case TAG_UIACTIONSHEET_QUERYSETUPROUTE:
                    actsheet.show(from: self.tableView.rectForHeader(inSection: 0), in: self.view, animated: true)
                default:
                    assert(false)
                    break
                }
            } else {
                let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate
                let win : UIWindow = apd.window!
                
                if (win.subviews ).contains(self.tableView as UIView) {
                    actsheet.show(in: self.view)
                } else {
                    actsheet.show(in: win)
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
    func actionSheet(_ actionSheet : UIActionSheet, clickedButtonAt buttonIndex : Int) {
        if buttonIndex < 0 {
            actionSelectProcFrom(actionSheet.tag, label: "キャンセル")
        } else {
            // <swift1> actionSelectProcFrom(actionSheet.tag, label: actionSheet.buttonTitleAtIndex(buttonIndex))
            actionSelectProcFrom(actionSheet.tag, label: (actionSheet.buttonTitle(at: buttonIndex))!)
        }
    }
    
    func actionSelectProcFrom(_ from : Int, label title : String) {

        let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate
        var rc : Int
        
        viewContextMode = 0
        
        switch from {
        case TAG_UIACTIONSHEET_AUTOROUTE:
            if (apd.context != FGD.CONTEXT_AUTOROUTE_VIEW) {
                return;
            }
            if (nil != title.range(of: "キャンセル")) {
                return;
            }
            /*  最短経路 doneTerminal: -> willApear: ->
            *
            */
            self.showIndicate()    /* start Activity and Disable UI */
            self.navigationController?.view?.isUserInteractionEnabled = false;
            longTermFuncMode = .AUTOROUTE;
            //[self performSelector:@selector(processDuringIndicatorAnimating:)
            //withObject:[NSNumber numberWithInteger:buttonIndex]
            //afterDelay:0.1];
            let selectIndex : Int = (nil != title.range(of: "使う")) ? 0 : 1
            let time = DispatchTime.now() + Double(Int64(0.1 * Double(NSEC_PER_SEC))) / Double(NSEC_PER_SEC)
            DispatchQueue.main.asyncAfter(deadline: time, execute: {
                //NSThread.detachNewThreadSelector(Selector("processDuringIndicatorAnimating:"), toTarget:self, withObject: selectIndex)
                self.processDuringIndicatorAnimating(selectIndex as AnyObject)
            })
            
        case TAG_UIACTIONSHEET_QUERYSETUPROUTE:
            /*  経路表示時の１行目の発駅変更(既存経路破棄→新規発駅設定)
            * doneTerminal: -> willApear
            */
//            if (selectIndex == 0) {
            if nil != title.range(of: "はい") {
                self.setBeginTerminal()
                self.tableView.reloadData()
            }
            
        case TAG_UIACTIONSHEET_OSAKAKANDETOUR:
            /*  大阪環状線 遠回り／近回り
            */
            if nil == title.range(of: "キャンセル") {
                if (nil != title.range(of: "大阪環状線")) {
                    rc = ds.setDetour(nil != title.range(of: "遠") ? true : false)
                    if (rc < 0) {
                        routeStat = ROUTE.DUPCHG_ERROR;
                    } else if (rc == 0) || (rc == 5) {
                        routeStat = ROUTE.FINISH;
                    } else {
                        routeStat = .OK  /* success */
                    }
                    if let cds = cCalcRoute(route: ds) {
                        fareInfo = cds.calcFare()
                    } else {
                        fareInfo = FareInfo()
                    }
                    self.tableView.reloadData()
                }
            }
        case TAG_UIACTIONSHEET_ROUTEHOLDER:
            /* きっぷホルダ 経路選択 from changeRoute, viewDidApear, actionSheetController */
            if nil != title.range(of: "はい") {
                self.setRouteList(routeList: nil)
            }
        default:
            // don't come here
            break
        }
    }

    // MARK: - local
    
    func fareResultSetting(_ rc: Int) {

        if (1 == rc) {
            switch (fareInfo?.result ?? 0) {
            case 0:
                routeStat = .OK
            case 1:
                routeStat = ROUTE.FARERESULT_ERR1;
            case 2:
                routeStat = ROUTE.FARERESULT_ERR2;
            default:
                routeStat = ROUTE.FARERESULT_FAIL;
            }
        }
        
        if let cds = cCalcRoute(route: ds) {
            if cds.isOsakakanDetourEnable() {
                self.actionBarButton.isEnabled = true
            } else {
                self.actionBarButton.isEnabled = false
            }
        } else {
            self.actionBarButton.isEnabled = false
        }
    }
    
    //  発駅設定・表示（ヘッダタップ、最短経路
    //
    //
    func setBeginTerminal() {
        let appDelegate:AppDelegate = UIApplication.shared.delegate as! AppDelegate
        
        let termId : Int = appDelegate.selectTerminalId
        ds.add(termId)
        routeStat = .OK
        tableView.reloadData()
        self.actionBarButton.isEnabled = false;
        cRouteUtil.save(toTerminalHistory: cRouteUtil.stationNameEx(termId))
    }
    
    //  経路設定
    //
    func setRouteList(routeList : cRouteList?) {
        if nil == routeList {
            self.ds = cRoute(routeList: self.dsPre)
        } else {
            self.ds = cRoute(routeList: routeList!)
        }
        routeStat = .OK
        if let cds = cCalcRoute(route: ds) {
            fareInfo = cds.calcFare()
        } else {
            fareInfo = FareInfo()
        }
        tableView.reloadData()
    }
    
    
    //  TableView scroll-up(追加後、削除後、最短経路、保持経路)
    //
    //
    func scrollTableView() {
        let lastIndex : Int = ds.getCount() - 1;
        if (0 < lastIndex) {
            let idxpath : IndexPath = IndexPath(row: lastIndex, section:0)
            tableView.scrollToRow(at: idxpath, at: UITableViewScrollPosition.bottom, animated: true)
        }
    }
    
    
    // Message view
    //
    func alertMessage(_ title : String, message : String) {
        if #available(iOS 8, OSX 10.10, *) {
            // iOS8
            let ac : UIAlertController = UIAlertController(title: self.title!, message: message, preferredStyle: .alert)
            ac.addAction(UIAlertAction(title: "OK", style: .default, handler: nil))
            self.present(ac, animated: true, completion: nil)
        } else {
            // Alart
            let alertView : UIAlertView = UIAlertView(title: title, message: message, delegate: self, cancelButtonTitle: nil, otherButtonTitles: "OK")
            alertView.alertViewStyle = UIAlertViewStyle.default;
            alertView.show()
        }
    }
}
