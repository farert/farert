//
//  LeftTableViewController.swift
//  Farert
//
//  Created by TAKEDA, Noriyuki on 2017/04/13.
//  Copyright © 2017年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit


protocol LeftMenuProtocol : AnyObject {
    func changeViewController()
}

public protocol MainTableViewControllerDelegate {
    func getRoute() -> cRouteList?
    func changeRoute(route : cRouteList)
}

class LeftTableViewController: UIViewController, UITableViewDataSource, UITableViewDelegate
{
    @IBOutlet weak var tableView: UITableView!
    @IBOutlet weak var totalFare: UILabel!
    @IBOutlet weak var totalSalesKm: UILabel!
    @IBOutlet weak var clearOrExportButton: UIButton!
    @IBOutlet weak var editButton: UIButton!
    @IBOutlet weak var appendButton: UIButton!
    
    var routeFolder : Routefolder! = Routefolder()
    var mainViewController: UIViewController!
 
    var delegate: MainTableViewControllerDelegate?
    
    // MARK: Method
  
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()

        self.tableView.separatorColor = UIColor(red: 224/255, green: 224/255, blue: 224/255, alpha: 1.0)

        // Uncomment the following line to preserve selection between presentations
        //self.clearsSelectionOnViewWillAppear = false

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        //self.navigationItem.leftBarButtonItem = self.editButtonItem
        
        self.tableView.dataSource = self
        self.tableView.delegate = self
        
        self.editButton.isHidden = true
        self.clearOrExportButton.isHidden = true
        
        self.reload()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    override func viewWillAppear(_ animated: Bool) {

        super.viewWillAppear(animated)
        
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        ///self.imageHeaderView.frame = CGRect(x: 0, y: 0, width: self.view.frame.width, height: 160)
        self.view.layoutIfNeeded()
    }
    
    override func viewDidAppear(_ animated: Bool) {
        updateAggregate()
        
        if let deleg = delegate {
            appendButton.isHidden = (deleg.getRoute() != nil) ? false : true
        } else {
            appendButton.isHidden = true
        }
        changeToExportButton(true)
        if (self.routeFolder.count() <= 0) {
            self.editButton.isHidden = true
            self.clearOrExportButton.isHidden = true
        } else {
            self.editButton.isHidden = false
            self.clearOrExportButton.isHidden = false
        }
        if let indexPathForSelectedRow = tableView.indexPathForSelectedRow {
            tableView.deselectRow(at: indexPathForSelectedRow, animated: true)
        }
    }
    
    // MARK: helper method
    
    func reload() {
        self.routeFolder.load()
        updateAggregate()
        self.tableView.reloadData()
    }
    
    // MARK: Delegate method

    func changeViewController() {
        self.slideMenuController()?.changeMainViewController(self.mainViewController, close: true)
    }
    
    // MARK: UI action
    
    
    // Append the Route
    @IBAction func actionAddButton(_ sender: UIButton) {
 
        if let route = delegate?.getRoute() {
            if !routeFolder.add(item: route) {
                maxAddFailure()
                return
            }
            self.editButton.isHidden = false
            self.clearOrExportButton.isHidden = false
            if (self.tableView.isEditing) {
                self.changeToExportButton(false) // all clear
                self.editButton.setTitle("完了", for: .normal)
            } else {
                self.changeToExportButton(true) // export
                self.editButton.setTitle("編集", for: .normal)
            }
            updateAggregate()
            self.tableView.reloadData()
        }
    }

    // [clear] button in edit mode
    @IBAction func actionClearButton(_ sender: Any) {
        if !self.tableView.isEditing {
            // export
            if (self.routeFolder.count() <= 0) {
                return
            }
            
            self.ShowAirDrop(sender as AnyObject, text: self.routeFolder.makeExportText())
            
            return
        }
        
        // remove all
    
        // iOS8
        let ac : UIAlertController = UIAlertController(title: "全消去します", message: "元に戻せませんがよろしいですか？", preferredStyle: .actionSheet)
        ac.addAction(UIAlertAction(title: "はい", style: .default, handler: { (action: UIAlertAction!) in
            self.routeFolder.removeAll()
            self.updateAggregate()
            self.editButton.isHidden = true
            self.clearOrExportButton.isHidden = true
            self.tableView.setEditing(false, animated: true)
            self.editButton.setTitle("編集", for: .normal)
            self.tableView.reloadData()
            }))
        ac.addAction(UIAlertAction(title: "いいえ", style: .default))

        // for iPad
        ac.modalPresentationStyle = UIModalPresentationStyle.popover
        //ac.popoverPresentationController?.barButtonItem = clearButton as? UIBarButtonItem
        ac.popoverPresentationController?.sourceView = self.clearOrExportButton
        // end of for iPad
        
        self.present(ac, animated: true, completion: nil)
    }

    // [Edit] button toggle the Edit / non-edit mode
    @IBAction func actionEditButton(_ sender: Any) {
        if let editBtn = sender as? UIButton {
            // in Edit mode
            if editBtn.titleLabel?.text == "編集" {
                self.tableView.setEditing(true, animated: true)
                editBtn.setTitle("完了", for: .normal)
                changeToExportButton(false)
            } else {
            // in Non-Edit mode
                self.tableView.setEditing(false, animated: true)
                editBtn.setTitle("編集", for: .normal)
                changeToExportButton(true)
            }
        } else {
            assert(false)
        }        
    }
    @IBAction func actionAggregateType(_ sender: Any) {
        let btn = sender as! UIButton
        //*let cell : HolderTableViewCell = btn.superview?.superview as! HolderTableViewCell
        //*let row : Int = (tableView.indexPath(for: cell)?.row)!
        let row : Int = btn.tag
 
        if #available(iOS 9.0, *) {
            let mcPicker = McPicker(data: [aggregate_label])
            let customLabel = UILabel()
            customLabel.textAlignment = .center
            customLabel.textColor = .white
            mcPicker.label = customLabel
            mcPicker.toolbarButtonsColor = .white
            mcPicker.toolbarBarTintColor = .darkGray
            mcPicker.pickerBackgroundColor = .gray
            
            mcPicker.show() { selections in
                if let sel : String = selections[0] {
                    if let bidx : Int = aggregate_label.firstIndex(of: sel) {
                        self.routeFolder.setAggregateType(index: row, aggr: bidx)
                        //*cell.aggregateType.setTitle(sel, for: .normal) // self.tableView.reloadData()
                        btn.setTitle(sel, for: .normal) // self.tableView.reloadData()
                        self.updateAggregate()
                        self.tableView.reloadData()
                    }
                }
            }
            
        } else {
            /* iOS8 */
            let ac : UIAlertController = UIAlertController(title: "運賃種別", message: nil, preferredStyle: .actionSheet)
            for item in aggregate_label {
                ac.addAction(UIAlertAction(title: item, style: .default, handler: { (action: UIAlertAction!) -> Void in
                    if let bidx : Int = aggregate_label.firstIndex(of: item) {
                        self.routeFolder.setAggregateType(index: row, aggr: bidx)
                        //*cell.aggregateType.setTitle(sel, for: .normal) // self.tableView.reloadData()
                        btn.setTitle(item, for: .normal) // self.tableView.reloadData()
                        self.updateAggregate()
                        self.tableView.reloadData()
                    }
                }))
            }
            ac.addAction(UIAlertAction(title: "キャンセル", style: .cancel, handler: nil))
            // for iPad
            ac.modalPresentationStyle = UIModalPresentationStyle.popover
            //ac.popoverPresentationController?.barButtonItem = (sender as! UIBarButtonItem)
            ac.popoverPresentationController?.sourceView = self.view
            // end of for iPad
            self.present(ac, animated: true, completion: nil)
            
        }
        // '//*' also is OK
        // don't come here....
    }

    
    // MARK: - tabe view
    
    ///func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
    ///    indexPath.row
    ///    return 0
    ///}
    
    // Selected cell
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        delegate?.changeRoute(route: self.routeFolder.routeItem(index: indexPath.row))
        self.changeViewController()
    }
    
    //func scrollViewDidScroll(_ scrollView: UIScrollView) {
    //    if self.tableView == scrollView {
    //
    //    }
    //}


/*
    func tableView(_ tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        guard let cell = self.tableView.dequeueReusableCell(withIdentifier: "holderHeaderCell") else {
            return nil
        }
        return cell
    }

    func  tableView(_ tableView : UITableView, heightForHeaderInSection section : Int) -> CGFloat {
        guard let cell = self.tableView.dequeueReusableCell(withIdentifier: "holderHeaderCell") else {
            return 100.0
        }
        return cell.contentView.frame.height
    }
*/    
    func numberOfSections(in tableView: UITableView) -> Int {
        // #warning Potentially incomplete method implementation.
        // Return the number of sections.
        return 1
    }

    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if section == 0 {
            return self.routeFolder.count()
        } else {
            return 0
        }
    }
    

    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        var cell : HolderTableViewCell
        
        cell = self.tableView.dequeueReusableCell(withIdentifier: "holderCell", for: indexPath) as! HolderTableViewCell
        
        if indexPath.row < routeFolder.count() {
            let rt : cRouteList = self.routeFolder.routeItem(index: indexPath.row)
            let bi : Int = self.routeFolder.aggregateType(index: indexPath.row)
            
            let begin : Int = rt.startStationId()
            let arrive : Int = rt.lastStationId()
            cell.holderItemTitle.text = "\(cRouteUtil.terminalName(begin)!) → \(cRouteUtil.terminalName(arrive)!)"
            let item_fare = self.routeFolder.routeItemFare(index: indexPath.row)
            cell.itemFare.text = item_fare.fare
            cell.aggregateType.setTitle(aggregate_label[bi], for: .normal)
            cell.aggregateType.tag = indexPath.row
            cell.itemSalesKm.text = item_fare.salesKm
        }
        return cell
    }
    
    func tableView(_ tableView: UITableView, canEditRowAt indexPath: IndexPath) -> Bool {
        // Return NO if you do not want the specified item to be editable.
        return 0 < self.routeFolder.count()
    }
    
    // When delete or append cell
    func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCell.EditingStyle, forRowAt  indexPath: IndexPath) {
        
        if editingStyle == UITableViewCell.EditingStyle.delete {
            self.routeFolder.remove(index: indexPath.row)
            
            tableView.deleteRows(at: [indexPath], with: UITableView.RowAnimation.fade)
            
            if self.routeFolder.count() <= 0 {
                self.editButton.isHidden = true
                self.clearOrExportButton.isHidden = true
                self.tableView.setEditing(false, animated: true)
                self.editButton.setTitle("編集", for: .normal)
            }
            updateAggregate()

        } else if (editingStyle == UITableViewCell.EditingStyle.insert) {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }
    }

    
    // Enable Re-order cell
    func tableView(_ tableView: UITableView, canMoveRowAt indexPath: IndexPath) -> Bool {
         return 1 < self.routeFolder.count()
    }
    
    func tableView(_ tableView: UITableView, moveRowAt sourceIndexPath: IndexPath, to destinationIndexPath: IndexPath) {
        self.routeFolder.exchange(index_source: sourceIndexPath.row, index_destination: destinationIndexPath.row)
    }

    func tableView(_ tableView: UITableView, editingStyleForRowAt indexPath: IndexPath) -> UITableViewCell.EditingStyle {
        // Disable swipe to delete, but still have delete in edit mode.
        if self.tableView.isEditing {
            return .delete
        }
        return .none
    }
    
    func tableView(_ tableView: UITableView, targetIndexPathForMoveFromRowAt sourceIndexPath: IndexPath, toProposedIndexPath proposedDestinationIndexPath: IndexPath) -> IndexPath {
        // Prevent moving rows to different sections
        if sourceIndexPath.section != proposedDestinationIndexPath.section {
            var row = 0
            if sourceIndexPath.section < proposedDestinationIndexPath.section {
                row = tableView.numberOfRows(inSection: sourceIndexPath.section) - 1
            }
            return IndexPath(row: row, section: sourceIndexPath.section)
        }
        return proposedDestinationIndexPath
    }

    // MARK: - local function

    func maxAddFailure() {
        let ac : UIAlertController = UIAlertController(title: "申し訳ありません", message: "きっぷホルダへは最大\(Int(MAX_HOLDER))までの格納となります", preferredStyle: .alert)
        ac.addAction(UIAlertAction(title: "OK", style: .default))

        // for iPad
        ac.modalPresentationStyle = UIModalPresentationStyle.popover
        ac.popoverPresentationController?.sourceView = self.appendButton
        // end of for iPad
        
        self.present(ac, animated: true, completion: nil)
    }
    
    func updateAggregate() {
        self.totalFare.text = self.routeFolder.totalFare
        self.totalSalesKm.text = self.routeFolder.totalSalesKm
    }
    //  全消去ボタンをExportボタン兼用なので外観を切り替える
    //
    //  isExport = true   : Exportボタン
    //           = false  : 全消去ボタン
    //
    func changeToExportButton(_ isExport: Bool) {
        if (isExport) {
            let origImage = UIImage(named: "exportButton")
            let tintedImage = origImage?.withRenderingMode(.alwaysTemplate)
            self.clearOrExportButton.setImage(tintedImage, for: .normal)
            if #available(iOS 13.0, *) {
                self.clearOrExportButton.tintColor = UIColor.link
            } else {
                self.clearOrExportButton.tintColor = UIColor.systemTeal
            }
            self.clearOrExportButton.setTitle("", for: .normal)
        } else {
            self.clearOrExportButton.setImage(nil, for: UIControl.State.normal)
            self.clearOrExportButton.setTitle("全消去", for: .normal)
        }
    }
    
    
    func ShowAirDrop(_ from : AnyObject, text : String) {
        let subject : String = "きっぷホルダ"
        let shareText : String = text
        let activityItems : [AnyObject] = [shareText as AnyObject]
        let excludeActivities : [UIActivity.ActivityType] = [UIActivity.ActivityType.postToWeibo]

        let activityController : UIActivityViewController = UIActivityViewController(activityItems: activityItems, applicationActivities: nil)

        // 除外サービスを指定
        activityController.excludedActivityTypes = excludeActivities

        activityController.setValue(subject, forKey: "subject")

        if #available(iOS 8, OSX 10.10, *) {            // for iPad(8.3)
            activityController.popoverPresentationController?.sourceView = self.view

        }
        // modalで表示
        self.present(activityController, animated: true, completion: nil)
    }
}
