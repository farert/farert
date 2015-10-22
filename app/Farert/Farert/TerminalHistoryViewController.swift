//
//  TerminalHistoryViewController.swift
//  iFarert
//
//  Created by TAKEDA, Noriyuki on 2015/03/23.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

class TerminalHistoryViewController: CSTableViewController {

    let HEADER_HEIGHT:CGFloat        = 44.0
    
    // MARK: Public property
    var historyTerms : [String] = []
    
    // MARK: UI property
    @IBOutlet weak var clerBarButtonItem: UIBarButtonItem!

    override func viewDidLoad() {
        super.viewDidLoad()
    
        // Uncomment the following line to preserve selection between presentations.
        self.clearsSelectionOnViewWillAppear = false
    
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        self.navigationItem.rightBarButtonItem = self.editButtonItem()
        let h : [String]? = RouteDataController.ReadFromTerminalHistory() as! [String]?
        if h != nil {
            self.historyTerms = h!
        } else {
            self.historyTerms = []
        }
        if self.historyTerms.count <= 0 {
            self.navigationItem.rightBarButtonItem!.enabled = false
        }
        self.clerBarButtonItem.enabled = false
    }
    
    // 戻ってきたときにセルの選択を解除
    override func viewWillAppear(animated : Bool) {
        super.viewWillAppear(animated)
        if let idx : NSIndexPath = self.tableView.indexPathForSelectedRow {
            self.tableView.deselectRowAtIndexPath(idx, animated:false)
        }
    }
    
    // MARK: - Table view data source
    
    override func numberOfSectionsInTableView(tableView : UITableView) -> Int {
        // Return the number of sections.
        return 1;
    }
    
    override func tableView(tableView : UITableView, numberOfRowsInSection section : Int) -> Int {
        if section == 0 {
            // Return the number of rows in the section.
            return self.historyTerms.count
        } else {
            return 0
        }
    }
    
    override func tableView(tableView : UITableView, cellForRowAtIndexPath indexPath : NSIndexPath) -> UITableViewCell {
    
        let cell : UITableViewCell = tableView.dequeueReusableCellWithIdentifier("termHistoryCell", forIndexPath: indexPath) 

        if (indexPath.row < self.historyTerms.count) {
            cell.textLabel?.text = self.historyTerms[indexPath.row]
        }
        return cell
    }
    
    override func tableView(tableView : UITableView, editingStyleForRowAtIndexPath indexPath : NSIndexPath) -> UITableViewCellEditingStyle {
        self.clerBarButtonItem.enabled = true
        return UITableViewCellEditingStyle.Delete
    }
    
    // Override to support conditional editing of the table view.
    override func tableView(tableView : UITableView, canEditRowAtIndexPath indexPath : NSIndexPath) -> Bool {
        self.clerBarButtonItem.enabled = false
        // Return NO if you do not want the specified item to be editable.
        return (0 < self.historyTerms.count)
    }
    
    // Override to support editing the table view.
    override func tableView(tableView : UITableView, commitEditingStyle editingStyle : UITableViewCellEditingStyle, forRowAtIndexPath indexPath : NSIndexPath) {
        
        if (editingStyle == UITableViewCellEditingStyle.Delete) {
            // Delete the row from the data source
            tableView.beginUpdates()
            let n : Int = self.historyTerms.count
            if (0 < n) {
                self.historyTerms.removeAtIndex(indexPath.row) // model
                tableView.deleteRowsAtIndexPaths([indexPath], withRowAnimation: UITableViewRowAnimation.Fade)
            }
            tableView.endUpdates()
            if (n == 1) {
                self.tableView.setEditing(false, animated: false)
                self.navigationItem.rightBarButtonItem?.enabled = false
                self.clerBarButtonItem.enabled = false
                tableView.reloadData()
            }
            RouteDataController.SaveToTerminalHistoryWithArray(self.historyTerms)

        } else if (editingStyle == UITableViewCellEditingStyle.Insert) {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }
    }
    
    
    override func tableView(tableView : UITableView, viewForHeaderInSection section : Int) -> UIView? {

        if (section == 0) && (self.historyTerms.count <= 0) {
            guard let cell : UITableViewCell = (tableView.dequeueReusableCellWithIdentifier("termHistoryEmptyCell")) else {
                return nil
            }
            return cell
        } else {
            return nil
        }
    }

    override func  tableView(tableView : UITableView, heightForHeaderInSection section : Int) -> CGFloat {
        if (section == 0) {
            if self.historyTerms.count  <= 0 {
                //guard let cell : UITableViewCell = tableView.dequeueReusableCellWithIdentifier("termHistoryEmptyCell") else {
                //}
                return HEADER_HEIGHT
            }
        }
        return UITableViewAutomaticDimension;
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
    
    @IBAction func clearButtonAction(sender: AnyObject) {

        RouteDataController.SaveToTerminalHistoryWithArray([])
        self.historyTerms.removeAll(keepCapacity: false)

//        self.tableView.reloadSections(NSIndexSet(index: 0), withRowAnimation: UITableViewRowAnimation.Fade)
        
        self.navigationItem.rightBarButtonItem!.enabled = false
        self.clerBarButtonItem.enabled = false

        self.tableView.reloadData() // ないとヘッダ文字が表示されない
        self.tableView.reloadSectionIndexTitles()
    }
    

    // MARK: - Navigation
    
    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {

        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        
        if (segue.identifier == "doneTerminalHistorySegue") {
            let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
            if let idx : Int = self.tableView.indexPathForSelectedRow?.row {
                if idx < self.historyTerms.count {
                    let station : String  = self.historyTerms[idx]
                    apd.selectTerminalId = RouteDataController.GetStationId(station)
                } else {
                    apd.selectTerminalId = 0
                }
            } else {
                apd.selectTerminalId = 0
            }
        }
    }
    
    //      Perform Segue (Select tableview row)
    //
    //
    override func shouldPerformSegueWithIdentifier(identifier : String, sender : AnyObject?) -> Bool {
        
        if identifier  == "doneTerminalHistorySegue" {
            if true == self.clerBarButtonItem.enabled {
                // 編集中は選択は無視する（何もしない）
                return false
            }
        }
        return true
    }
}
