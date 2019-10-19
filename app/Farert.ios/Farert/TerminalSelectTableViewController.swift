//
//  TerminalSelectTableViewController.swift
//  iFarert 発駅 or 着駅(最短経路選択時) 選択 View
//
//  Created by TAKEDA, Noriyuki on 2015/03/23.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit


class TerminalSelectTableViewController: CSTableViewController {

    @IBOutlet weak var clerBarButtonItem: UIBarButtonItem!
    @IBOutlet weak var scopeBar: UISegmentedControl!    // section of 会社、都道府県

    let SEGIDX_COMPANY = 0
    let SEGIDX_PREFECT = 1
    let SEGIDX_HISTORY = 2
    
    let HEADER_HEIGHT:CGFloat        = 44.0
    
    // MARK: Public property
    var historyTerms : [String] = []

    var termFilteredArray : [Int] = []
    var termCompanyPrefectArray : [[Int]] = cRouteUtil.getCompanyAndPrefects() as! [[Int]]

    let searchController : UISearchController! = UISearchController(searchResultsController: nil)
    
    override func viewDidLoad() {
        super.viewDidLoad()
  
        // Setup the Search Controller
        searchController.searchResultsUpdater = self
        searchController.searchBar.delegate = self
        definesPresentationContext = true
        searchController.dimsBackgroundDuringPresentation = false
        
        searchController.hidesNavigationBarDuringPresentation = true
        
        searchController.searchBar.placeholder = "駅名（よみ）入力"

        searchController.searchBar.searchBarStyle = UISearchBar.Style.prominent
        searchController.searchBar.sizeToFit()
        
        //tableView.register(UITableViewCell.self, forCellReuseIdentifier: "termSearchbarCell")
        
        // Setup the Scope Bar
        tableView.tableHeaderView = searchController.searchBar

        if let h : [String] = cRouteUtil.readFromTerminalHistory() as! [String]? {
            historyTerms = h
        }

        self.navigationController?.isToolbarHidden = false
        
        let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate

        if (apd.context == FGD.CONTEXT.AUTOROUTE_VIEW) {
            //self.navigationController.title = @"着駅指定(最短経路)";
            self.title = "着駅指定(最短経路)"
            self.navigationItem.prompt = "着駅指定(最短経路)"
        } else {
            //self.navigationController.title = @"発駅指定";
            self.title = "発駅指定"
            self.navigationItem.prompt = "発駅指定"
        }
        // Uncomment the following line to preserve selection between presentations.
        self.clearsSelectionOnViewWillAppear = true
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        self.navigationItem.rightBarButtonItem = self.editButtonItem;
        
        self.navigationItem.rightBarButtonItem!.isEnabled = false // Disable [Edit] button
        self.clerBarButtonItem.isEnabled = false    // Disable [Clear] button

        if #available(iOS 13.0, *) {
            self.view.backgroundColor = UIColor.systemBackground
        } else {
            // Fallback on earlier versions
        }
    }
  
    deinit {
        searchController.view.removeFromSuperview()
    }
    
    override func viewWillAppear(_ animated : Bool) {

        super.viewWillAppear(animated)
    
        // 最初に表示されたときに検索バーを非表示に
        //    [self.tableView setContentOffset:CGPointMake(0.0f, self.searchDisplayController.searchBar.frame.size.height)];
    
        // 戻ってきたときにセルの選択を解除
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
        // Return the number of rows in the section.

        if searchController.isActive && searchController.searchBar.text != "" {
            return self.termFilteredArray.count
        } else {
            let section = scopeBar.selectedSegmentIndex
            if section != SEGIDX_HISTORY {
                return self.termCompanyPrefectArray[section].count
            } else {
                return self.historyTerms.count
            }
        }
    }
    
    // Header view
    //
    override func tableView(_ tableView : UITableView, viewForHeaderInSection section : Int) -> UIView? {
        
        if (!searchController.isActive || searchController.searchBar.text == "") &&
            (scopeBar.selectedSegmentIndex == SEGIDX_HISTORY) && (section == 0) && (self.historyTerms.count <= 0) {

            guard let cell : UITableViewCell = (tableView.dequeueReusableCell(withIdentifier: "termHistoryEmptyCell")) else {
                    return nil
            }
            return cell
        }
        return nil
    }

    override func  tableView(_ tableView : UITableView, heightForHeaderInSection section : Int) -> CGFloat {
        if (!searchController.isActive || searchController.searchBar.text == "") &&
            (scopeBar.selectedSegmentIndex == SEGIDX_HISTORY) && (section == 0) && (self.historyTerms.count <= 0) {
                return HEADER_HEIGHT
        }
        return UITableView.automaticDimension;
    }

    // Section Header
    override func tableView(_ tableView : UITableView, titleForHeaderInSection section : Int) -> String? {
        
        if !searchController.isActive || searchController.searchBar.text == "" {
            let scope = scopeBar.selectedSegmentIndex
            switch (scope) {
            case SEGIDX_COMPANY:
                return "会社"
            case SEGIDX_PREFECT:
                return "都道府県"
            case SEGIDX_HISTORY:
                return "履歴"
            default:
                break;
            }
        } else {
            if (5 < self.termFilteredArray.count) {
                return "一致件数：\(self.termFilteredArray.count)件"
            }
        }
        return nil
    }
    
    // Each row cell text
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {

        let cell_identifier : String = "termSearchbarCell"
        var cell : UITableViewCell!
        
        if searchController.isActive && searchController.searchBar.text != "" {
            cell = tableView.dequeueReusableCell(withIdentifier: cell_identifier)
            if (cell == nil) {
                cell = UITableViewCell(style: UITableViewCell.CellStyle.value1, reuseIdentifier: cell_identifier)
                if (cell == nil) {
                    return cell
                }
            }
            let ident : Int = self.termFilteredArray[indexPath.row]
            cell.textLabel?.text = cRouteUtil.stationNameEx(ident)
            cell.detailTextLabel?.text = "\(cRouteUtil.getKanaFromStationId(ident)!)(\(cRouteUtil.prectName(byStation: ident)!))"
        } else {
            let secidx = scopeBar.selectedSegmentIndex
            if secidx != SEGIDX_HISTORY {
                cell = tableView.dequeueReusableCell(withIdentifier: "termCompanyAndPrefectCell")
                let ident : Int = self.termCompanyPrefectArray[secidx][indexPath.row]
                cell.textLabel?.text = cRouteUtil.companyOrPrefectName(ident)
            } else {
                cell = tableView.dequeueReusableCell(withIdentifier: "termHistoryCell")
                cell.textLabel?.text = self.historyTerms[indexPath.row]
            }
        }
        return cell;
    }

    // Override to support conditional editing of the table view.
    override func tableView(_ tableView : UITableView, canEditRowAt indexPath : IndexPath) -> Bool {
        historyEditEnd()
        // Return NO if you do not want the specified item to be editable.
        return (!searchController.isActive || searchController.searchBar.text == "") &&
            (SEGIDX_HISTORY == scopeBar.selectedSegmentIndex) && (0 < self.historyTerms.count)
    }
    
    // edit cell
    //  start Edit history
    override func tableView(_ tableView : UITableView, editingStyleForRowAt indexPath : IndexPath) -> UITableViewCell.EditingStyle {
        historyEditBegin()
        return UITableViewCell.EditingStyle.delete   // Enable 'delete' action
    }
    
    // Override to support editing the table view.
    override func tableView(_ tableView : UITableView, commit editingStyle : UITableViewCell.EditingStyle, forRowAt indexPath : IndexPath) {
        
        if (editingStyle == UITableViewCell.EditingStyle.delete) {
            // Delete the row from the data source
            tableView.beginUpdates()
            let n : Int = self.historyTerms.count
            if (0 < n) {
                self.historyTerms.remove(at: indexPath.row) // model
                tableView.deleteRows(at: [indexPath], with: UITableView.RowAnimation.fade)
            }
            tableView.endUpdates()
            if (n == 1) {
                self.tableView.setEditing(false, animated: false)
                self.navigationItem.rightBarButtonItem?.isEnabled = false
                historyEditEnd()
                tableView.reloadData()
            }
            cRouteUtil.saveToTerminalHistory(with: self.historyTerms)
            
        } else if (editingStyle == UITableViewCell.EditingStyle.insert) {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }
    }
    
    //  Selected Table cell
    //
    override func tableView(_ tableView : UITableView, didSelectRowAt indexPath : IndexPath) {
        
        if searchController.isActive && searchController.searchBar.text != "" {
            /* direct input */
            if indexPath.row < self.termFilteredArray.count {
                let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate
                apd.selectTerminalId = self.termFilteredArray[indexPath.row]
                self.performSegue(withIdentifier: "termSelectDone", sender: self)
            }
        } else if SEGIDX_HISTORY == scopeBar.selectedSegmentIndex {
            /* history select */
            if indexPath.row < self.historyTerms.count {
                let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate
                apd.selectTerminalId = cRouteUtil.getStationId(self.historyTerms[indexPath.row])
                self.performSegue(withIdentifier: "termSelectDone", sender: self)
            }
        } else {
            /* company or prefect select */
            // @"Select comapny or prefect");
        }
    }
    
    // MARK: - Navigation
    
    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue : UIStoryboardSegue, sender sender_ : Any?) {
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        let segid : String! = segue.identifier
        
        if segid == "termLineSegue" {
            // 都道府県または会社に属する路線一覧
            let selectLineTblViewController : SelectLineTableViewController = segue.destination as! SelectLineTableViewController
            
//            let section : Int = self.tableView.indexPathForSelectedRow?.section ?? 0
            let section = scopeBar.selectedSegmentIndex
            if (0 == section) || (1 == section) {
                let row : Int = self.tableView.indexPathForSelectedRow?.row ?? 0
                let iDs : [Int] = self.termCompanyPrefectArray[section] as [Int]
                if row < iDs.count {
                    selectLineTblViewController.companyOrPrefectId = iDs[row]
                    selectLineTblViewController.baseStationId = 0
                    selectLineTblViewController.lastLineId = 0
                }
            }
        } else if segid == "toHistorySegue" {
            // 過去指定 履歴駅一覧 (Jan.2017,4th removed)
            let dvc : CSTableViewController = segue.destination as! CSTableViewController
            dvc.transit_state = FGD.TRANSIT.HISTORY
            
        } /* else termSelectDone(unwind)(検索結果から駅選択) */
        /* else termCancelSegue(unwind)(return to Main) */
    }
    
    
    
    // MARK: - Content Filtering
    func filterContentForSearchText(_ searchText : String) {
        // Update the filtered array based on the search text and scope.
        // Remove all objects from the filtered search array
        self.termFilteredArray.removeAll(keepingCapacity: false)
        
        if (true != searchText.isEmpty) {
            self.termFilteredArray = cRouteUtil.keyMatchStations(searchText) as! [Int]
        }
        tableView.reloadData()
    }

    // MARK: - Segmented Control
    @IBAction func scopeChanged(_ sender: UISegmentedControl) {

        self.setEditing(false, animated: false)
        
        // Disable/Enable [Edit] button
        self.navigationItem.rightBarButtonItem!.isEnabled =
            ((SEGIDX_HISTORY == sender.selectedSegmentIndex) && (0 < self.historyTerms.count))
  
        self.tableView.reloadData()
        self.tableView.reloadSectionIndexTitles()
    }
    
    // [Clear] button touched
    //
    @IBAction func clearButtonAction(_ sender: Any) {
        cRouteUtil.saveToTerminalHistory(with: [])
        self.historyTerms.removeAll(keepingCapacity: false)
        
        //        self.tableView.reloadSections(NSIndexSet(index: 0), withRowAnimation: UITableViewRowAnimation.Fade)
        
        self.navigationItem.rightBarButtonItem!.isEnabled = false
        
        historyEditEnd()
    
        self.tableView.reloadData() // ないとヘッダ文字が表示されない
        self.tableView.reloadSectionIndexTitles()

    }

    //      Perform Segue (Select tableview row)
    //
    override func shouldPerformSegue(withIdentifier identifier : String, sender : Any?) -> Bool {
        
        //if true == self.clerBarButtonItem.isEnabled {
        //    // 編集中は選択は無視する（何もしない）
        //    // コメントを解除すると、履歴編集中に戻ることができなくする
        //    return false
        //}
        return true
    }
    
    func historyEditEnd() {
        self.clerBarButtonItem.isEnabled = false    // Disable [Edit] button
        self.scopeBar.isHidden = false
        self.searchController.searchBar.isHidden = false
        self.navigationItem.leftBarButtonItem?.isEnabled = true
        self.navigationItem.leftBarButtonItem?.tintColor = UIColor(red: 0.0, green: 122.0/255.0, blue: 1.0, alpha: 1.0)
    }
    
   func historyEditBegin() {
        self.clerBarButtonItem.isEnabled = true // Enable [Edit] button
        self.searchController.searchBar.isHidden = true
        self.navigationItem.leftBarButtonItem?.isEnabled = false
        self.navigationItem.leftBarButtonItem?.tintColor = UIColor.white // hide
        self.scopeBar.isHidden = true
    }
}

extension TerminalSelectTableViewController: UISearchBarDelegate {
    // MARK: - UISearchBar Delegate
    func searchBar(_ searchBar: UISearchBar, selectedScopeButtonIndexDidChange selectedScope: Int) {
        filterContentForSearchText(searchController.searchBar.text!)
    }
}

extension TerminalSelectTableViewController: UISearchResultsUpdating {
    // MARK: - UISearchResultsUpdating Delegate
    func updateSearchResults(for searchController: UISearchController) {
        self.setEditing(false, animated: false)

        filterContentForSearchText(searchController.searchBar.text!)
    }
}
