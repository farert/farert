//
//  TerminalHistoryViewController.swift
//  iFarert 選択履歴から駅選択 view
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
        self.navigationItem.rightBarButtonItem = self.editButtonItem
        let h : [String]? = RouteDataController.readFromTerminalHistory() as! [String]?
        if h != nil {
            self.historyTerms = h!
        } else {
            self.historyTerms = []
        }
        if self.historyTerms.count <= 0 {
            self.navigationItem.rightBarButtonItem!.isEnabled = false
        }
        self.clerBarButtonItem.isEnabled = false
    }
    
    // 戻ってきたときにセルの選択を解除
    override func viewWillAppear(_ animated : Bool) {
        super.viewWillAppear(animated)
        if let idx : IndexPath = self.tableView.indexPathForSelectedRow {
            self.tableView.deselectRow(at: idx, animated:false)
        }
    }
    
    // MARK: - Table view data source
    
    override func numberOfSections(in tableView : UITableView) -> Int {
        // Return the number of sections.
        return 1;
    }
    
    override func tableView(_ tableView : UITableView, numberOfRowsInSection section : Int) -> Int {
        if section == 0 {
            // Return the number of rows in the section.
            return self.historyTerms.count
        } else {
            return 0
        }
    }
    
    override func tableView(_ tableView : UITableView, cellForRowAt indexPath : IndexPath) -> UITableViewCell {
    
        let cell : UITableViewCell = tableView.dequeueReusableCell(withIdentifier: "termHistoryCell", for: indexPath) 

        if (indexPath.row < self.historyTerms.count) {
            cell.textLabel?.text = self.historyTerms[indexPath.row]
        }
        return cell
    }
    
    override func tableView(_ tableView : UITableView, editingStyleForRowAt indexPath : IndexPath) -> UITableViewCellEditingStyle {
        self.clerBarButtonItem.isEnabled = true
        return UITableViewCellEditingStyle.delete
    }
    
    // Override to support conditional editing of the table view.
    override func tableView(_ tableView : UITableView, canEditRowAt indexPath : IndexPath) -> Bool {
        self.clerBarButtonItem.isEnabled = false
        // Return NO if you do not want the specified item to be editable.
        return (0 < self.historyTerms.count)
    }
    
    // Override to support editing the table view.
    override func tableView(_ tableView : UITableView, commit editingStyle : UITableViewCellEditingStyle, forRowAt indexPath : IndexPath) {
        
        if (editingStyle == UITableViewCellEditingStyle.delete) {
            // Delete the row from the data source
            tableView.beginUpdates()
            let n : Int = self.historyTerms.count
            if (0 < n) {
                self.historyTerms.remove(at: indexPath.row) // model
                tableView.deleteRows(at: [indexPath], with: UITableViewRowAnimation.fade)
            }
            tableView.endUpdates()
            if (n == 1) {
                self.tableView.setEditing(false, animated: false)
                self.navigationItem.rightBarButtonItem?.isEnabled = false
                self.clerBarButtonItem.isEnabled = false
                tableView.reloadData()
            }
            RouteDataController.saveToTerminalHistory(with: self.historyTerms)

        } else if (editingStyle == UITableViewCellEditingStyle.insert) {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }
    }
    
    
    override func tableView(_ tableView : UITableView, viewForHeaderInSection section : Int) -> UIView? {

        if (section == 0) && (self.historyTerms.count <= 0) {
            guard let cell : UITableViewCell = (tableView.dequeueReusableCell(withIdentifier: "termHistoryEmptyCell")) else {
                return nil
            }
            return cell
        } else {
            return nil
        }
    }

    override func  tableView(_ tableView : UITableView, heightForHeaderInSection section : Int) -> CGFloat {
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
    
    @IBAction func clearButtonAction(_ sender: AnyObject) {

        RouteDataController.saveToTerminalHistory(with: [])
        self.historyTerms.removeAll(keepingCapacity: false)

//        self.tableView.reloadSections(NSIndexSet(index: 0), withRowAnimation: UITableViewRowAnimation.Fade)
        
        self.navigationItem.rightBarButtonItem!.isEnabled = false
        self.clerBarButtonItem.isEnabled = false

        self.tableView.reloadData() // ないとヘッダ文字が表示されない
        self.tableView.reloadSectionIndexTitles()
    }
    

    // MARK: - Navigation
    
    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {

        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        
        if (segue.identifier == "doneTerminalHistorySegue") {
            let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate
            if let idx : Int = self.tableView.indexPathForSelectedRow?.row {
                if idx < self.historyTerms.count {
                    let station : String  = self.historyTerms[idx]
                    apd.selectTerminalId = RouteDataController.getStationId(station)
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
    override func shouldPerformSegue(withIdentifier identifier : String, sender : Any?) -> Bool {
        
        if identifier  == "doneTerminalHistorySegue" {
            if true == self.clerBarButtonItem.isEnabled {
                // 編集中は選択は無視する（何もしない）
                return false
            }
        }
        return true
    }
}
