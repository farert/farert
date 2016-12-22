//
//  ArchiveRouteTableViewController.swift
//  iFarert Result Strage list management view
//
//  Created by TAKEDA, Noriyuki on 2015/03/23.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

class ArchiveRouteTableViewController: UITableViewController, UIActionSheetDelegate {

    let TAG_UIACTIONSHEET_QUERYCLEARALL : Int = 3940
    let TAG_UIACTIONSHEET_SELECTFUNC : Int = 1203
    
    // MARK: - public property
    
    var currentRouteString : String = ""    // in
    var selectRouteString : String = ""     // out
    
    // MARK: - private property
    var routeList : [String] = []
    var saved : Bool = false

    // MARK: - UI variables
    @IBOutlet weak var allClearButton: UIBarButtonItem!
    @IBOutlet weak var saveButton: UIBarButtonItem!
    

    // MARK: - view
    
    override func viewDidLoad() {
        super.viewDidLoad()

        self.navigationController?.isToolbarHidden = false

        // Uncomment the following line to preserve selection between presentations.
        self.clearsSelectionOnViewWillAppear = false;
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        self.navigationItem.rightBarButtonItem = self.editButtonItem
        
        self.tableView.estimatedRowHeight = 44.0
        self.tableView.rowHeight = UITableViewAutomaticDimension // for iOS8

        // Straged route read from filesystem
        self.routeList = RouteDataController.loadStrageRoute() as! [String]
        
        if !self.currentRouteString.isEmpty {
            if let idx = self.routeList.index(of: self.currentRouteString) {      // is exist ?
                self.routeList.remove(at: idx)
                self.routeList.insert(self.currentRouteString, at: 0)  // 入れ替え
                RouteDataController.save(toRouteArray: self.routeList)   // して保存
                self.saveButton.isEnabled = false
                self.saved = true
            } else {                                            // non exist
                routeList.insert(self.currentRouteString, at: 0)
                saveButton.isEnabled = true
                self.saved = false
            }
        } else {
            saveButton.isEnabled = false
            self.saved = true
        }
        if self.routeList.count <= 0 {
            self.navigationItem.rightBarButtonItem?.isEnabled = false
        }
        allClearButton.isEnabled = false
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
        // #warning Potentially incomplete method implementation.
        // Return the number of sections.
        return 1
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if section == 0 {
            // Return the number of rows in the section.
            return self.routeList.count
        } else {
            return 0
        }
    }

    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {

        let cell : RouteListTableViewCell = tableView.dequeueReusableCell(withIdentifier: "routeListCell", for: indexPath) as! RouteListTableViewCell

        if indexPath.row < self.routeList.count {
            //cell.routeString?.font = UIFont.preferredFontForTextStyle(FA_STYLE_ROUTE_FONT)

            if (indexPath.row == 0) && (self.routeList[0] == currentRouteString) {
                if (self.saved) {
                    cell.routeString?.textColor = UIColor.darkGray
                } else {
                    cell.routeString?.textColor = UIColor.red
                }
            } else {
                cell.routeString?.textColor = UIColor.black
            }
            cell.routeString.text = self.routeList[indexPath.row]
        }
        return cell
    }
    

    // Override to support conditional editing of the table view.
    override func tableView(_ tableView: UITableView, canEditRowAt indexPath: IndexPath) -> Bool {
        allClearButton.isEnabled = false
        // Return NO if you do not want the specified item to be editable.
        return 0 < self.routeList.count
    }

    
    // Override to support editing the table view.
    override func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCellEditingStyle, forRowAt indexPath: IndexPath) {
    
        if editingStyle == UITableViewCellEditingStyle.delete {
            tableView.beginUpdates()
            let c : Int = self.routeList.count
            if (0 < c) {
                // Delete the row from the data source
                self.routeList.remove(at: indexPath.row)       // model
                tableView.deleteRows(at: [indexPath], with: UITableViewRowAnimation.fade)
            }
            tableView.endUpdates()
            if (c == 1) {   // last deleted
                self.tableView.setEditing(true, animated: true)
                self.navigationItem.rightBarButtonItem?.isEnabled = false
                self.allClearButton.isEnabled = false
                self.saveButton.isEnabled = false
                self.tableView.reloadData()
            }
            RouteDataController.save(toRouteArray: self.routeList)
            
        } else if (editingStyle == UITableViewCellEditingStyle.insert) {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }
    }

    /*
    // Override to support rearranging the table view.
    override func tableView(tableView: UITableView, moveRowAtIndexPath fromIndexPath: NSIndexPath, toIndexPath: NSIndexPath) {

    }
    */
    override func tableView(_ tableView: UITableView, editingStyleForRowAt indexPath: IndexPath) -> UITableViewCellEditingStyle {
        self.allClearButton.isEnabled = true
        return UITableViewCellEditingStyle.delete
    }
    /*
    // Override to support conditional rearranging of the table view.
    override func tableView(tableView: UITableView, canMoveRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        // Return NO if you do not want the item to be re-orderable.
        return true
    }
    */

    //  Height Row
    //
    //
    override func tableView(_ tableView : UITableView, heightForRowAt indexPath : IndexPath) -> CGFloat {

        if self.routeList.count <= 0 {
            return super.tableView(tableView, heightForRowAt: indexPath)
        }
        let cell : RouteListTableViewCell = tableView.dequeueReusableCell(withIdentifier: "routeListCell") as! RouteListTableViewCell
        return cell.heightForTitle(self.routeList[indexPath.row])
    }
    
    //  Header
    //
    //
    override func tableView(_ tableView : UITableView, viewForHeaderInSection section : Int) -> UIView? {

        if (section == 0) && (self.routeList.count <= 0) {
            let cell = tableView.dequeueReusableCell(withIdentifier: "routeListEmptyCell") as UITableViewCell!
//          return cell.contentView
            return cell
        } else {
            return nil
        }
    }
    
    //  Height Header for Empty
    //
    //
    override func tableView(_ tableView : UITableView, heightForHeaderInSection section : NSInteger) -> CGFloat {
        if (section == 0) {
            if (self.routeList.count <= 0) {
                //let cell = tableView.dequeueReusableCellWithIdentifier("routeListEmptyCell") as! UITableViewCell
                //return cell.contentView.bounds.size.height
                return 44.0
            }
        }
        return UITableViewAutomaticDimension
    }
    

    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        if segue.identifier == "unwindArchiveRouteSelectSegue" {
            // セル選択
            if let selidx = self.tableView.indexPathForSelectedRow?.row {
                let str : String = self.routeList[selidx]
                self.selectRouteString = str.trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)
            } else {
                self.selectRouteString = ""
            }
        } else {
            // 閉じる(上端左側バーボタン
            //@"unwindArchiveRouteBackSegue"
        }
    }

    //      Perform Segue (Select tableview row)
    //
    //
    override func shouldPerformSegue(withIdentifier identifier : String, sender : Any?) -> Bool {

        if identifier  == "unwindArchiveRouteSelectSegue" {
            if true == self.allClearButton.isEnabled {
                // 編集中は選択は無視する（何もしない）
                return false
            }
            if self.saved || (self.tableView.indexPathForSelectedRow?.row == 0) {
                return true
            } else {

                if #available(iOS 8, OSX 10.10, *) {
    // iOS8
                    let ac : UIAlertController = UIAlertController(title: "経路は保存されていません.", message: nil, preferredStyle: .actionSheet)
                    ac.addAction(UIAlertAction(title: "選択した経路を表示", style: .default, handler: { (action) in self.actionSelectProc(0, Tag: self.TAG_UIACTIONSHEET_SELECTFUNC)}))
                    ac.addAction(UIAlertAction(title: "経路選択画面へ元の経路を保持したまま戻る", style: .default, handler: { (action) in self.actionSelectProc(1, Tag: self.TAG_UIACTIONSHEET_SELECTFUNC)}))
                    ac.addAction(UIAlertAction(title: "キャンセル", style: UIAlertActionStyle.cancel, handler:  { (action) in self.actionSelectProc(2, Tag: self.TAG_UIACTIONSHEET_SELECTFUNC)}))
                    // for iPad
                    ac.modalPresentationStyle = UIModalPresentationStyle.popover
                    ac.popoverPresentationController?.sourceRect = self.tableView.rectForRow(at: self.tableView.indexPathForSelectedRow!)
                    ac.popoverPresentationController?.sourceView = self.view
                    // end of for iPad
                    self.present(ac, animated: true, completion: nil)
                    
                } else {
                    // iOS7
                    let actsheet : UIActionSheet = UIActionSheet()
                    
                    actsheet.delegate = self
                    actsheet.title = "経路は保存されていません."

                    actsheet.addButton(withTitle: "選択した経路を表示")
                    actsheet.addButton(withTitle: "経路選択画面に元の経路のまま戻る")
                    actsheet.addButton(withTitle: "キャンセル")
                    actsheet.cancelButtonIndex = actsheet.numberOfButtons - 1
                    actsheet.tag = TAG_UIACTIONSHEET_SELECTFUNC
                    
                    if UIDevice.current.userInterfaceIdiom == .pad {
                        self.clearsSelectionOnViewWillAppear = false
                        self.preferredContentSize = CGSize(width: self.navigationController!.view!.frame.width/2, height: self.view!.frame.height)
                        actsheet.show(from: self.tableView.rectForRow(at: self.tableView.indexPathForSelectedRow!), in: self.view, animated: true)
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
                return false
            }
        } else {
            // unwindArchiveRouteBackSegue
        }
        return true
    }
    
    //  On select Action sheet.
    //
    func actionSheet(_ actionSheet : UIActionSheet, clickedButtonAt buttonIndex : Int) {
        actionSelectProc(buttonIndex, Tag: actionSheet.tag)
    }

    func actionSelectProc(_ selectIndex : Int, Tag tag : Int) {
        if tag == TAG_UIACTIONSHEET_SELECTFUNC {
            //    NSLog(@"Archive Action Select:%d", buttonIndex);
            // 0: update : 更新
            // 1: return : なにもしないで画面も戻る
            // 2: cancel : なにもしない
            if (selectIndex == 0) {
                self.performSegue(withIdentifier: "unwindArchiveRouteSelectSegue", sender: self)
            } else if selectIndex == 1 {
                self.dismiss(animated: true, completion: nil)
            }
            // cancel
            self.tableView.deselectRow(at: tableView.indexPathForSelectedRow!, animated: false)
        } else if tag == TAG_UIACTIONSHEET_QUERYCLEARALL {
            if selectIndex == 0 {
                RouteDataController.save(toRouteArray: [])
                self.routeList.removeAll(keepingCapacity: false)
                
                //self.tableView.endUpdates()
                
                //self.tableView.reloadSections(NSIndexSet(index: 0), withRowAnimation: UITableViewRowAnimation.Fade)
                
                self.navigationItem.rightBarButtonItem?.isEnabled = false
                
                self.saveButton.isEnabled = false
                self.allClearButton.isEnabled = false
                self.tableView.reloadData()
                self.tableView.reloadSectionIndexTitles()
            }
        }
    }
    // MARK: - UI Action
    
    // [Clear] button
    @IBAction func allClearButtonTapped(_ sender: AnyObject) {
        
        if #available(iOS 8, OSX 10.10, *) {
            // iOS8
            let ac : UIAlertController = UIAlertController(title: "全経路を破棄してよろしいですか？", message: nil, preferredStyle: .actionSheet)
            ac.addAction(UIAlertAction(title: "はい", style: .default, handler: { (action: UIAlertAction) in self.actionSelectProc(0, Tag: self.TAG_UIACTIONSHEET_QUERYCLEARALL)}))
            ac.addAction(UIAlertAction(title: "いいえ", style: .default, handler: { (action: UIAlertAction) in self.actionSelectProc(1, Tag: self.TAG_UIACTIONSHEET_QUERYCLEARALL)}))
            // for iPad
            ac.modalPresentationStyle = UIModalPresentationStyle.popover
            ac.popoverPresentationController?.barButtonItem = sender as? UIBarButtonItem
            ac.popoverPresentationController?.sourceView = self.view
            // end of for iPad
            self.present(ac, animated: true, completion: nil)
            
        } else {
            // iOS7
            let actsheet : UIActionSheet = UIActionSheet()
            
            actsheet.delegate = self
            actsheet.title = "全経路を破棄してよろしいですか？"
            
            actsheet.addButton(withTitle: "はい")
            actsheet.addButton(withTitle: "いいえ")
            //actsheet.cancelButtonIndex = actsheet.numberOfButtons - 1
            actsheet.tag = TAG_UIACTIONSHEET_QUERYCLEARALL;
            
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
//
    }
    
    // [Save] button
    @IBAction func saveButtonTapped(_ sender: AnyObject) {
        RouteDataController.save(toRouteArray: self.routeList)
        self.saved = true
        self.saveButton.isEnabled = false
        self.tableView.reloadData()
    }
}
