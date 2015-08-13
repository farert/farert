//
//  SelectStationTableViewController.swift
//  iFarert
//
//  Created by TAKEDA, Noriyuki on 2015/03/30.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

class SelectStationTableViewController: CSTableViewController {
    // in
    var companyOrPrefectId : Int = 0
    var lineId : Int = 0
    var lastStationId : Int = 0

    //
    var stations : [Int] = [];

    override func viewDidLoad() {
        super.viewDidLoad()
        
        //NSLog(@"didload begin: transit %d", self.transit_state);
        
        // Uncomment the following line to preserve selection between presentations.
        self.clearsSelectionOnViewWillAppear = false
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem;
        let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate

        if (apd.context == FGD.CONTEXT_AUTOROUTE_VIEW) || (apd.context == FGD.CONTEXT_TERMINAL_VIEW) {
            self.navigationItem.setRightBarButtonItems(nil, animated:true)
            stations = RouteDataController.StationsWithInCompanyOrPrefectAnd(companyOrPrefectId, lineId: lineId) as! [Int]
            if (apd.context == FGD.CONTEXT_AUTOROUTE_VIEW) {
                self.title = "着駅指定";
            } else {
                self.title = "発駅指定";
            }
        } else { //CONTEXT_ROUTESELECT_VIEW
            if (FGD.FA_TRANSIT_STA2JCT != self.transit_state) {
                self.stations = RouteDataController.StationsIdsOfLineId(lineId) as! [Int]
                self.title = "着駅指定"
                self.navigationItem.rightBarButtonItem!.title = "分岐駅指定";
            } else {
                stations = RouteDataController.JunctionIdsOfLineId(lineId) as! [Int]
                self.title = "分岐駅指定"
                self.navigationItem.rightBarButtonItem!.title = "着駅指定"
            }
        }
        //NSLog(@"didload last:transit %d", self.transit_state);
    }
    
    
    @IBAction func terminalBarButtonAction(sender: AnyObject) {
        self.performSegueWithIdentifier("stationViewSegue", sender: self)
    }
   
    
    // mark - Table view data source
    
    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        // Return the number of sections.
        return 1;
    }
    
    
    override func tableView(tableView : UITableView, titleForHeaderInSection section : Int) -> String? {
        if (0 < self.companyOrPrefectId) {
            let companyOrPrefctString : String = RouteDataController.CompanyOrPrefectName(companyOrPrefectId)
            return "\(companyOrPrefctString) - \(RouteDataController.LineName(self.lineId))" as String

        } else {
            return RouteDataController.LineName(lineId) as String
        }
    }

    
    
    override func tableView(tableView : UITableView, numberOfRowsInSection section : Int) -> Int {
        // Return the number of rows in the section.
        return stations.count
    }
    
    
    override func tableView(tableView : UITableView, cellForRowAtIndexPath indexPath : NSIndexPath) -> UITableViewCell {

        let stationId : Int = stations[indexPath.row]
        var cell : UITableViewCell! = nil
        
        let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
        if ((apd.context == FGD.CONTEXT_ROUTESELECT_VIEW) && (self.lastStationId == stationId)) {
            //インデント付き＋チェックマーク付き（起点駅)
            cell = tableView.dequeueReusableCellWithIdentifier("terminalStationCell2", forIndexPath: indexPath) as! UITableViewCell
        } else {
            cell = tableView.dequeueReusableCellWithIdentifier("terminalStationCell", forIndexPath: indexPath) as! UITableViewCell
        }
        
        // Configure the cell...;
        cell!.textLabel?.text = RouteDataController.StationName(stationId)
        
        var details : String = "(\(RouteDataController.GetKanaFromStationId(stationId)))"
        if (RouteDataController.IsJunction(stationId) &&
            !RouteDataController.IsSpecificJunction(self.lineId, stationId: stationId)) {
                /* junction(lflag.bit12 on) */
            var lid : Int
            for lid in RouteDataController.LineIdsFromStation(stationId) as! [Int] {
                if ((lid != self.lineId)  &&
                    !RouteDataController.IsSpecificJunction(lid, stationId: stationId)) {
                    details += "/" + RouteDataController.LineName(lid)
                }
            }
            //if ("/" == details[details.endIndex.predecessor()]) {
            //    details += RouteDataController.LineName(self.lineId)
            //}
        }

        cell.detailTextLabel?.text = details
        
        return cell;
    }
    
    
    // 戻ってきたときにセルの選択を解除
    override func viewWillAppear(animated : Bool) {
        super.viewWillAppear(animated)
        if let idx : NSIndexPath = self.tableView.indexPathForSelectedRow() {
            self.tableView.deselectRowAtIndexPath(idx, animated:false)
        }
    }
    
    // MARK: - Navigation
    
    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        let segid : String = segue.identifier!
        
        if (segid == "terminalSelectDoneSegue") {
            let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
            apd.selectTerminalId = 0xffff & self.stations[self.tableView.indexPathForSelectedRow()!.row]
            apd.selectLineId = self.lineId;
            
        } else if (segid == "stationViewSegue") {
            let thisViewController : SelectStationTableViewController = segue.destinationViewController as! SelectStationTableViewController
            //NSLog(@"prepare segue: transit set %d-> ", self.transit_state);
            thisViewController.transit_state = (self.transit_state == FGD.FA_TRANSIT_STA2JCT) ? FGD.FA_TRANSIT_JCT2STA : FGD.FA_TRANSIT_STA2JCT;
            thisViewController.lineId = self.lineId;
            thisViewController.companyOrPrefectId = self.companyOrPrefectId;
            assert(self.companyOrPrefectId == 0, "bug")
            thisViewController.lastStationId = self.lastStationId;
        }
    }
}
