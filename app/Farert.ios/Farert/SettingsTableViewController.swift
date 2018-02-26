//
//  SettingsTableViewController.swift
//  iFarert Option Change database source management view
//
//  Created by TAKEDA, Noriyuki on 2015/03/27.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit



class SettingsTableViewController: UITableViewController {

    // MARK: - Public property
    var selectDbId : Int = 0
    
    // MARK: - Private property
    var before_dbid_idx : Int = 0
    
    var isSameShinkanzanKokuraHakataOther : Bool = false;
    
    // MARK: - UI Propery
    @IBOutlet weak var swShinkansenKokuraHakataOther: UISwitch!
    
    @IBOutlet weak var sgmDataVer: UISegmentedControl!

    // MARK: Method
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem()
        super.viewDidLoad()
        
        // Uncomment the following line to preserve selection between presentations.
        // self.clearsSelectionOnViewWillAppear = NO;

        self.navigationController?.isToolbarHidden = false

        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem;
        
        before_dbid_idx = cRouteUtil.getDatabaseId()
        if ((before_dbid_idx < DbId.DB_MIN_ID) || (DbId.DB_MAX_ID < before_dbid_idx)) {
            before_dbid_idx = DbId.DB_MAX_ID
        }
        
        self.selectDbId = -1;   /* is no select */
        
        self.sgmDataVer.selectedSegmentIndex = before_dbid_idx - DbId.DB_MIN_ID;
        self.swShinkansenKokuraHakataOther.setOn(isSameShinkanzanKokuraHakataOther, animated: false);
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    // MARK: - Table view data source
    /*
    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        // #warning Potentially incomplete method implementation.
        // Return the number of sections.
        return 0
    }

    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        // #warning Incomplete method implementation.
        // Return the number of rows in the section.
        return 0
    }
    */
    
    override func tableView(_ tableView : UITableView, titleForHeaderInSection section : Int) -> String? {
        if (section == 0) {
            return "データソース"
        } else if (section == 1) {
            return "設定"
        }
        return nil
    }

    /*
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("reuseIdentifier", forIndexPath: indexPath) as UITableViewCell

        // Configure the cell...

        return cell
    }
    */

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


    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        if (segue.identifier == "settingsSegue") {
            var dbid : Int = self.sgmDataVer.selectedSegmentIndex + DbId.DB_MIN_ID;
            if ((dbid < DbId.DB_MIN_ID) || (DbId.DB_MAX_ID < dbid)) {
                dbid = DbId.DB_MAX_ID
            }
            if (before_dbid_idx != dbid) {
                cRouteUtil.save(toDatabaseId: dbid)
                cRouteUtil.close()
                cRouteUtil.openDatabase(dbid)
                self.selectDbId = dbid
            } else {
                self.selectDbId = -1;   /* no change */
            }
            self.isSameShinkanzanKokuraHakataOther = self.swShinkansenKokuraHakataOther.isOn;
        }
    }
}
