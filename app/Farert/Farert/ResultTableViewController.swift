//
//  ResultTableViewController.swift
//  iFarert
//
//  Created by TAKEDA, Noriyuki on 2015/03/23.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit
import MessageUI



class ResultTableViewController: UITableViewController, UIActionSheetDelegate, UIDocumentInteractionControllerDelegate, UIAlertViewDelegate {

    // MARK: Constant
    
    // MARK: Public property
    var ds : RouteDataController = RouteDataController()

    // MARK: Local property
    
    var num_of_km : Int = 0
    var num_of_fare : Int = 0
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
        
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "preferredContentSizeChanged", name: UIContentSizeCategoryDidChangeNotification, object: nil)
        
        self.tableView.estimatedRowHeight = 100.0
        if 8.0 <= ((UIDevice.currentDevice().systemVersion as NSString).floatValue) {
            self.tableView.rowHeight = UITableViewAutomaticDimension
        }
        //NSLog(@"ResultView didLoad entry");
        // Uncomment the following line to preserve selection between presentations.
        // self.clearsSelectionOnViewWillAppear = NO;
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem;
        
        self.navigationController?.toolbarHidden = false
        
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
    override func viewWillAppear(animated : Bool) {
        super.viewWillAppear(animated)
        if let idx : NSIndexPath = self.tableView.indexPathForSelectedRow() {
            self.tableView.deselectRowAtIndexPath(idx, animated:false)
        }
    }
    
    override func viewDidAppear(animated: Bool) {
        super.viewDidAppear(animated)
        self.tableView.reloadData()
    }

    // MARK: - Table view data source

    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        if self.fareInfo.result == 0 {
            return 6;
        } else {
            return 1;
        }
    }

    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        
        if (nil == self.fareInfo) || (self.fareInfo.result != 0) {
            return 1;   // Error message
        }
        switch (section) {
        case 0:
            return 1; /* section */
        case 1:
            return num_of_km; /* sales, calc km */
        case 2:
            return num_of_fare; /* Fare */
        case 3:
            return self.contentsForMessage.count
        case 4, 5:            /* route list */
            return 1;
        default:
            break;
        }
        return 0;
    }


    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        
        var cell : UITableViewCell
        
        if (nil == self.fareInfo) || (self.fareInfo.result != 0) {    // error
            cell = tableView.dequeueReusableCellWithIdentifier("rsRouteListCell", forIndexPath: indexPath) as!UITableViewCell
            //lbl = (UILabel*)[cell viewWithTag:1];
            //lbl.text = [self.fareInfo routeList];
            cell.textLabel?.font = UIFont.preferredFontForTextStyle(UIFontTextStyleBody)
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
            cell.textLabel?.font = UIFont.preferredFontForTextStyle(UIFontTextStyleBody)
            cell.textLabel?.numberOfLines = 0;
            return cell;   // Error message
        }
        
        switch indexPath.section {
        case 0:
            /* section */
            cell = tableView.dequeueReusableCellWithIdentifier("rsTitleCell", forIndexPath: indexPath) as! UITableViewCell

            let lbl : UILabel = cell.viewWithTag(1) as! UILabel
            lbl.text = "\(RouteDataController.TerminalName(self.fareInfo.beginStationId)) → \(RouteDataController.TerminalName(self.fareInfo.endStationId))"
            
        case 1:
            /* KM */
            let dic : [String : String] = self.contentsForKm[indexPath.row]
            
            if indexPath.row == 0 {
                cell = tableView.dequeueReusableCellWithIdentifier("rsKmCell1", forIndexPath:indexPath) as! UITableViewCell
                var lbl : UILabel = cell.viewWithTag(2) as! UILabel
                lbl.text = dic["salesKm"]
                if let str : String = dic["calcKm"] {
                    lbl = cell.viewWithTag(3) as! UILabel
                    lbl.text = dic["subTitle"]
                    lbl = cell.viewWithTag(4) as! UILabel
                    lbl.text = str;
                } else {
                    lbl = cell.viewWithTag(3) as! UILabel
                    lbl.text = ""
                    lbl = cell.viewWithTag(4) as! UILabel
                    lbl.text = ""
                }
            } else {
                if nil == dic["Company_salesKm"] {
                    if ((dic["title"]?.rangeOfString("114")) != nil) {
                        cell = tableView.dequeueReusableCellWithIdentifier("rsKmCell2x", forIndexPath:indexPath) as! UITableViewCell
                    } else {
                        cell = tableView.dequeueReusableCellWithIdentifier("rsKmCell2", forIndexPath:indexPath) as! UITableViewCell
                    }
                    var lbl : UILabel = cell.viewWithTag(1) as! UILabel
                    lbl.text = dic["title"]
                    lbl = cell.viewWithTag(2) as! UILabel
                    lbl.text = dic["salesKm"]
                    
                    lbl = cell.viewWithTag(4) as! UILabel
                    if let str : String? = dic["calcKm"] {
                        lbl.text = str
                    } else {
                        lbl.text = ""
                    }
                } else {
                    cell = tableView.dequeueReusableCellWithIdentifier("rsKmCell3", forIndexPath: indexPath) as! UITableViewCell
                    var lbl : UILabel = cell.viewWithTag(2) as! UILabel
                    lbl.text = dic["JR_salesKm"]
                    lbl = cell.viewWithTag(4) as! UILabel
                    lbl.text = dic["Company_salesKm"]
                }
            }
            
        case 2:
            /* FARE */
            let dic : [String : String] = self.contentsForFare[indexPath.row]
            
            switch indexPath.row  {
            case 0:
                cell = tableView.dequeueReusableCellWithIdentifier("rsFareCell", forIndexPath: indexPath) as! UITableViewCell
                var lbl : UILabel = cell.viewWithTag(2) as! UILabel
                lbl.text = dic["fare"];
                
                lbl = cell.viewWithTag(3) as! UILabel
                lbl.text = dic["subTitle"];
                
                lbl = cell.viewWithTag(4) as! UILabel
                lbl.text = dic["subFare"]; /* company or IC */
                
            case 1:
                cell = tableView.dequeueReusableCellWithIdentifier("rsFareCell2", forIndexPath: indexPath) as! UITableViewCell
                var lbl : UILabel = cell.viewWithTag(2) as! UILabel
                lbl.text = dic["fare"];

                lbl = cell.viewWithTag(4) as! UILabel
                lbl.text = dic["subFare"];
                
            default:
                var lbl : UILabel
                
                if let subtitle = dic["subTitle"] {
                    cell = tableView.dequeueReusableCellWithIdentifier("rsPersonDiscountFareCell", forIndexPath: indexPath) as! UITableViewCell

                    lbl = cell.viewWithTag(3) as! UILabel
                    lbl.text = subtitle
                    
                    lbl = cell.viewWithTag(4) as! UILabel
                    lbl.text = dic["subFare"]
                    
                } else {
                    cell = tableView.dequeueReusableCellWithIdentifier("rsDiscountFareCell", forIndexPath: indexPath) as! UITableViewCell
                }
                lbl = cell.viewWithTag(1) as! UILabel
                lbl.text = dic["title"];

                lbl = cell.viewWithTag(2) as! UILabel
                lbl.text = dic["fare"];
                
                break
            }
        case 3:
            let message : String = self.contentsForMessage[indexPath.row]
            cell = tableView.dequeueReusableCellWithIdentifier("rsMetroAvailDaysCell", forIndexPath: indexPath) as! UITableViewCell
            let lbl : UILabel = cell.viewWithTag(1) as! UILabel
            lbl.text = message
            
        case 4:
            /* avail days */
            if (false == self.fareInfo.isArbanArea) {
                cell = tableView.dequeueReusableCellWithIdentifier("rsAvailDaysCell", forIndexPath: indexPath) as! UITableViewCell
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
            } else {
                cell = tableView.dequeueReusableCellWithIdentifier("rsMetroAvailDaysCell", forIndexPath: indexPath) as! UITableViewCell
                if nil == self.fareInfo {
                    cell.textLabel?.text = "エラー"
                    assert(false)
                }
            }
            
        case 5:
            /* ROUTE */
            let rcell : RouteListTableViewCell = tableView.dequeueReusableCellWithIdentifier("rsRouteListCell", forIndexPath: indexPath) as! RouteListTableViewCell
            rcell.routeString.text = self.fareInfo.routeList!
            cell = rcell
        default:
            cell = tableView.dequeueReusableCellWithIdentifier("rsRouteListCell", forIndexPath: indexPath) as! UITableViewCell
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

    override func tableView(tableView : UITableView, titleForHeaderInSection section : Int) -> String? {
        
        if (self.fareInfo?.result != 0) {
            let title : String = "無効な経路"   // Error message
            self.navigationController?.title = title
            return title
        }
 
        self.navigationItem.title = "\(RouteDataController.TerminalName(self.fareInfo.endStationId))"
        self.navigationItem.prompt = "\(RouteDataController.TerminalName(self.fareInfo.beginStationId)) → "

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
    override func tableView(tableView : UITableView, heightForRowAtIndexPath indexPath : NSIndexPath) -> CGFloat {

        var value : CGFloat
        
        if self.fareInfo == nil {
            return super.tableView(tableView, heightForRowAtIndexPath: indexPath)
        }

        var ident : String = ""
        
        switch indexPath.section {
        case 0:
            /* section */
            ident = "rsTitleCell"
            value = 51

        case 1:
            /* KM */
            if indexPath.row == 0 {
                ident = "rsKmCell1"
                value = 44
                
            } else {
                let dic : [String : String] = self.contentsForKm[indexPath.row]
                if nil == dic["Company_salesKm"] {
                    ident = "rsKmCell2"
                    value = 21
                } else {
                    ident = "rsKmCell3"
                    value = 34
                }
            }

        case 2:
            /* FARE */
            if indexPath.row == 0 {
                ident = "rsFareCell"
                value = 44
            } else if indexPath.row == 1 {
                ident = "rsFareCell2"
                value = 36
            } else {
                ident = "rsDiscountFareCell"
                value = 24
            }
        case 3:
                    ident = "rsMetroAvailDaysCell"
                    value = 44
        case 4:
            /* avail days */
            if !self.fareInfo.isArbanArea ?? false {
                ident = "rsAvailDaysCell"
                value = 44
            } else {
                ident = "rsMetroAvailDaysCell"
                value = 44
            }

        case 5:
            /* ROUTE */
            let cell : RouteListTableViewCell = tableView.dequeueReusableCellWithIdentifier("rsRouteListCell"/*, forIndexPath: indexPath  EXC_BAD_ACCESSになる*/) as! RouteListTableViewCell
            return cell.heightForTitle(self.fareInfo.routeList)

        default:
            value = UITableViewAutomaticDimension
            break;
        }
        
        if !ident.isEmpty || 0 < value {
            return value
        }
        return super.tableView(tableView, heightForRowAtIndexPath: indexPath)
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
    func preferredContentSizeChanged(aNotification: NSNotification) {
        // refresh tableView
        self.tableView.reloadData()
    }
    
    // MARK: - Action

    // Action menu selected
    
    func actionSelectProc(title: String) {
        if nil != title.rangeOfString("特例") {
            if nil != title.rangeOfString("しない") {
                // @"特例を適用しない";
                ds.setFareOption(FGD.FAREOPT_RULE_NO_APPLIED, availMask: FGD.FAREOPT_AVAIL_RULE_APPLIED)
            } else {
                // @"特例を適用する";
                ds.setFareOption(FGD.FAREOPT_RULE_APPLIED, availMask: FGD.FAREOPT_AVAIL_RULE_APPLIED)
            }
            self.reCalcFareInfo()
            
        } else if nil != title.rangeOfString("単駅") {
            if nil != title.rangeOfString("発駅") {
                // "発駅を単駅指定";
                ds.setFareOption(FGD.FAREOPT_APPLIED_TERMINAL, availMask: FGD.FAREOPT_AVAIL_APPLIED_START_TERMINAL)
            } else {
                // "着駅を単駅指定";
                ds.setFareOption(FGD.FAREOPT_APPLIED_START, availMask: FGD.FAREOPT_AVAIL_APPLIED_START_TERMINAL)
            }
            self.reCalcFareInfo()
            self.tableView.reloadData()
            
        } else if nil != title.rangeOfString("最短経路") {
            self.showIndicate()
            self.navigationController?.view.userInteractionEnabled = false
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, Int64(0.1 * Double(NSEC_PER_SEC))), dispatch_get_main_queue(), {
                NSThread.detachNewThreadSelector(Selector("processDuringIndicatorAnimating:"), toTarget:self, withObject: nil)
            })
            
        } else if nil != title.rangeOfString("回り") {
            let rc : Int = ds.setDetour((nil != title.rangeOfString("遠")) ? true : false)
            if 0 <= rc {
                self.reCalcFareInfo()
                self.tableView.reloadData()
            } else {
                self.ShowAlertView("エラー", message: "経路が重複するため指定できません")
            }
        } else {
            // cancel
        }
    }

    //  Compose button
    //
    @IBAction func chgOptionButtonAction(sender: AnyObject) {

        if self.fareInfo == nil {
            return
        }

        var items : [String] = Array<String>()
        
        if self.fareInfo.isRuleAppliedEnable() {
            if self.fareInfo.isRuleApplied() {
                items.append("特例を適用しない")

                if self.fareInfo.isMeihanCityStartTerminalEnable() {
                    if self.fareInfo.isMeihanCityStart() {
                        // 発駅=都区市内
                        items.append("発駅を単駅指定")
                    } else  {
                        items.append("着駅を単駅指定")
                    }
                }
            } else {
                items.append("特例を適用する")
            }
        }
        if (self.fareInfo.isArbanArea) {
            items.append("最短経路算出")
        }
        
        if self.fareInfo.isOsakakanDetourEnable() {
            if self.fareInfo.isOsakakanDetourShortcut() {
                items.append("大阪環状線 近回り")
            } else {
                items.append("大阪環状線 遠回り")
            }
        }
        
        if nil != objc_getClass("UIAlertController") {
            // iOS8
            let ac : UIAlertController = UIAlertController(title: self.title!, message: nil, preferredStyle: .ActionSheet)
            for item in items {
                ac.addAction(UIAlertAction(title: item, style: .Default, handler: { (action: UIAlertAction!) in self.actionSelectProc(action!.title)}))
            }
            ac.addAction(UIAlertAction(title: "キャンセル", style: .Cancel, handler: nil))
            // for iPad
            ac.modalPresentationStyle = UIModalPresentationStyle.Popover
            ac.popoverPresentationController?.barButtonItem = sender as! UIBarButtonItem
            ac.popoverPresentationController?.sourceView = self.view
            // end of for iPad
            self.presentViewController(ac, animated: true, completion: nil)

        } else {
            // iOS7
            let actsheet : UIActionSheet = UIActionSheet()
            
            actsheet.delegate = self
            actsheet.title = self.title!

            for item in items {
                actsheet.addButtonWithTitle(item)
            }
            actsheet.addButtonWithTitle("キャンセル")
            actsheet.cancelButtonIndex = actsheet.numberOfButtons - 1
            
            if UIDevice.currentDevice().userInterfaceIdiom == .Pad {
                self.clearsSelectionOnViewWillAppear = false
                self.preferredContentSize = CGSize(width: self.navigationController!.view!.frame.width/2, height: self.view!.frame.height)
                actsheet.showFromBarButtonItem(sender as! UIBarButtonItem, animated: true)
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
    }
    
    // action sheet
    // Send to other application.
    //
    @IBAction func actionButtonAction(sender: AnyObject) {
        self.ShowAirDrop(sender)
    }
    
    //  Action Sheet
    //
    func actionSheet(actionSheet : UIActionSheet, clickedButtonAtIndex buttonIndex : Int) {
        //NSLog(@"action select:%d", buttonIndex);
    
        if (actionSheet.numberOfButtons - 1) == buttonIndex {
            return; // Canceled
        }
    
        let title : String = actionSheet.buttonTitleAtIndex(buttonIndex)
        actionSelectProc(title)
    }
    
    //  長い処理
    //
    func processDuringIndicatorAnimating(param : AnyObject) {
        
        let begin_id : Int = ds.startStationId()
        let end_id : Int = ds.lastStationId()
        
        ds.resetStartStation()
        ds.addRoute(begin_id)
        let rc : Int = ds.autoRoute(false, arrive: end_id)
        if 0 <= rc {
            self.reCalcFareInfo()
            self.tableView.reloadData()
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

        if self.fareInfo.isFareOptEnabled() || self.fareInfo.isArbanArea {
            self.chgOptionButton.enabled = true
        } else {
            self.chgOptionButton.enabled = false
        }
    }
    
    
    
    /* viewDidLoad */
    
    func setupDispContent() {
        
        if self.fareInfo == nil {
            return
        }
        
        /* KM */
        
        num_of_km = 1;

        if (self.fareInfo.jrCalcKm == self.fareInfo.jrSalesKm) {
            contentsForKm = [["salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.totalSalesKm))km"]]
        } else {
            contentsForKm = [["salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.totalSalesKm))km",
                             "calcKm" : "\(RouteDataController.kmNumStr(self.fareInfo.jrCalcKm))km",
                             "subTitle" : (self.fareInfo.companySalesKm != 0) ? "計算キロ(JR)" : "計算キロ"]]
        }
    
        if self.fareInfo.salesKmForHokkaido != 0 {
            num_of_km++;
            if self.fareInfo.calcKmForHokkaido == self.fareInfo.salesKmForHokkaido {
                contentsForKm += [["title" : "JR北海道",
                                   "salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.salesKmForHokkaido))km"]]
            } else {
                contentsForKm += [["title" : "JR北海道",
                                   "salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.salesKmForHokkaido))km",
                                   "calcKm" : "\(RouteDataController.kmNumStr(self.fareInfo.calcKmForHokkaido))km"]]
            }
        }
        if (self.fareInfo.salesKmForKyusyu != 0) {
            num_of_km++;
            if self.fareInfo.calcKmForKyusyu == self.fareInfo.salesKmForKyusyu {
                contentsForKm += [["title" : "JR九州",
                                  "salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.salesKmForKyusyu))km"]]
            } else {
                contentsForKm += [["title" : "JR九州",
                                   "salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.salesKmForKyusyu))km",
                                   "calcKm" : "\(RouteDataController.kmNumStr(self.fareInfo.calcKmForKyusyu))km"]]
            }
        }
        if self.fareInfo.salesKmForShikoku != 0 {
            num_of_km++;
            if (self.fareInfo.calcKmForShikoku == self.fareInfo.salesKmForShikoku) {
                contentsForKm += [["title" : "JR四国",
                                   "salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.salesKmForShikoku))km"]]
            } else {
                contentsForKm += [["title" : "JR四国",
                                   "salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.salesKmForShikoku))km",
                                   "calcKm" : "\(RouteDataController.kmNumStr(self.fareInfo.calcKmForShikoku))km"]]
            }
        }
        if (self.fareInfo.isRule114Applied) {
            num_of_km++;
            //if (self.fareInfo.rule114_salesKm != self.fareInfo.rule114_calcKm) && (0 != self.fareInfo.rule114_calcKm) {
            if (self.fareInfo.jrCalcKm != self.fareInfo.jrSalesKm) {
                contentsForKm += [["title" : "規程114条適用",
                                    "salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.rule114_salesKm))km",
                                    "calcKm" : "\(RouteDataController.kmNumStr(self.fareInfo.rule114_calcKm))km"]]
            } else {
                contentsForKm += [["title" : "規程114条適用",
                    "salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.rule114_salesKm))km"]]
            }
        }
        if (self.fareInfo.companySalesKm != 0) {
            num_of_km++;
            contentsForKm += [["Company_salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.companySalesKm))km",
                               "JR_salesKm" : "\(RouteDataController.kmNumStr(self.fareInfo.jrSalesKm))km"]]
        }
        
        
        /* FARE */
        
        num_of_fare = 3;    /* normal + round + child */
        
        /* 1行目 普通＋会社 or 普通 + IC */
        /* 2行目 (往復）同上 */
        var strFareOpt : String = (self.fareInfo.isRoundtripDiscount) ? " (割引)" : ""

        if (self.fareInfo.isRule114Applied) {
            strFareOpt +=  "(¥\(RouteDataController.fareNumStr(self.fareInfo.roundTripFareWithCompanyLinePriorRule114)))"
        }

        if (self.fareInfo.fareForCompanyline != 0) {
            /* 1: 普通運賃＋会社線 */
            contentsForFare = [["fare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.fare))",
                                "subTitle" : "うち会社線",
                                "subFare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.fareForCompanyline))"]]
            /* 2: 往復運賃(割引可否) ＋ 会社線往復 */
            contentsForFare += [["fare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.roundTripFareWithCompanyLine))" + strFareOpt,
                                "subFare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.fareForCompanyline * 2))"]]
        } else if (self.fareInfo.fareForIC != 0) {
            /* 1: 普通運賃 ＋ IC運賃 */
            contentsForFare = [["fare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.fare))",
                                "subTitle" : "IC運賃",
                                "subFare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.fareForIC))"]]
            /* 2: 往復運賃 ＋ IC往復運賃 (割引無し) */
            contentsForFare += [["fare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.roundTripFareWithCompanyLine))" + strFareOpt,
                                 "subFare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.fareForIC * 2))"]]
        } else {
            /* 1: 普通運賃 */
            contentsForFare = [["fare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.fare))",
                                "subTitle" : "", "subFare" : ""]]
            /* 2: 往復運賃(割引可否) */
            contentsForFare += [["fare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.roundTripFareWithCompanyLine))" + strFareOpt,
                                 "subFare" : ""]]
        }
        
        /* 114 exception */
        if (self.fareInfo.isRule114Applied) {
            num_of_fare++;
            let fare_str : String = "(¥\(RouteDataController.fareNumStr(self.fareInfo.farePriorRule114)))"
            contentsForFare += [["title" : "規程114条 適用しない運賃",
                                 "fare" : fare_str]]
        }
        
        /* stock discount */
        num_of_fare += self.fareInfo.availCountForFareOfStockDiscount;
        
        for (var i : Int = 0; i < self.fareInfo.availCountForFareOfStockDiscount; i++) {
            var fare_str : String
            if (self.fareInfo.isRule114Applied) {
                fare_str = "¥\(RouteDataController.fareNumStr(self.fareInfo.fareForStockDiscount(2 + i)))(¥\(RouteDataController.fareNumStr(self.fareInfo.fareForStockDiscount(i))))"
            } else {
                fare_str = "¥\(RouteDataController.fareNumStr(self.fareInfo.fareForStockDiscount(i)))"
            }
            contentsForFare += [["title" : self.fareInfo.fareForStockDiscountTitle(i), "fare" : fare_str]]
        }
        
        // Child fare
        contentsForFare += [["title" : "小児運賃",
                             "fare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.childFare))",
                             "subTitle" : "往復",
                             "subFare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.childFare * 2))"]]
        if self.fareInfo.isAcademicFare {
            num_of_fare += 1
            contentsForFare += [["title" : "学割運賃",
                "fare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.academicFare))",
                "subTitle" : "往復",
                "subFare" : "¥\(RouteDataController.fareNumStr(self.fareInfo.roundtripAcademicFare))"]]
        }

        if self.fareInfo.isResultCompanyBeginEnd() {
            contentsForMessage.append("会社線通過連絡運輸ではないためJR窓口で乗車券は発券されません.")
        }
        if self.fareInfo.isResultCompanyMultipassed() {
            contentsForMessage.append("複数の会社線を跨っているため、乗車券は通し発券できません. 運賃額も異なります.")
        }
    }
    
    
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
        self.indicator = nil
        self.frontView = nil
        
        self.navigationController?.view.userInteractionEnabled = true
        self.tableView.reloadData()
    }
    
    func ShowAirDrop(from : AnyObject) {
        if (nil == self.fareInfo) || (self.fareInfo.result != 0) {
            return;   // Error message
        }
        let subject : String = self.resultTitle()
        let shareText : String = self.resultMessage(subject)
        let activityItems : [AnyObject] = [shareText as AnyObject]
        let excludeActivities : [AnyObject] = [UIActivityTypePostToWeibo]
        
        let activityController : UIActivityViewController = UIActivityViewController(activityItems: activityItems, applicationActivities: nil)
        
        // 除外サービスを指定
        activityController.excludedActivityTypes = excludeActivities
        
        activityController.setValue(subject, forKey: "subject")
        
        if (UIDevice.currentDevice().systemVersion as NSString).floatValue >= 8.0 {
            // for iPad(8.3)
            activityController.popoverPresentationController?.sourceView = self.view
            activityController.popoverPresentationController?.barButtonItem = from as! UIBarButtonItem
        }
        // modalで表示
        self.presentViewController(activityController, animated: true, completion: nil)
    }
        
    func resultTitle() -> String {
        if self.fareInfo == nil {
            return "エラー"
        }
        return "運賃詳細(\(RouteDataController.TerminalName(self.fareInfo.beginStationId)) - \(RouteDataController.TerminalName(self.fareInfo.endStationId)))"
    }
    
    func resultMessage(subject : String) -> String {
        
        if self.fareInfo == nil {
            return "エラー"
        }

        var body : String = "\(subject)\n\n\(ds.showFare())\n"
        
        body = body.stringByReplacingOccurrencesOfString("\\", withString: "¥")
        
        // メールビュー生成
        //_mailViewCtl = [[MFMailComposeViewController alloc] init];
        
        if self.fareInfo.isRuleAppliedEnable() {
            if self.fareInfo.isRuleApplied() {
                body += "(特例適用)\n"
            } else {
                body += "(特例未適用)\n"
            }
        }
        body += "\n[指定経路]\n"
        body += ds.routeScript()
        
        return body
    }
    
    func ShowAlertView(title : String, message message_ : String) {
        if nil != objc_getClass("UIAlertController") {
            // iOS8 -
            let ac : UIAlertController = UIAlertController(title: self.title!, message: message_, preferredStyle: .Alert)
            ac.addAction(UIAlertAction(title: "OK", style: .Default, handler: nil))
            self.presentViewController(ac, animated: true, completion: nil)
        } else {
            // iOS7
            let alertView : UIAlertView = UIAlertView(title: title, message: message_, delegate: self, cancelButtonTitle: nil, otherButtonTitles: "OK")

            alertView.show()
        }
    }
}
