//
//  ArchiveRouteTableViewController.swift
//  iFarert
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

        self.navigationController?.toolbarHidden = false

        // Uncomment the following line to preserve selection between presentations.
        self.clearsSelectionOnViewWillAppear = false;
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        self.navigationItem.rightBarButtonItem = self.editButtonItem()
        
        self.tableView.estimatedRowHeight = 44.0
        self.tableView.rowHeight = UITableViewAutomaticDimension // for iOS8

        // Straged route read from filesystem
        self.routeList = RouteDataController.loadStrageRoute() as! [String]
        
        if !self.currentRouteString.isEmpty {
            if let idx = find(self.routeList, self.currentRouteString) {      // is exist ?
                self.routeList.removeAtIndex(idx)
                self.routeList.insert(self.currentRouteString, atIndex: 0)  // 入れ替え
                RouteDataController.saveToRouteArray(self.routeList)   // して保存
                self.saveButton.enabled = false
                self.saved = true
            } else {                                            // non exist
                routeList.insert(self.currentRouteString, atIndex: 0)
                saveButton.enabled = true
                self.saved = false
            }
        } else {
            saveButton.enabled = false
            self.saved = true
        }
        if self.routeList.count <= 0 {
            self.navigationItem.rightBarButtonItem?.enabled = false
        }
        allClearButton.enabled = false
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
        // #warning Potentially incomplete method implementation.
        // Return the number of sections.
        return 1
    }

    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if section == 0 {
            // Return the number of rows in the section.
            return self.routeList.count
        } else {
            return 0
        }
    }

    
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {

        let cell : RouteListTableViewCell = tableView.dequeueReusableCellWithIdentifier("routeListCell", forIndexPath: indexPath) as! RouteListTableViewCell

        if indexPath.row < self.routeList.count {
            //cell.routeString?.font = UIFont.preferredFontForTextStyle(FA_STYLE_ROUTE_FONT)

            if (indexPath.row == 0) && (self.routeList[0] == currentRouteString) {
                if (self.saved) {
                    cell.routeString?.textColor = UIColor.darkGrayColor()
                } else {
                    cell.routeString?.textColor = UIColor.redColor()
                }
            } else {
                cell.routeString?.textColor = UIColor.blackColor()
            }
            cell.routeString.text = self.routeList[indexPath.row]
        }
        return cell
    }
    

    // Override to support conditional editing of the table view.
    override func tableView(tableView: UITableView, canEditRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        // Return NO if you do not want the specified item to be editable.
        return 0 < self.routeList.count
    }

    
    // Override to support editing the table view.
    override func tableView(tableView: UITableView, commitEditingStyle editingStyle: UITableViewCellEditingStyle, forRowAtIndexPath indexPath: NSIndexPath) {
    
        if editingStyle == UITableViewCellEditingStyle.Delete {
            tableView.beginUpdates()
            let c : Int = self.routeList.count
            if (0 < c) {
                // Delete the row from the data source
                self.routeList.removeAtIndex(indexPath.row)       // model
                tableView.deleteRowsAtIndexPaths([indexPath], withRowAnimation: UITableViewRowAnimation.Fade)
            }
            tableView.endUpdates()
            if (c == 1) {   // last deleted
                self.tableView.setEditing(true, animated: true)
                self.navigationItem.rightBarButtonItem?.enabled = false
                self.allClearButton.enabled = false
                self.saveButton.enabled = false
                self.tableView.reloadData()
            }
            RouteDataController.saveToRouteArray(self.routeList)
            
        } else if (editingStyle == UITableViewCellEditingStyle.Insert) {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }
    }

    /*
    // Override to support rearranging the table view.
    override func tableView(tableView: UITableView, moveRowAtIndexPath fromIndexPath: NSIndexPath, toIndexPath: NSIndexPath) {

    }
    */
    override func tableView(tableView: UITableView, editingStyleForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCellEditingStyle {
        self.allClearButton.enabled = true
        return UITableViewCellEditingStyle.Delete
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
    override func tableView(tableView : UITableView, heightForRowAtIndexPath indexPath : NSIndexPath) -> CGFloat {

        if self.routeList.count <= 0 {
            return super.tableView(tableView, heightForRowAtIndexPath: indexPath)
        }
        let cell : RouteListTableViewCell = tableView.dequeueReusableCellWithIdentifier("routeListCell") as! RouteListTableViewCell
        return cell.heightForTitle(self.routeList[indexPath.row])
    }
    
    //  Header
    //
    //
    override func tableView(tableView : UITableView, viewForHeaderInSection section : Int) -> UIView? {

        if (section == 0) && (self.routeList.count <= 0) {
            let cell : UITableViewCell = tableView.dequeueReusableCellWithIdentifier("routeListEmptyCell") as! UITableViewCell
            return cell
//            return cell.contentView
        } else {
            return nil
        }
    }
    
    //  Height Header for Empty
    //
    //
    override func tableView(tableView : UITableView, heightForHeaderInSection section : NSInteger) -> CGFloat {
        if (section == 0) {
            if (self.routeList.count <= 0) {
                let cell : UITableViewCell = tableView.dequeueReusableCellWithIdentifier("routeListEmptyCell") as! UITableViewCell
                //return cell.contentView.bounds.size.height
                return 44.0
            }
        }
        return UITableViewAutomaticDimension
    }
    

    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        if segue.identifier == "unwindArchiveRouteSelectSegue" {
            // セル選択
            if let selidx = self.tableView.indexPathForSelectedRow()?.row {
                let str : String = self.routeList[selidx]
                self.selectRouteString = str.stringByTrimmingCharactersInSet(NSCharacterSet.whitespaceAndNewlineCharacterSet())
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
    override func shouldPerformSegueWithIdentifier(identifier : String?, sender : AnyObject?) -> Bool {

        if identifier  == "unwindArchiveRouteSelectSegue" {
            if self.saved || (self.tableView.indexPathForSelectedRow()?.row == 0) {
                return true
            } else {

                if nil != objc_getClass("UIAlertController") {
                    // iOS8
                    let ac : UIAlertController = UIAlertController(title: "経路は保存されていません.", message: nil, preferredStyle: .ActionSheet)
                    ac.addAction(UIAlertAction(title: "選択した経路を表示", style: .Default, handler: { (action) in self.actionSelectProc(0, Tag: self.TAG_UIACTIONSHEET_SELECTFUNC)}))
                    ac.addAction(UIAlertAction(title: "経路選択画面へ元の経路を保持したまま戻る", style: .Default, handler: { (action) in self.actionSelectProc(1, Tag: self.TAG_UIACTIONSHEET_SELECTFUNC)}))
                    ac.addAction(UIAlertAction(title: "キャンセル", style: UIAlertActionStyle.Cancel, handler:  { (action) in self.actionSelectProc(2, Tag: self.TAG_UIACTIONSHEET_SELECTFUNC)}))
                    // for iPad
                    ac.modalPresentationStyle = UIModalPresentationStyle.Popover
                    ac.popoverPresentationController?.sourceRect = self.tableView.rectForRowAtIndexPath(self.tableView.indexPathForSelectedRow()!)
                    ac.popoverPresentationController?.sourceView = self.view
                    // end of for iPad
                    self.presentViewController(ac, animated: true, completion: nil)
                    
                } else {
                    // iOS7
                    let actsheet : UIActionSheet = UIActionSheet()
                    
                    actsheet.delegate = self
                    actsheet.title = "経路は保存されていません."

                    actsheet.addButtonWithTitle("選択した経路を表示")
                    actsheet.addButtonWithTitle("経路選択画面に元の経路のまま戻る")
                    actsheet.addButtonWithTitle("キャンセル")
                    actsheet.cancelButtonIndex = actsheet.numberOfButtons - 1
                    actsheet.tag = TAG_UIACTIONSHEET_SELECTFUNC
                    
                    if UIDevice.currentDevice().userInterfaceIdiom == .Pad {
                        self.clearsSelectionOnViewWillAppear = false
                        self.preferredContentSize = CGSize(width: self.navigationController!.view!.frame.width/2, height: self.view!.frame.height)
                        actsheet.showFromRect(self.tableView.rectForRowAtIndexPath(self.tableView.indexPathForSelectedRow()!), inView: self.view, animated: true)
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
    func actionSheet(actionSheet : UIActionSheet, clickedButtonAtIndex buttonIndex : Int) {
        actionSelectProc(buttonIndex, Tag: actionSheet.tag)
    }

    func actionSelectProc(selectIndex : Int, Tag tag : Int) {
        if tag == TAG_UIACTIONSHEET_SELECTFUNC {
            //    NSLog(@"Archive Action Select:%d", buttonIndex);
            // 0: update : 更新
            // 1: return : なにもしないで画面も戻る
            // 2: cancel : なにもしない
            if (selectIndex == 0) {
                self.performSegueWithIdentifier("unwindArchiveRouteSelectSegue", sender: self)
            } else if selectIndex == 1 {
                self.dismissViewControllerAnimated(true, completion: nil)
            }
            // cancel
            self.tableView.deselectRowAtIndexPath(tableView.indexPathForSelectedRow()!, animated: false)
        } else if tag == TAG_UIACTIONSHEET_QUERYCLEARALL {
            if selectIndex == 0 {
                RouteDataController.saveToRouteArray([])
                self.routeList.removeAll(keepCapacity: false)
                
                //self.tableView.endUpdates()
                
                //self.tableView.reloadSections(NSIndexSet(index: 0), withRowAnimation: UITableViewRowAnimation.Fade)
                
                self.navigationItem.rightBarButtonItem?.enabled = false
                
                self.saveButton.enabled = false
                self.allClearButton.enabled = false
                self.tableView.reloadData()
                self.tableView.reloadSectionIndexTitles()
            }
        }
    }
    // MARK: - UI Action
    
    // [Clear] button
    @IBAction func allClearButtonTapped(sender: AnyObject) {
        
        if nil != objc_getClass("UIAlertController") {
            // iOS8
            let ac : UIAlertController = UIAlertController(title: "全経路を破棄してよろしいですか？", message: nil, preferredStyle: .ActionSheet)
            ac.addAction(UIAlertAction(title: "はい", style: .Default, handler: { (action: UIAlertAction!) in self.actionSelectProc(0, Tag: self.TAG_UIACTIONSHEET_QUERYCLEARALL)}))
            ac.addAction(UIAlertAction(title: "いいえ", style: .Default, handler: { (action: UIAlertAction!) in self.actionSelectProc(1, Tag: self.TAG_UIACTIONSHEET_QUERYCLEARALL)}))
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
            actsheet.title = "全経路を破棄してよろしいですか？"
            
            actsheet.addButtonWithTitle("はい")
            actsheet.addButtonWithTitle("いいえ")
            //actsheet.cancelButtonIndex = actsheet.numberOfButtons - 1
            actsheet.tag = TAG_UIACTIONSHEET_QUERYCLEARALL;
            
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
//
    }
    
    // [Save] button
    @IBAction func saveButtonTapped(sender: AnyObject) {
        RouteDataController.saveToRouteArray(self.routeList)
        self.saved = true
        self.saveButton.enabled = false
        self.tableView.reloadData()
    }
}
