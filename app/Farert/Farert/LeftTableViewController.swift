//
//  LeftTableViewController.swift
//  Farert
//
//  Created by TAKEDA, Noriyuki on 2017/04/13.
//  Copyright © 2017年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit


protocol LeftMenuProtocol : class {
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
    @IBOutlet weak var clearButton: UIButton!
    @IBOutlet weak var editButton: UIButton!
    @IBOutlet weak var appendButton: UIButton!

    
    var routeFolder : Routefolder! = Routefolder()
    var mainViewController: UIViewController!
 
    var delegate: MainTableViewControllerDelegate?
    
    let aggregate_label = [ "普通運賃", "小児運賃", "往復運賃", "株割運賃", "株4割運賃",
                            "学割運賃", "学割往復" ]
    
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
        self.clearButton.isHidden = true
        
        self.routeFolder.load()

        self.tableView.reloadData()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    override func viewWillAppear(_ animated: Bool) {

        super.viewWillAppear(animated)
        
        updateAggregate()
        
        if (delegate != nil) {
            appendButton.isHidden = (delegate?.getRoute() != nil) ? false : true
        }
        self.editButton.isHidden = self.routeFolder.count() <= 0
        if let indexPathForSelectedRow = tableView.indexPathForSelectedRow {
            tableView.deselectRow(at: indexPathForSelectedRow, animated: true)
        }
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        ///self.imageHeaderView.frame = CGRect(x: 0, y: 0, width: self.view.frame.width, height: 160)
        self.view.layoutIfNeeded()
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
            self.clearButton.isHidden = !self.tableView.isEditing
            updateAggregate()
            self.tableView.reloadData()
        }
    }

    // [clear] button in edit mode
    @IBAction func actionClearButton(_ sender: Any) {
        if !self.tableView.isEditing {
            return
        }

        // iOS8
        let ac : UIAlertController = UIAlertController(title: "全消去します", message: "元に戻せませんがよろしいですか？", preferredStyle: .actionSheet)
        ac.addAction(UIAlertAction(title: "はい", style: .default, handler: { (action: UIAlertAction!) in
            self.routeFolder.removeAll()
            self.updateAggregate()
            self.editButton.isHidden = true
            self.clearButton.isHidden = true
            self.tableView.setEditing(false, animated: true)
            self.editButton.setTitle("編集", for: .normal)
            self.tableView.reloadData()
            }))
        ac.addAction(UIAlertAction(title: "いいえ", style: .default))

        // for iPad
        ac.modalPresentationStyle = UIModalPresentationStyle.popover
        //ac.popoverPresentationController?.barButtonItem = clearButton as? UIBarButtonItem
        ac.popoverPresentationController?.sourceView = self.clearButton
        // end of for iPad
        
        self.present(ac, animated: true, completion: nil)
    }

    // [Edit] button toggle the Edit / non-edit mode
    @IBAction func actionEditButton(_ sender: Any) {
        if let editBtn = sender as? UIButton {
            // in Edit mode
            if editBtn.titleLabel?.text == "編集" {
                self.clearButton.isHidden = false
                self.tableView.setEditing(true, animated: true)
                editBtn.setTitle("完了", for: .normal)
            } else {
            // in Non-Edit mode
                self.clearButton.isHidden = true
                self.tableView.setEditing(false, animated: true)
                editBtn.setTitle("編集", for: .normal)
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
        
        McPicker.show(data: [aggregate_label], doneHandler: { selections in
            if let sel : String = selections[0] {
                if let bidx : Int = self.aggregate_label.index(of: sel) {
                    self.routeFolder.setAggregateType(index: row, aggr: bidx)
                    //*cell.aggregateType.setTitle(sel, for: .normal) // self.tableView.reloadData()
                    btn.setTitle(sel, for: .normal) // self.tableView.reloadData()
                    self.updateAggregate()
                    self.tableView.reloadData()
                }
            }
        })
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
            cell.itemFare.text = self.routeFolder.routeItemFare(index: indexPath.row)
            cell.aggregateType.setTitle(aggregate_label[bi], for: .normal)
            cell.aggregateType.tag = indexPath.row
        }
        return cell
    }
    
    func tableView(_ tableView: UITableView, canEditRowAt indexPath: IndexPath) -> Bool {
        // Return NO if you do not want the specified item to be editable.
        return 0 < self.routeFolder.count()
    }
    
    // When delete or append cell
    func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCellEditingStyle, forRowAt  indexPath: IndexPath) {
        
        if editingStyle == UITableViewCellEditingStyle.delete {
            self.routeFolder.remove(index: indexPath.row)
            
            tableView.deleteRows(at: [indexPath], with: UITableViewRowAnimation.fade)
            
            if self.routeFolder.count() <= 0 {
                self.editButton.isHidden = true
                self.clearButton.isHidden = true
                self.tableView.setEditing(false, animated: true)
                self.editButton.setTitle("編集", for: .normal)
            }
            updateAggregate()

        } else if (editingStyle == UITableViewCellEditingStyle.insert) {
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

    func tableView(_ tableView: UITableView, editingStyleForRowAt indexPath: IndexPath) -> UITableViewCellEditingStyle {
        // Disable swipe to delete, but still have delete in edit mode.
        if self.tableView.isEditing {
            return .delete
        }
        return .none
    }
    
    /*
    
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
***/
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
}
