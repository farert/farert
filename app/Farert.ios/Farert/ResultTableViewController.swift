//
//  ResultTableViewController.swift
//  iFarert Result show secound right side view.
//
//  Created by TAKEDA, Noriyuki on 2015/03/23.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit
import MessageUI



class ResultTableViewController: UITableViewController, UIActionSheetDelegate, UIDocumentInteractionControllerDelegate, UIAlertViewDelegate {

    // MARK: Constant
    
    // MARK: Public property
    var ds : cCalcRoute = cCalcRoute()

    // MARK: Local property
    
    var contentsForKm : [[String : String]] = []
    var contentsForFare : [[String : String]] = []

    var contentsForMessage : [String] = []

    var fareInfo : FareInfo!
    
    // MARK: UI property
    
    @IBOutlet weak var actionButton: UIBarButtonItem!
    @IBOutlet weak var chgOptionButton: UIBarButtonItem!

    var frontView : UIView!
    var indicator : UIActivityIndicatorView!
    
    var docInterCon : UIDocumentInteractionController!

    // MARK: View method
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem()
        //[self.tableView registerClass:[UITableViewCell class] forCellReuseIdentifier:@"Cell"];
        
        NotificationCenter.default.addObserver(self, selector: Selector(("preferredContentSizeChanged")), name: UIContentSizeCategory.didChangeNotification, object: nil)
        
        self.tableView.estimatedRowHeight = 100.0
        if 8.0 <= ((UIDevice.current.systemVersion as NSString).floatValue) {
            self.tableView.rowHeight = UITableView.automaticDimension
        }
        //NSLog(@"ResultView didLoad entry");
        // Uncomment the following line to preserve selection between presentations.
        // self.clearsSelectionOnViewWillAppear = NO;
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem;
        
        self.navigationController?.isToolbarHidden = false
        
        // Inner state variables
        ///self.specificAppliedBarButton.enabled = YES;
        
        self.reCalcFareInfo(true)
        
        self.tableView.sectionHeaderHeight = 40.0;
        self.tableView.sectionFooterHeight = 0.01;

    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    // 戻ってきたときにセルの選択を解除
    override func viewWillAppear(_ animated : Bool) {
        super.viewWillAppear(animated)
        if let idx : IndexPath = self.tableView.indexPathForSelectedRow {
            self.tableView.deselectRow(at: idx, animated:false)
        }
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        self.tableView.reloadData()
    }

    // MARK: - Table view data source

    override func numberOfSections(in tableView: UITableView) -> Int {
        if self.fareInfo.result == 0 {
            return 6;
        } else {
            return 1;
        }
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        
        if (nil == self.fareInfo) || (self.fareInfo.result != 0) {
            return 1;   // Error message
        }
        switch (section) {
        case 0:
            return 1; /* section */
        case 1:
            return contentsForKm.count; /* sales, calc km */
        case 2:
            return contentsForFare.count; /* Fare */
        case 3:
            return self.contentsForMessage.count
        case 4:
            return 1;
        case 5:            /* route list */
            if (self.fareInfo.routeListForTOICA.isEmpty) {
                return 1;
            } else {
                return 2;
            }
        default:
            break;
        }
        return 0;
    }


    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        let dic_tags = ["title", "value1", "subtitle", "value2"]
        var cell : UITableViewCell
        
        if (nil == self.fareInfo) || (self.fareInfo.result != 0) {    // error
            cell = tableView.dequeueReusableCell(withIdentifier: "rsRouteListCell", for: indexPath) 
            //lbl = (UILabel*)[cell viewWithTag:1];
            //lbl.text = [self.fareInfo routeList];
            cell.textLabel?.font = UIFont.preferredFont(forTextStyle: UIFont.TextStyle.body)
            cell.textLabel?.numberOfLines = 0;
            
            if (nil == self.fareInfo) {
                cell.textLabel?.text = "経路が空"
            } else {
                switch (self.fareInfo.result) {
                case 1:
                    cell.textLabel?.text = "経路が不完全です.続けて指定してください"
                case 2:
                    cell.textLabel?.text = "会社線のみ運賃は表示できません"
                default:
                    cell.textLabel?.text = "運賃は表示できません"
                    break;
                }
            }
            cell.textLabel?.font = UIFont.preferredFont(forTextStyle: UIFont.TextStyle.body)
            cell.textLabel?.numberOfLines = 0;
            return cell;   // Error message
        }
        
        switch indexPath.section {
        case 0:
            /* section */
            cell = tableView.dequeueReusableCell(withIdentifier: "rsTitleCell", for: indexPath) 

            let lbl : UILabel = cell.viewWithTag(1) as! UILabel
            lbl.text = "\(cRouteUtil.terminalName(self.fareInfo.beginStationId)!) → \(cRouteUtil.terminalName(self.fareInfo.endStationId)!)"
            
        case 1:
            /* KM */
            let dic : [String : String] = self.contentsForKm[indexPath.row]
            if let celname = dic["cell"] {
                cell = tableView.dequeueReusableCell(withIdentifier: celname, for: indexPath)
                for i in 1..<5 {
                    if let lbl : UILabel = cell.viewWithTag(i) as! UILabel? {
                        lbl.text = dic[dic_tags[i - 1]] ?? ""
                    }
                }
            } else {
                cell = tableView.dequeueReusableCell(withIdentifier: "rsKmCell2x", for: indexPath)
                for i in 1..<5 {
                    if let lbl = cell.viewWithTag(i) as! UILabel? {
                        lbl.text = ""
                    }
                }
            }
            
        case 2:
            /* FARE */
            let dic : [String : String] = self.contentsForFare[indexPath.row]
            if let celname = dic["cell"] {
                cell = tableView.dequeueReusableCell(withIdentifier: celname, for: indexPath)
                for i in 1..<5 {
                    if let lbl : UILabel = cell.viewWithTag(i) as! UILabel? {
                        lbl.text = dic[dic_tags[i - 1]] ?? ""
                    }
                }
            } else {
                cell = tableView.dequeueReusableCell(withIdentifier: "rsPersonDiscountFareCell", for: indexPath)
                for i in 1..<5 {
                    if let lbl = cell.viewWithTag(i) as! UILabel? {
                        lbl.text = ""
                    }
                }
            }

        case 3:
            let message : String = self.contentsForMessage[indexPath.row]
            cell = tableView.dequeueReusableCell(withIdentifier: "rsMetroAvailDaysCell", for: indexPath) 
            let lbl : UILabel = cell.viewWithTag(1) as! UILabel
            lbl.text = message
            
        case 4:
            /* avail days */
            cell = tableView.dequeueReusableCell(withIdentifier: "rsAvailDaysCell", for: indexPath)
            var lbl : UILabel = cell.viewWithTag(1) as! UILabel
            lbl.text = "\(self.fareInfo.ticketAvailDays)日間"
            lbl = cell.viewWithTag(2) as! UILabel
            if 1 < self.fareInfo.ticketAvailDays {
                var str : String
                if (self.fareInfo.isBeginInCity && self.fareInfo.isEndInCity) {
                    str = "(発着駅都区市内の駅を除き"
                } else if (self.fareInfo.isBeginInCity) {
                    str = "(発駅都区市内の駅を除き"
                } else if (self.fareInfo.isEndInCity) {
                    str = "(着駅都区市内の駅を除き"
                } else {
                    str = "("
                }
                lbl.text = str + "途中下車可能)"
            } else {
                lbl.text = "(途中下車前途無効)"
            }
            /* isUrban
                // 近郊区間内ですので最短経路の運賃で利用可能です(途中下車不可、有効日数当日限り)
                cell = tableView.dequeueReusableCell(withIdentifier: "rsMetroAvailDaysCell", for: indexPath)
            */
            
        case 5:
            /* ROUTE */
            let rcell : RouteListTableViewCell = tableView.dequeueReusableCell(withIdentifier: "rsRouteListCell", for: indexPath) as! RouteListTableViewCell
            if (indexPath.row == 0) {
                rcell.routeString.text = self.fareInfo.routeList!
            } else if (indexPath.row == 1) {
                rcell.routeString.text = "＜IC運賃計算経路＞\n\(self.fareInfo.routeListForTOICA!)"
            }
            cell = rcell
        default:
            cell = tableView.dequeueReusableCell(withIdentifier: "rsRouteListCell", for: indexPath) 
            // provisional
            break;
        }
        //println("\(indexPath.section), \(indexPath.row), \(cell.contentView.frame.height), \(cell.frame.size.height), \(cell.bounds.size.height)");
        return cell
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

    override func tableView(_ tableView : UITableView, titleForHeaderInSection section : Int) -> String? {
        
        if (self.fareInfo?.result != 0) {
            let title : String = "無効な経路"   // Error message
            self.navigationController?.title = title
            return title
        }
 
        self.navigationItem.title = "\(cRouteUtil.terminalName(self.fareInfo.endStationId)!)"
        self.navigationItem.prompt = "\(cRouteUtil.terminalName(self.fareInfo.beginStationId)!) → "

        switch (section) {
        case 0:
            return "区間" /* section */
        case 1:
            return "キロ程" /* sales, calc km */
        case 2:
            return "運賃" /* Fare */
        case 3:
            return ""
        case 4:
            return "有効日数"
        case 5:
            return "経由" /* route list */
        default:
            break
        }
        return nil
    }

    //  行の高さ
    //
    override func tableView(_ tableView : UITableView, heightForRowAt indexPath : IndexPath) -> CGFloat {

        var value : CGFloat
        
        if self.fareInfo == nil {
            return super.tableView(tableView, heightForRowAt: indexPath)
        }
        
        switch indexPath.section {
        case 0:
            /* section */
            // "rsTitleCell"
            value = 51

        case 1:
            /* KM */
            
            switch contentsForKm[indexPath.row]["cell"] {
            case "rsKmCell1":
                value = 44
            case "rsKmCell2":
                value = 21
            case "rsKmCell2x":
                value = 18
            case "rsKmCell3":
                value = 34
            default:
                value = 10
                break
            }

        case 2:
            /* FARE */
            switch contentsForFare[indexPath.row]["cell"] {
            case "rsFareCell":
                value = 44
            case "rsFareCell2":
                value = 36
            case "rsDiscountFareCell":
                value = 24
            case "rsPersonDiscountFareCell":
                value = 22
            default:
                value = 22
                break
            }
        case 3:
            // "rsMetroAvailDaysCell"
            value = 44
        case 4:
            /* avail days */
            value = 44

        case 5:
            /* ROUTE */
            let cell : RouteListTableViewCell = tableView.dequeueReusableCell(withIdentifier: "rsRouteListCell"/*, forIndexPath: indexPath  EXC_BAD_ACCESSになる*/) as! RouteListTableViewCell
            return cell.heightForTitle(self.fareInfo.routeList)

        default:
            value = UITableView.automaticDimension
            break;
        }
        
        if 0 < value {
            return value
        }
        return super.tableView(tableView, heightForRowAt: indexPath)
    }

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
    }
    */
    
    
    // callback for NSNotificationCenter
    func preferredContentSizeChanged(_ aNotification: Notification) {
        // refresh tableView
        self.tableView.reloadData()
    }
    
    // MARK: - Action

    // Action menu selected
    
    func actionSelectProc(_ title: String) {
        if nil != title.range(of: "特例") {
            if nil != title.range(of: "しない") {
                // @"特例を適用しない";
                ds.setNoRule(true);
            } else {
                // @"特例を適用する";
                ds.setNoRule(false);
            }
            self.reCalcFareInfo()
            self.tableView.reloadData()

        } else if nil != title.range(of: "を単駅指定") {
            if nil != title.range(of: "発駅") {
                // "発駅を単駅指定";
                ds.setArriveAsCity();
            } else {
                // "着駅を単駅指定";
                ds.setStartAsCity();
            }
            self.reCalcFareInfo()
            self.tableView.reloadData()
            
//        } else if nil != title.range(of: "最短経路") {
//            let begin_id : Int = ds.startStationId()
//            let end_id : Int = ds.lastStationId()
//            if begin_id == end_id {
//                self.ShowAlertView("確認", message: "開始駅=終了駅では最短経路は算出しません.")
//                return
//            }
//
//            self.showIndicate() /* wait active indicator */
//            self.navigationController?.view.isUserInteractionEnabled = false
//           DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + Double(Int64(0.1 * Double(NSEC_PER_SEC))) / Double(NSEC_PER_SEC), execute: {
 //               //NSThread.detachNewThreadSelector(Selector("processDuringIndicatorAnimating:"), toTarget:self, withObject: nil)
//                self.processDuringIndicatorAnimating(NSNull.self)
//            })
//
        } else if nil != title.range(of: "回り") {
            if let route : cRoute = cRoute() {
                route.sync(ds)
                let rc = route.setDetour((nil != title.range(of: "遠")) ? true : false)
                if 0 <= rc {
                    ds.sync(route)
                    self.reCalcFareInfo()
                    self.tableView.reloadData()
                } else {
                    self.ShowAlertView("エラー", message: "経路が重複するため指定できません")
                }
            }
        } else if nil != title.range(of: "株主優待") {
            if nil != title.range(of: "しない") {
                // @"JR東海株主優待券を適用しない";
                ds.setJrTokaiStockApply(false);
            } else {
                // @"JR東海株主優待券を使用する";
                ds.setJrTokaiStockApply(true);
            }
            self.reCalcFareInfo()
            self.tableView.reloadData()
        } else if nil != title.range(of: "指定した経路") {
            ds.setLong(true);
            self.reCalcFareInfo()
            self.tableView.reloadData()
        } else if nil != title.range(of: "最安経路") {
            ds.setLong(false);
            self.reCalcFareInfo()
            self.tableView.reloadData()
        } else if nil != title.range(of: "規程115条") {
            if nil != title.range(of: "単駅最安") {
                ds.setSpecificTermRule115(false)
            } else /* if nil != title.range(of: "特定都区市内発着") */ {
                ds.setSpecificTermRule115(true)
            }
            self.reCalcFareInfo()
            self.tableView.reloadData()
        } else {
            // cancel
        }
    }

    //  Compose button
    //
    @IBAction func chgOptionButtonAction(_ sender: AnyObject) {

        if self.fareInfo == nil {
            return
        }

        var items : [String] = Array<String>()
        
        if self.fareInfo.isRuleAppliedEnable {
            if self.fareInfo.isRuleApplied {
                items.append("特例を適用しない")

                if self.fareInfo.isMeihanCityStartTerminalEnable {
                    if self.fareInfo.isMeihanCityTerminal {
                        // 発駅=都区市内
                        items.append("着駅を単駅指定")
                    } else  {
                        items.append("発駅を単駅指定")
                    }
                }
                if self.fareInfo.isEnableLongRoute {
                    if self.fareInfo.isLongRoute {
                        items.append("最安経路で運賃計算")
                    } else {
                        items.append("指定した経路で運賃計算")
                    }
                }
                
                if self.fareInfo.isEnableRule115 {
                    if self.fareInfo.isDisableSpecificTermRule115 {
                        items.append("旅客営業取扱基準規程115条(単駅最安)")
                    } else {
                        items.append("旅客営業取扱基準規程115条(特定都区市内発着)")
                    }
                }
            } else {
                items.append("特例を適用する")
            }
        }

        if self.fareInfo.isJRCentralStockEnable {
            if self.fareInfo.isJRCentralStock {
                items.append("JR東海株主優待券を適用しない")
            } else {
                items.append("JR東海株主優待券を適用する")
            }
        }

        if self.ds.isOsakakanDetourEnable() {
            if self.ds.isOsakakanDetour() {
                items.append("大阪環状線 近回り")
            } else {
                items.append("大阪環状線 遠回り")
            }
        }
        
        if #available(iOS 8, OSX 10.10, *) {            // iOS8
            let ac : UIAlertController = UIAlertController(title: self.title!, message: nil, preferredStyle: .actionSheet)
            for item in items {
                ac.addAction(UIAlertAction(title: item, style: .default, handler: { (action: UIAlertAction) in self.actionSelectProc(action.title!)}))
            }
            ac.addAction(UIAlertAction(title: "キャンセル", style: .cancel, handler: nil))
            // for iPad
            ac.modalPresentationStyle = UIModalPresentationStyle.popover
            ac.popoverPresentationController?.barButtonItem = (sender as! UIBarButtonItem)
            ac.popoverPresentationController?.sourceView = self.view
            // end of for iPad
            self.present(ac, animated: true, completion: nil)

        } else {
            // iOS7
            let actsheet : UIActionSheet = UIActionSheet()
            
            actsheet.delegate = self
            actsheet.title = self.title!

            for item in items {
                actsheet.addButton(withTitle: item)
            }
            actsheet.addButton(withTitle: "キャンセル")
            actsheet.cancelButtonIndex = actsheet.numberOfButtons - 1
            
            if UIDevice.current.userInterfaceIdiom == .pad {
                self.clearsSelectionOnViewWillAppear = false
                self.preferredContentSize = CGSize(width: self.navigationController!.view!.frame.width/2, height: self.view!.frame.height)
                actsheet.show(from: sender as! UIBarButtonItem, animated: true)
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
    }
    
    // action sheet
    // Send to other application.
    //
    @IBAction func actionButtonAction(_ sender: AnyObject) {
        self.ShowAirDrop(sender)
    }
    
    //  Action Sheet
    //
    func actionSheet(_ actionSheet : UIActionSheet, clickedButtonAt buttonIndex : Int) {
        //NSLog(@"action select:%d", buttonIndex);
    
        if (actionSheet.numberOfButtons - 1) == buttonIndex {
            return; // Canceled
        }
    
        let title : String = actionSheet.buttonTitle(at: buttonIndex)!
        actionSelectProc(title)
    }
    
    //  長い処理
    //
    func processDuringIndicatorAnimating(_ param : AnyObject) {
        
        let begin_id : Int = ds.startStationId()
        let end_id : Int = ds.lastStationId()
    
        if let route = cRoute() {
            route.add(begin_id)
            let rc : Int = route.autoRoute(0, arrive: end_id)
            if 1 == rc {
                self.ds.sync(route)
                self.reCalcFareInfo()
                self.tableView.reloadData()
            } else if rc == 0 || rc == 4 || rc == 5 {
                self.ShowAlertView("最短経路算出", message: "経路は片道条件に達しています")
                /* 開始駅 = 終了駅 は前段でチェックしているからここには来ない */
            }
        }
        self.hideIndicate()    /* hide Activity and enable UI */
    }
    
    
    // MARK: - local
    
    // FareInfo 再計算
    func reCalcFareInfo(_ initial : Bool = false) {
        
        // initial = false : fareInfoは有効ではなくてはいけない
        // initial = true  : 初期化プロセスなのでfareInfoは無効で良い
        if (false == initial) && ((nil == self.fareInfo) || (self.fareInfo.result != 0)) {
            return   // Error message
        }
        
        self.fareInfo = self.ds.calcFare()
        if self.fareInfo == nil {
            self.navigationItem.title = "エラー"
            self.navigationItem.prompt = ""
            return
        }
    
        // refresh tableView
        self.setupDispContent()

        if !initial {
            self.tableView.reloadData()
        }

        if self.fareInfo.isFareOptEnabled {
            self.chgOptionButton.isEnabled = true
        } else {
            self.chgOptionButton.isEnabled = false
        }
    }
    
    
    
    /* viewDidLoad */
    
    func setupDispContent() {
        
        if self.fareInfo == nil {
            return
        }
        
        /* KM */
        
        if (self.fareInfo.jrCalcKm == self.fareInfo.jrSalesKm) {
            // 営業キロ
            contentsForKm = [["cell" : "rsKmCell1",
                              "title" : "営業キロ",
                              "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.totalSalesKm)!)km",
                              "subtitle" : "",
                              "value2" : "" ]]
        } else {
            // 営業キロ、計算キロ
            contentsForKm = [["cell" : "rsKmCell1",
                              "title" : "営業キロ",
                             "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.totalSalesKm)!)km",
                             "value2" : "\(cRouteUtil.kmNumStr(self.fareInfo.jrCalcKm)!)km",
                             "subtitle" : (self.fareInfo.companySalesKm != 0) ? "計算キロ(JR)" : "計算キロ"]]
        }
    
        if self.fareInfo.salesKmForHokkaido != 0 {
            if self.fareInfo.calcKmForHokkaido == self.fareInfo.salesKmForHokkaido {
                contentsForKm += [["cell" : "rsKmCell2",
                                   "title" : "JR北海道",
                                   "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.salesKmForHokkaido)!)km"]]
            } else {
                contentsForKm += [["cell" : "rsKmCell2",
                                   "title" : "JR北海道",
                                   "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.salesKmForHokkaido)!)km",
                                   "value2" : "\(cRouteUtil.kmNumStr(self.fareInfo.calcKmForHokkaido)!)km"]]
            }
        }
        if (self.fareInfo.salesKmForKyusyu != 0) {
            if self.fareInfo.calcKmForKyusyu == self.fareInfo.salesKmForKyusyu {
                contentsForKm += [["cell" : "rsKmCell2",
                                  "title" : "JR九州",
                                  "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.salesKmForKyusyu)!)km"]]
            } else {
                contentsForKm += [["cell" : "rsKmCell2",
                                   "title" : "JR九州",
                                   "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.salesKmForKyusyu)!)km",
                                   "value2" : "\(cRouteUtil.kmNumStr(self.fareInfo.calcKmForKyusyu)!)km"]]
            }
        }
        if self.fareInfo.salesKmForShikoku != 0 {
            if (self.fareInfo.calcKmForShikoku == self.fareInfo.salesKmForShikoku) {
                contentsForKm += [["cell" : "rsKmCell2",
                                   "title" : "JR四国",
                                   "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.salesKmForShikoku)!)km"]]
            } else {
                contentsForKm += [["cell" : "rsKmCell2",
                                   "title" : "JR四国",
                                   "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.salesKmForShikoku)!)km",
                                   "value2" : "\(cRouteUtil.kmNumStr(self.fareInfo.calcKmForShikoku)!)km"]]
            }
        }
        if (self.fareInfo.isRule114Applied) {
            //if (self.fareInfo.rule114_salesKm != self.fareInfo.rule114_calcKm) && (0 != self.fareInfo.rule114_calcKm) {
            if (self.fareInfo.jrCalcKm != self.fareInfo.jrSalesKm) {
                contentsForKm += [["cell" : "rsKmCell2x",
                                    "title" : "規程114条適用",
                                    "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.rule114_salesKm)!)km",
                                    "value2" : "\(cRouteUtil.kmNumStr(self.fareInfo.rule114_calcKm)!)km"]]
            } else {
                contentsForKm += [["cell" : "rsKmCell2x",
                                   "title" : "規程114条適用",
                                   "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.rule114_salesKm)!)km"]]
            }
        }

        if ((self.fareInfo.companySalesKm != 0) && (self.fareInfo.brtSalesKm != 0)) {
                // 2行
                // JR線、会社線
                contentsForKm += [[
                    "cell" : "rsKmCell3",
                    "title" : "JR線",
                    "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.jrSalesKm)!)km",
                    "subtitle": "会社線",
                    "value2" : "\(cRouteUtil.kmNumStr(self.fareInfo.companySalesKm)!)km",
                    ]]
                // BRT線
                contentsForKm += [["cell" : "rsKmCell2x",
                                   "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.brtSalesKm)!)km",
                                    "title" : "BRT線",
                                    "subtitle": "",
                                    "value2" : "" ]]
        } else if (self.fareInfo.companySalesKm != 0) {
                    // 1行
                    // JR線、会社線
                contentsForKm += [["cell" : "rsKmCell3",
                                   "title" : "JR線",
                                   "subtitle" : "会社線",
                                   "value2" : "\(cRouteUtil.kmNumStr(self.fareInfo.companySalesKm)!)km",
                                   "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.jrSalesKm)!)km"]]
        } else if (self.fareInfo.brtSalesKm != 0) {
                // 1行
                // JR線、BRT線
                contentsForKm += [["cell" : "rsKmCell3",
                                   "title" : "JR線",
                                   "value2" : "\(cRouteUtil.kmNumStr(self.fareInfo.brtSalesKm)!)km",
                                   "value1" : "\(cRouteUtil.kmNumStr(self.fareInfo.jrSalesKm)!)km",
                                   "subtitle" : "BRT線"]]
        }
        
        
        /* FARE */
        
        /* 1行目 普通＋会社 or 普通 + IC or 普通+BRT */
        /* 2行目 BRT */
        /* 2 or 3行目 (往復）同上 */
        var strFareOpt : String = (self.fareInfo.isRoundtripDiscount) ? " (割引)" : ""

        if (self.fareInfo.isRule114Applied) {
            strFareOpt +=  "(¥\(cRouteUtil.fareNumStr(self.fareInfo.roundTripFareWithCompanyLinePriorRule114)!))"
        }

        if ((self.fareInfo.fareForBRT != 0) && (self.fareInfo.fareForCompanyline != 0)) {
            // 1: 普通運賃+会社線
            // 2: BRT
            contentsForFare = [[
                "cell" : "rsFareCell",
                "title" : "普通",
                "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fare)!)",
                "subtitle" : "うち会社線",
                "value2" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fareForCompanyline)!)"]]
            contentsForFare += [[
                "cell" : "rsPersonDiscountFareCell",
                "title" : "うちBRT",
                "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fareForBRT)!)"
                ]]
            /* 3: 往復運賃(割引可否) ＋ 会社線往復 */
            if (self.fareInfo.isRoundtrip) {
                contentsForFare += [[
                    "cell" : "rsFareCell2",
                    "title": "往復",
                    "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.roundTripFareWithCompanyLine)!)" + strFareOpt,
                    "value2" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fareForCompanyline * 2)!)"]]
            }
        } else if (self.fareInfo.fareForCompanyline != 0) {
            /* 1: 普通運賃＋会社線 */
            contentsForFare = [[
                "cell" : "rsFareCell",
                "title" : "普通",
                "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fare)!)",
                "subtitle" : "うち会社線",
                "value2" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fareForCompanyline)!)"]]
            /* 2: 往復運賃(割引可否) ＋ 会社線往復 */
            if (self.fareInfo.isRoundtrip) {
                contentsForFare += [[
                    "cell" : "rsFareCell2",
                    "title" : "往復",
                    "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.roundTripFareWithCompanyLine)!)" + strFareOpt,
                    "value2" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fareForCompanyline * 2)!)"]]
            }
        } else if (self.fareInfo.fareForBRT != 0) {
            /* 1: 普通運賃＋BRT線 */
            contentsForFare = [[
                "cell" : "rsFareCell",
                "title" : "普通",
                "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fare)!)",
                "subtitle" : "うちBRT線",
                "value2" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fareForBRT)!)"]]
            /* 2: 往復運賃(割引可否) ＋ BRT線往復 */
            if (self.fareInfo.isRoundtrip) {
                contentsForFare += [[
                    "cell" : "rsFareCell2",
                    "title" : "往復",
                    "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.roundTripFareWithCompanyLine)!)" + strFareOpt,
                    "value2" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fareForBRT * 2)!)"]]
            }

        } else if (self.fareInfo.fareForIC != 0) {
            /* 1: 普通運賃 ＋ IC運賃 */
            contentsForFare = [[
                "cell" : "rsFareCell",
                "title" : "普通",
                "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fare)!)",
                "subtitle" : "IC運賃",
                "value2" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fareForIC)!)"]]
            /* 2: 往復運賃 ＋ IC往復運賃 (割引無し) */
            if (self.fareInfo.isRoundtrip) {
                contentsForFare += [[
                    "cell" : "rsFareCell2",
                    "title": "往復",
                    "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.roundTripFareWithCompanyLine)!)" + strFareOpt,
                    "value2" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fareForIC * 2)!)"]]
            }
        } else {
            /* 1: 普通運賃 */
            contentsForFare = [["cell" : "rsFareCell",
                                "title" : "普通",
                                "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.fare)!)",
                                "subtitle" : "",
                                "value2" : ""]]
            /* 2: 往復運賃(割引可否) */
            if (self.fareInfo.isRoundtrip) {
                contentsForFare += [["cell" : "rsFareCell2",
                                     "title" : "往復",
                                     "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.roundTripFareWithCompanyLine)!)" + strFareOpt,
                                     "value2" : ""]]
            }
        }
        
        /* 114 exception */
        if (self.fareInfo.isRule114Applied) {
            let fare_str : String = "(¥\(cRouteUtil.fareNumStr(self.fareInfo.farePriorRule114)!))"
            contentsForFare += [["cell" : "rsDiscountFareCell",
                                 "title" : "規程114条 適用しない運賃",
                                 "value1" : fare_str]]
        }
        
        for i in 0 ..< self.fareInfo.availCountForFareOfStockDiscount {
            var fare_str : String
            if (self.fareInfo.isRule114Applied) {
                fare_str = "¥\(cRouteUtil.fareNumStr(self.fareInfo.fare(forStockDiscount: 2 + i))!)(¥\(cRouteUtil.fareNumStr(self.fareInfo.fare(forStockDiscount: i))!))"
            } else {
                fare_str = "¥\(cRouteUtil.fareNumStr(self.fareInfo.fare(forStockDiscount: i))!)"
            }
            contentsForFare += [["cell" : "rsDiscountFareCell",
                                 "title" : self.fareInfo.fare(forStockDiscountTitle: i),
                                 "value1" : fare_str]]
        }
        
        // Child fare
        if (self.fareInfo.isRoundtrip) {
            contentsForFare += [["cell" : "rsPersonDiscountFareCell",
                                 "title" : "小児運賃",
                                 "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.childFare)!)",
                                 "subtitle" : "往復",
                                 "value2" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.childFare * 2)!)"]]
        } else {
            contentsForFare += [["cell" : "rsPersonDiscountFareCell",
                                 "title" : "小児運賃",
                                 "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.childFare)!)",
                                 "subtitle": "",
                                 "value2": "" ]]
        }

        if self.fareInfo.isAcademicFare {
            if (self.fareInfo.isRoundtrip) {
                contentsForFare += [[
                    "cell" : "rsPersonDiscountFareCell",
                    "title" : "学割運賃",
                    "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.academicFare)!)",
                    "subtitle" : "往復",
                    "value2" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.roundtripAcademicFare)!)"]]
            } else {
                contentsForFare += [[
                    "cell" : "rsPersonDiscountFareCell",
                    "title" : "学割運賃",
                    "value1" : "¥\(cRouteUtil.fareNumStr(self.fareInfo.academicFare)!)",
                    "subtitle": "",
                    "value2":""]]
            }
        }

        contentsForMessage.removeAll();
        if self.fareInfo.isResultCompanyBeginEnd {
            contentsForMessage.append("会社線発着のため一枚の乗車券として発行されない場合があります.")
        }
        if self.fareInfo.isResultCompanyMultipassed {
            /* 2017.3 以降 ここに来ることはない */
            contentsForMessage.append("複数の会社線を跨っているため乗車券は通し発券できません. 運賃額も異なります.")
        }
        if self.fareInfo.isEnableTokaiStockSelect {
            contentsForMessage.append("JR東海株主優待券使用オプション選択可")
        }
        if self.fareInfo.isBRTdiscount {
            contentsForMessage.append("BRT乗り継ぎ割引適用")
        }
        if let s = self.fareInfo.resultMessage {
            if !s.isEmpty {
                let ss : [String] = s.components(separatedBy: "\r\n")
                for sss in ss {
                    contentsForMessage.append(sss)
                }
            }
        }
    }
    
    
    func showIndicate() {
        self.frontView = UIView(frame: self.navigationController!.view.bounds)
        self.frontView.backgroundColor = UIColor.clear
        self.navigationController!.view!.addSubview(self.frontView)
        
        self.indicator = UIActivityIndicatorView(style: UIActivityIndicatorView.Style.whiteLarge)
        self.indicator.color = UIColor.black
        self.indicator.center = self.frontView.center
        self.frontView.addSubview(self.indicator)
        self.frontView.bringSubviewToFront(self.indicator)
        self.indicator.startAnimating()
    }
    
    func hideIndicate() {
        self.indicator.stopAnimating()
        self.indicator.removeFromSuperview()
        self.frontView.removeFromSuperview()
        self.indicator = nil
        self.frontView = nil
        
        self.navigationController?.view.isUserInteractionEnabled = true
        self.tableView.reloadData()
    }
    
    func ShowAirDrop(_ from : AnyObject) {
        if (nil == self.fareInfo) || (self.fareInfo.result != 0) {
            return;   // Error message
        }
        let subject : String = self.resultTitle()
        let shareText : String = self.resultMessage(subject)
        let activityItems : [AnyObject] = [shareText as AnyObject]
        let excludeActivities : [UIActivity.ActivityType] = [UIActivity.ActivityType.postToWeibo]
        
        let activityController : UIActivityViewController = UIActivityViewController(activityItems: activityItems, applicationActivities: nil)
        
        // 除外サービスを指定
        activityController.excludedActivityTypes = excludeActivities
        
        activityController.setValue(subject, forKey: "subject")
        
        if #available(iOS 8, OSX 10.10, *) {            // for iPad(8.3)
            activityController.popoverPresentationController?.sourceView = self.view
            activityController.popoverPresentationController?.barButtonItem = (from as! UIBarButtonItem)
        }
        // modalで表示
        self.present(activityController, animated: true, completion: nil)
    }
        
    func resultTitle() -> String {
        if self.fareInfo == nil {
            return "エラー"
        }
        return "運賃詳細(\(cRouteUtil.terminalName(self.fareInfo.beginStationId)!) - \(cRouteUtil.terminalName(self.fareInfo.endStationId)!))"
    }
    
    func resultMessage(_ subject : String) -> String {
        
        if self.fareInfo == nil {
            return "エラー"
        }

        var body : String = "\(subject)\n\n\(ds.showFare()!)\n"
        
        body = body.replacingOccurrences(of: "\\", with: "¥")
        
        // メールビュー生成
        //_mailViewCtl = [[MFMailComposeViewController alloc] init];
        
        if self.fareInfo.isRuleAppliedEnable {
            if self.fareInfo.isRuleApplied {
                body += "(特例適用)\n"
            } else {
                body += "(特例未適用)\n"
            }
        }
        body += "\n[指定経路]\n"
        body += ds.routeScript()
        
        return body
    }
    
    func ShowAlertView(_ title : String, message message_ : String) {
        if #available(iOS 8, OSX 10.10, *) {            // iOS8 -
            let ac : UIAlertController = UIAlertController(title: self.title!, message: message_, preferredStyle: .alert)
            ac.addAction(UIAlertAction(title: "OK", style: .default, handler: nil))
            self.present(ac, animated: true, completion: nil)
        } else {
            // iOS7
            let alertView : UIAlertView = UIAlertView(title: title, message: message_, delegate: self, cancelButtonTitle: nil, otherButtonTitles: "OK")

            alertView.show()
        }
    }
}
