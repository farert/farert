//
//  TerminalSelectTableViewController.swift
//  iFarert 発駅 or 着駅(最短経路選択時) 選択 View
//
//  Created by TAKEDA, Noriyuki on 2015/03/23.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit


class TerminalSelectTableViewController: CSTableViewController {

    @IBOutlet weak var scopeBar: UISegmentedControl!    // section of 会社、都道府県

    var termFilteredArray : [Int] = []
    var termCompanyPrefectArray : [[Int]] = RouteDataController.getCompanyAndPrefects() as! [[Int]]

    let searchController = UISearchController(searchResultsController: nil)

    override func viewDidLoad() {
        super.viewDidLoad()
  
        // Setup the Search Controller
        searchController.searchResultsUpdater = self
        searchController.searchBar.delegate = self
        definesPresentationContext = true
        searchController.dimsBackgroundDuringPresentation = false
        
        searchController.searchBar.placeholder = "駅名（よみ）入力"

        // Setup the Scope Bar
        tableView.tableHeaderView = searchController.searchBar

        
        self.navigationController?.isToolbarHidden = false
        
        let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate

        if (apd.context == FGD.CONTEXT_AUTOROUTE_VIEW) {
            //self.navigationController.title = @"着駅指定(最短経路)";
            self.title = "着駅指定(最短経路)"
            self.navigationItem.prompt = "着駅指定(最短経路)"
        } else {
            //self.navigationController.title = @"発駅指定";
            self.title = "発駅指定"
            self.navigationItem.prompt = "発駅指定"
        }
        // Uncomment the following line to preserve selection between presentations.
        self.clearsSelectionOnViewWillAppear = false
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem;
        
        //UIBarButtonItem *backButton = [[UIBarButtonItem alloc] initWithTitle:@"戻る" style:UIBarButtonItemStylePlain target:nil action:nil];
        //self.navigationItem.backBarButtonItem = backButton;
        //self.navigationItem.backBarButtonItem.title = @"戻る"; //これはよいけど（って効かないからだめだが）上は幅広バーになっちゃう
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
            let arys : [Int] = self.termCompanyPrefectArray[section] as [Int]
            return arys.count
        }
    }
    
    override func tableView(_ tableView : UITableView, titleForHeaderInSection section : Int) -> String? {
        
        if !searchController.isActive || searchController.searchBar.text == "" {
            let scope = scopeBar.selectedSegmentIndex
            switch (scope) {
            case 0:
                return "会社";
            case 1:
                return "都道府県";
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
    
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {

        let cell_identifier : String = "termSearchbarCell"
        var cell : UITableViewCell!
        
        if searchController.isActive && searchController.searchBar.text != "" {
            cell = tableView.dequeueReusableCell(withIdentifier: cell_identifier)
            if (cell == nil) {
                cell = UITableViewCell(style: UITableViewCellStyle.value1, reuseIdentifier: cell_identifier)
                if (cell == nil) {
                    return cell
                }
            }
            let ident : Int = self.termFilteredArray[indexPath.row]
            cell.textLabel?.text = RouteDataController.stationNameEx(ident)
            cell.detailTextLabel?.text = "\(RouteDataController.getKanaFromStationId(ident)!)(\(RouteDataController.prectName(byStation: ident)!))"
        } else {
            cell = tableView.dequeueReusableCell(withIdentifier: "termCompanyAndPrefectCell")
            let secidx = scopeBar.selectedSegmentIndex
            let ident : Int = self.termCompanyPrefectArray[secidx][indexPath.row]
            if cell != nil {
                cell.textLabel?.text = RouteDataController.companyOrPrefectName(ident)
            }
        }
        return cell;
    }
    
    /*
    // Override to support conditional editing of the table view.
    - (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
    {
    // Return NO if you do not want the specified item to be editable.
    return YES;
    }
    */
    
    /*
    // Override to support editing the table view.
    - (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
    {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
    // Delete the row from the data source
    [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
    // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }
    }
    */
    
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
            let row : Int = self.tableView.indexPathForSelectedRow?.row ?? 0
            let iDs : [Int] = self.termCompanyPrefectArray[section] as [Int]
            selectLineTblViewController.companyOrPrefectId = iDs[row]
            selectLineTblViewController.baseStationId = 0
            selectLineTblViewController.lastLineId = 0
            
        } else if segid == "toHistorySegue" {
            // 過去指定 履歴駅一覧
            let dvc : CSTableViewController = segue.destination as! CSTableViewController
            dvc.transit_state = FGD.FA_TRANSIT_HISTORY
            
        } /* else termSelectDone(unwind)(検索結果から駅選択) */
        /* else termCancelSegue(unwind)(return to Main) */
    }
    
    
    
    // MARK: - Content Filtering
    func filterContentForSearchText(_ searchText : String) {
        // Update the filtered array based on the search text and scope.
        // Remove all objects from the filtered search array
        self.termFilteredArray.removeAll(keepingCapacity: false)
        
        if (true != searchText.isEmpty) {
            self.termFilteredArray = RouteDataController.keyMatchStations(searchText) as! [Int]
        }
        tableView.reloadData()
    }


    /* --- Old version
    // MARK: - UISearchDisplayController Delegate Methods
    func searchDisplayController(_ controller : UISearchDisplayController, shouldReloadTableForSearch searchString : String?) -> Bool {
        // Tells the table data source to reload when text changes
        
        let selidx : Int = self.searchDisplayController?.searchBar.selectedScopeButtonIndex ?? 0
        var scope_str : String
        if let scope_titles : [String] self.searchDisplayController?.searchBar.scopeButtonTitles {
            scope_str = scope_titles?[selidx] ?? ""
        } else {
            scope_str = ""
        }
        if searchString != nil {
            self.filterContentForSearchText(searchString!, scope: scope_str)
        }
        // Return YES to cause the search result table view to be reloaded.

        return true
    }
    
    func searchDisplayController(_ controller : UISearchDisplayController, shouldReloadTableForSearchScope searchOption : Int) -> Bool {
        
        var scope_str : String
        if let scope_titles : [AnyObject]? = self.searchDisplayController?.searchBar.scopeButtonTitles as [AnyObject]?? {
            scope_str = scope_titles![searchOption] as! String
        } else {
            scope_str = ""
        }
        let searchText : String = self.searchDisplayController?.searchBar.text ?? ""

        // Tells the table data source to reload when scope bar selection changes
        self.filterContentForSearchText(searchText, scope: scope_str)
        
        // Return YES to cause the search result table view to be reloaded.
        return true
    }
    ---- old version */
    
    override func tableView(_ tableView : UITableView, didSelectRowAt indexPath : IndexPath) {
        
        if searchController.isActive && searchController.searchBar.text != "" {

//                if ([self.delegate respondsToSelector:@selector(terminalSelectTablebleViewDone:)]) {
//                [self.delegate terminalSelectTablebleViewDone:[[termFilteredArray objectAtIndex:[indexPath row]] intValue]];
//                }
//                self.terminalId = [NSNumber numberWithInt:indexPath.row];
//                [self dismissViewControllerAnimated:YES completion:NULL];

            let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate
            apd.selectTerminalId = self.termFilteredArray[indexPath.row]
        
            self.performSegue(withIdentifier: "termSelectDone", sender: self)
        } else {
            // @"Select comapny or prefect");
        }
    }
    // MARK: - Segmented Control
    @IBAction func scopeChanged(_ sender: UISegmentedControl) {
        tableView.reloadData()
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
        filterContentForSearchText(searchController.searchBar.text!)
    }
}
