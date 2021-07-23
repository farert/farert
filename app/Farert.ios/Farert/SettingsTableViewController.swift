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
    var before_dbid_idx : Int = DB._MAX_ID.rawValue
    
    var isSameShinkanzanKokuraHakataOther : Bool = false;
    
    var isSaved = false
    @IBOutlet weak var btnResetInfoMessage: UIButton!
    
    // MARK: - UI Propery
    @IBOutlet weak var swShinkansenKokuraHakataOther: UISwitch!
    
    @IBOutlet weak var sgmDataVer: UISegmentedControl!

    // MARK: Method
    
    override func viewDidLoad() {

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
        
        before_dbid_idx = cRouteUtil.getDatabaseId().rawValue
        if ((before_dbid_idx < DB._MIN_ID.rawValue) || (DB._MAX_ID.rawValue < before_dbid_idx)) {
            before_dbid_idx = DB._MAX_ID.rawValue
        }
        
        self.selectDbId = -1;   /* is no select */
        self.sgmDataVer.selectedSegmentIndex = before_dbid_idx - DB._MIN_ID.rawValue;
        self.swShinkansenKokuraHakataOther.setOn(isSameShinkanzanKokuraHakataOther, animated: false);
        
        var b : Bool = false
        let keys = [ "setting_key_hide_osakakan_detour_info", "setting_key_hide_no_rule_info", "import_guide"]
        for (_, k) in keys.enumerated() {
            if (cRouteUtil.read(fromKey: k) != nil) {
                b = true
                break
            }
        }
        if b {
            // どっちか1つでも抑制されてたら復活できるようにボタン有効
            btnResetInfoMessage.isEnabled = true
            isSaved = false
        } else {
            btnResetInfoMessage.isEnabled = false
            isSaved = true
        }
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
        } else if (section == 2) {
            return "その他"
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

    @IBAction func actBtnResetInfoMessageTouched(_ sender: UIButton) {
        if !isSaved {
            let keys = [ "setting_key_hide_osakakan_detour_info", "setting_key_hide_no_rule_info", "import_guide"]
            for (_, k) in keys.enumerated() {
                cRouteUtil.save(toKey: k, value: "", sync: true)
            }
            isSaved = true
            sender.isEnabled = false
        }
    }
    
    
    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        if (segue.identifier == "settingsSegue") {
            var dbid = self.sgmDataVer.selectedSegmentIndex + DB._MIN_ID.rawValue;
            if ((dbid < DB._MIN_ID.rawValue) || (DB._MAX_ID.rawValue < dbid)) {
                dbid = DB._MAX_ID.rawValue
            }
            if (before_dbid_idx != dbid) {
                cRouteUtil.save(toDatabaseId: dbid)
                cRouteUtil.closeDatabase()
                cRouteUtil.openDatabase(DB(rawValue: dbid)!)
                self.selectDbId = dbid
            } else {
                self.selectDbId = -1;   /* no change */
            }
            let bKokuraHakataShinzai = self.swShinkansenKokuraHakataOther.isOn
            if (self.isSameShinkanzanKokuraHakataOther != bKokuraHakataShinzai) {
                self.isSameShinkanzanKokuraHakataOther = bKokuraHakataShinzai
                let tf = bKokuraHakataShinzai ? "true" : ""
                cRouteUtil.save(toKey: "kokura_hakata_shinzai", value: tf, sync: true)
            }
        }
    }
    
}
