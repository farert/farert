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
    var existIndex : Int = -1

    // MARK: - UI variables
    @IBOutlet weak var allClearButton: UIBarButtonItem!
    @IBOutlet weak var saveButton: UIBarButtonItem!
    @IBOutlet weak var exportButton: UIBarButtonItem!
    
    // MARK: - view

    override func viewDidLoad() {
        super.viewDidLoad()

        self.navigationController?.isToolbarHidden = false

        // Uncomment the following line to preserve selection between presentations.
        self.clearsSelectionOnViewWillAppear = false;

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        self.navigationItem.rightBarButtonItem = self.editButtonItem

        self.tableView.estimatedRowHeight = 44.0
        self.tableView.rowHeight = UITableView.automaticDimension // for iOS8

        // Straged route read from filesystem
        self.routeList = cRouteUtil.loadStrageRoute() as! [String]

        if !self.currentRouteString.isEmpty {
            if let idx = self.routeList.firstIndex(of: self.currentRouteString) {
                // is exist ?(保存済み) a, b
                self.existIndex = idx
                if (0 < idx) {
                     // b. 居たら先頭へ持ってくる
                    self.routeList.remove(at: idx)
                    self.routeList.insert(self.currentRouteString, at: 0)
                } // else a. 居て既に先頭にある
                saveButton.isEnabled = false
            } else { // c. non exist(未保存)
                routeList.insert(self.currentRouteString, at: 0)
                saveButton.isEnabled = true // Enable 'save' button
                existIndex = -1
            }
        } else {
            // d. 現在表示・選択の経路はなし
            saveButton.isEnabled = false
            existIndex = -2         // empty
        }
        if self.routeList.count <= 0 {
            self.navigationItem.rightBarButtonItem?.isEnabled = false
        }
        allClearButton.isEnabled = false
        if #available(iOS 13.0, *) {
            self.view.backgroundColor = UIColor.systemBackground
        } else {
            // Fallback on earlier versions
        }
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
                if (existIndex != -1) {
                    cell.routeString?.textColor = UIColor.systemGray
                } else {
                    cell.routeString?.textColor = UIColor.systemRed
                }
            } else {
                if #available(iOS 13.0, *) {
                    cell.routeString?.textColor = UIColor.label
                } else {
                    cell.routeString?.textColor = UIColor.black
                }
            }
            cell.routeString.text = self.routeList[indexPath.row]
        }
        return cell
    }


    // Override to support conditional editing of the table view.
    override func tableView(_ tableView: UITableView, canEditRowAt indexPath: IndexPath) -> Bool {
        self.editEnd()
        // Return NO if you do not want the specified item to be editable.
        return 0 < self.routeList.count
    }


    // Override to support editing the table view.
    override func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCell.EditingStyle, forRowAt indexPath: IndexPath) {

        if editingStyle == UITableViewCell.EditingStyle.delete {
            tableView.beginUpdates()
            let c : Int = self.routeList.count
            if (0 < c) {
                let rmIdx = indexPath.row
                
                // Delete the row from the data source
                self.routeList.remove(at: rmIdx)       // model
                tableView.deleteRows(at: [indexPath], with: UITableView.RowAnimation.fade)

                if (existIndex == 0 || existIndex == -2) {
                    // a or d
                    if (rmIdx == 0) {
                        existIndex = -2 // same as 'd'
                    }
                    cRouteUtil.save(toRouteArray: self.routeList)
                } else if (0 < existIndex) {
                    // b
                    if (rmIdx == 0) {
                        existIndex = -2
                    } else {
                        existIndex = 0
                    }
                    cRouteUtil.save(toRouteArray: self.routeList)
                } else if (existIndex == -1) {
                    // c
                    if (rmIdx == 0) {
                        existIndex = -2
                        // no save
                    } else {
                        // c. 未保存の場合は先頭に保存候補の経路が入っているので破棄
                        let route_list = self.routeList.dropFirst()
                        cRouteUtil.save(toRouteArray: Array(route_list))
                    }
                }
            }
            tableView.endUpdates()
            if (c == 1) {   // last deleted
                self.tableView.setEditing(true, animated: true)
                self.navigationItem.rightBarButtonItem?.isEnabled = false
                self.allClearButton.isEnabled = false
                self.saveButton.isEnabled = false
                editEnd()
                self.tableView.reloadData()
            }

        } else if (editingStyle == UITableViewCell.EditingStyle.insert) {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }
    }

    override func tableView(_ tableView: UITableView, editingStyleForRowAt indexPath: IndexPath) -> UITableViewCell.EditingStyle {
        self.editBegin()
        return UITableViewCell.EditingStyle.delete
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

            let cell = tableView.dequeueReusableCell(withIdentifier: "routeListEmptyCell") as UITableViewCell?
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
        return UITableView.automaticDimension
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
            if var selidx = self.tableView.indexPathForSelectedRow?.row {
                let selstr : String = self.routeList[selidx]
                // selectRouteString is interface the parent view
                self.selectRouteString = selstr.trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)

                if (0 < selidx) {
                    if (0 < existIndex) {
                        // b. メイン画面表示経路あり+経路保存済み
                        let oldselroute = self.currentRouteString
                        self.routeList.remove(at: selidx)
                        self.routeList.remove(at: 0)
                        self.routeList.insert(oldselroute, at: (self.existIndex < selidx) ? existIndex : existIndex - 1)
                        self.routeList.insert(selstr, at: 0)
                        // 選んだ経路は先頭に移動して保存
                        cRouteUtil.save(toRouteArray: self.routeList)

                    } else {
                        if (existIndex == -1) {
                            self.routeList.remove(at: 0) // 保存されていないのでNonVolatileメモリ内と同じ状態に
                            selidx -= 1     // c 経路未保存(先頭にメイン画面の経路)
                        }
                        
                        // a 先頭にメイン画面の経路 or cで保存されてた場合
                        // d メイン画面の経路はなし

                        if (0 < selidx) {
                            self.routeList.remove(at: selidx)
                            self.routeList.insert(selstr, at: 0)
                            cRouteUtil.save(toRouteArray: self.routeList)
                        }
                    }
                } else if ((selidx == 0) && (0 < existIndex)) {
                    // b.
                    cRouteUtil.save(toRouteArray: self.routeList)
                }
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
            if ((existIndex != -1)
            || (self.tableView.indexPathForSelectedRow?.row == 0)) {
                return true
            } else {

                if #available(iOS 8, OSX 10.10, *) {
    // iOS8
                    let ac : UIAlertController = UIAlertController(title: "経路は保存されていません.", message: nil, preferredStyle: .actionSheet)
                    ac.addAction(UIAlertAction(title: "選択した経路を表示", style: .default, handler: { (action) in self.actionSelectProc(0, Tag: self.TAG_UIACTIONSHEET_SELECTFUNC)}))
                    ac.addAction(UIAlertAction(title: "経路選択画面へ元の経路を保持したまま戻る", style: .default, handler: { (action) in self.actionSelectProc(1, Tag: self.TAG_UIACTIONSHEET_SELECTFUNC)}))
                    ac.addAction(UIAlertAction(title: "キャンセル", style: UIAlertAction.Style.cancel, handler:  { (action) in self.actionSelectProc(2, Tag: self.TAG_UIACTIONSHEET_SELECTFUNC)}))
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

    // Query input
    // 
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
                cRouteUtil.save(toRouteArray: [])
                self.routeList.removeAll(keepingCapacity: false)

                //self.tableView.endUpdates()

                //self.tableView.reloadSections(NSIndexSet(index: 0), withRowAnimation: UITableViewRowAnimation.Fade)

                self.navigationItem.rightBarButtonItem?.isEnabled = false
                existIndex = -2

                self.editEnd()
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
    }

    // [Save] button
    @IBAction func saveButtonTapped(_ sender: AnyObject) {
        if (existIndex == -1) {
            // カレント経路数
            let free_archive_route = FGD.MAX_ARCHIVE_ROUTE - self.countof(Route: self.routeList)
            
            if (free_archive_route <= 0) {
                let msg = "これ以上経路は追加できません. 既存の経路をいくつか削除してください"
                let actview = UIAlertController(title: "経路保存", message: msg, preferredStyle: .alert)
                actview.addAction(UIAlertAction(title: "了解",
                                                style: .default) {
                    action in
                      self.saveButton.isEnabled = false
                })
                self.present(actview, animated: true, completion: nil)
            } else {
                cRouteUtil.save(toRouteArray: self.routeList)
                existIndex = 0      // same as 'a'
            }
        }
        self.saveButton.isEnabled = false
        self.tableView.reloadData()
    }
    func editEnd() {
        self.saveButton.isEnabled = (existIndex == -1)

        self.allClearButton.isEnabled = false    // Disable [Edit] button
        self.navigationItem.leftBarButtonItem?.isEnabled = true
        if #available(iOS 13.0, *) {
            self.navigationItem.leftBarButtonItem?.tintColor = UIColor.link
        } else {
            self.navigationItem.leftBarButtonItem?.tintColor = UIColor.systemTeal
        }
    }

    func editBegin() {
        self.saveButton.isEnabled = (existIndex == -1)
        self.allClearButton.isEnabled = true // Enable [Edit] button
        self.navigationItem.leftBarButtonItem?.isEnabled = false
        self.navigationItem.leftBarButtonItem?.tintColor = UIColor.systemGray
    }
    
    // Export button
    @IBAction func onExportButton(_ sender: Any) {
        // 文字列配列にはいった経路を、改行区切りの文字列として返す
        let route_script_all : String = cRouteUtil.script(fromRouteArray: self.routeList)
        self.ShowAirDrop(sender as AnyObject, text: route_script_all)
    }
    
    // import button
    @IBAction func onImportButton(_ sender: Any) {
        // 経路文字列を複数行得る
        // Wait ProgressDialog
        let key_prop = "import_guide"
        let sw = cRouteUtil.read(fromKey: key_prop)
        if sw == "true" {
            // hide
            doImport()
            return          // 2回以降
        }

        // インポートする経路をテキストで「メモ」等の外部エディタで作成してください
        // (スペースまたはカンマ','区切りで1行1経路)で指定します）
        // その後、クリップボードにコピーしてで本機能を実行します.
        // わからないという人は使用しないでください
        //
        let msg = NSLocalizedString("title_import", comment: "")
        let agree = NSLocalizedString("agree", comment: "")
        let hide_later = NSLocalizedString("hide_specific_import", comment: "")
        let ac = UIAlertController(title: "インポート", message: msg, preferredStyle: .alert)

        // 今後、この画面は表示せず、インポート機能を実行する
        ac.addAction( UIAlertAction(title: hide_later, style: .default) {
            action in
              cRouteUtil.save(toKey: key_prop, value:"true", sync: true)
                // 1回目はやらない
        })
        // 了解
        ac.addAction(UIAlertAction(title: agree, style: .default) {
            action in
        })
        self.present(ac, animated: true, completion: nil)
    }
    
    func ShowAirDrop(_ from : AnyObject, text : String) {
        let subject : String = "保存経路"
        let shareText : String = text
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
    
    
    // import
    func doImport() -> Void {
        // 文字列をパースして配列を作成する(エラーがあったらエラーカウントする)(経路にXX箇所不正な路線・駅を検出しました）
        // 配列へマージする(既に入っているものは削除して後ろへ追加する）
        // 配列数が上限に達したら切り捨てられる(後半XX経路が切り捨てられました）
        // 文字数単位で、60✖️100=6000までで、Overしたら経路単位で切り捨てる

        var exist_count = 0
        var new_count = 0           // new add
        var error_count = 0
        var noadd_count = 0
        var is_top_import = false
        var msg : String = ""

        // カレント経路数
        var free_archive_route = FGD.MAX_ARCHIVE_ROUTE - self.countof(Route: self.routeList)
        
        if (free_archive_route <= 0) {
            msg = "これ以上経路は追加できません. 既存の経路をいくつか削除してください"
        } else {
            // クリップボードから経由を得る
            let past = UIPasteboard.general.string
            if let scr_route = past {
                let cs = CharacterSet(charactersIn: " ,.[]")
                let route_script_list = scr_route.components(separatedBy: "\n")
                for scriptRoute : String in route_script_list {
                    let scr_route = scriptRoute.components(separatedBy: cs).filter()
                    { $0 != ""}.joined(separator: ",")
                    if (free_archive_route <= 0) {
                        noadd_count += 1
                    } else {
                        let rt : cRoute = cRoute()
                        let rc = rt.setupRoute(scr_route)
                        if (rc < 0) {
                            // error
                            error_count += 1
                        } else {
                            // OK
                            if let inIndex = self.routeList.firstIndex(of: scr_route) {
                                // already exist
                                if (inIndex == 0) {
                                    is_top_import = true
                                }
                                exist_count += 1
                            } else {
                                new_count += 1 // 追加数
                                               // 経路数
                                free_archive_route -= self.countof(Route: scr_route)
                                self.routeList.append(scr_route)
                            }
                        }
                    }
                }
            }
            var msg_trail : String?
            if (0 < noadd_count) {
                msg_trail = String(format:
                "\n上限を越えたため追加できなかった経路が%d経路あります", noadd_count)
            }
            var msg_fail : String?
            if ((0 < new_count) && (0 < error_count)) {
                msg_fail = String(format:"\n%d経路が不正書式でした.", error_count)
            }

            if (new_count <= 0) {
                if (0 < error_count) {
                    msg = "追加経路はありません(すべて不正書式でした）"
                } else if (exist_count <= 0) {
                    msg = "追加経路はありません."
                } else {
                    msg = "追加経路はありません(既に全経路あります)."
                }
            } else if (0 < exist_count) {
                msg = String(format:
                    "%d経路を追加しました(%d経路は既にあります.)",
                             new_count, exist_count)
            } else {
                msg = String(format:
                    "%d経路を追加しました",
                             new_count)
            }
            msg += msg_fail ?? ""
            msg += msg_trail ?? ""
        }
        let actview = UIAlertController(title: "インポート", message: msg, preferredStyle: .alert)

        // 了解
        let agree = NSLocalizedString("agree", comment: "")
        actview.addAction(UIAlertAction(title: agree, style: .default) {
            action in
            if (0 < new_count) {
                if ((self.existIndex == -1) && (!is_top_import)) {
                    // c. 未保存の場合は先頭に保存候補の経路が入っているので破棄
                    let route_list = self.routeList.dropFirst()
                    cRouteUtil.save(toRouteArray: Array(route_list))
                } else {
                    cRouteUtil.save(toRouteArray: self.routeList)
                }
                
                if (0 < self.existIndex) {
                    // b
                    self.existIndex = -2
                } else if ((self.existIndex == -1) && is_top_import) {
                    // c
                    self.existIndex = 0  // same as 'a'
                }
                self.tableView.reloadData()
            }
        })
        self.present(actview, animated: true, completion: nil)
    }
    
    // 経路の経路数をカウントする。
    // 東京 東海道線 大阪 なら、3を返す
    //
    func countof(Route rt : [String]) -> Int {
        var c = 0
        for r : String in rt {
            let cs = CharacterSet(charactersIn: " ,.[]<>/{}")
            let a = r.components(separatedBy: cs).filter() { $0 != "" }
            c += a.count
        }
        return c
    }
    func countof(Route rt : String) -> Int {
        return countof(Route: rt.components(separatedBy: "\n"))
    }
}
