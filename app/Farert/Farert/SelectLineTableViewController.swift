//
//  SelectLineTableViewController.swift
//  iFarert
//
//  Created by TAKEDA, Noriyuki on 2015/03/23.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

class SelectLineTableViewController: UITableViewController {

    // in
    var companyOrPrefectId : Int = 0
    var baseStationId : Int = 0
    var lastLineId : Int = 0

    var lineList : [Int] = []
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem()
        // Uncomment the following line to preserve selection between presentations.
        self.clearsSelectionOnViewWillAppear = false;
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem;
        
        let apd:AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
        if ((apd.context == FGD.CONTEXT_AUTOROUTE_VIEW) || (apd.context == FGD.CONTEXT_TERMINAL_VIEW)) {
            self.navigationItem.setRightBarButtonItems(nil, animated: true)
        }
        if (apd.context == FGD.CONTEXT_ROUTESELECT_VIEW) {
            lineList = RouteDataController.LineIdsFromStation(self.baseStationId) as! [Int]
        } else { /* TAG_TERMINAL_VIEW */
            lineList = RouteDataController.LinesFromCompanyOrPrefect(self.companyOrPrefectId) as! [Int]
        }
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    // 戻ってきたときにセルの選択を解除
    override func viewDidAppear(animated: Bool) {
        super.viewDidAppear(animated);
        if let idx : NSIndexPath = self.tableView.indexPathForSelectedRow {
            self.tableView.deselectRowAtIndexPath(idx, animated:false)
        }
    }

    // MARK: - Table view data source
    
    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 1
    }
    
    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return lineList.count
    }
    
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        var cell : UITableViewCell
        let lineId : Int = lineList[indexPath.row]
        let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
        
        if ((apd.context == FGD.CONTEXT_ROUTESELECT_VIEW) && (self.lastLineId == lineId)) {
            cell = tableView.dequeueReusableCellWithIdentifier("termLineCell2", forIndexPath: indexPath) 
        } else {
            cell = tableView.dequeueReusableCellWithIdentifier("termLineCell", forIndexPath: indexPath) 
        }
        
        cell.textLabel?.text = RouteDataController.LineName(lineId)
        
        return cell;
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

    override func tableView(tableView: UITableView, titleForHeaderInSection section : Int) -> String? {
        let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
    
        if (apd.context == FGD.CONTEXT_ROUTESELECT_VIEW) {
            return RouteDataController.StationName(baseStationId)
        } else {
            return RouteDataController.CompanyOrPrefectName(self.companyOrPrefectId)
        }
    }
    

    // MARK: - Navigation
    
    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        let segid : String = segue.identifier!
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        if (segid == "termLineToStationSegue") {
            // 駅一覧テーブルビュー(to SelectStationTableView)
            let selStationViewController : SelectStationTableViewController = segue.destinationViewController as! SelectStationTableViewController
            selStationViewController.companyOrPrefectId = self.companyOrPrefectId;
            selStationViewController.lineId = Int(self.lineList[self.tableView.indexPathForSelectedRow?.row ?? 0])
            selStationViewController.lastStationId = self.baseStationId
            selStationViewController.transit_state = FGD.FA_TRANSIT_STA2JCT
        } else if (segid  == "autoRouteSegue") {
            // 最短経路ボタン(to TermSelectTableView)
            let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
            apd.context = FGD.CONTEXT_AUTOROUTE_VIEW;
            //UINavigationController* nvc = [segue destinationViewController];
            //CSTableViewController* vc = [nvc.viewControllers  objectAtIndex:0];
            //CSTableViewController* vc = [segue destinationViewController];
            //vc.transit_state = FA_TRANSIT_AUTOROUTE;
        }
    }
}



