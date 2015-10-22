//
//  TerminalSelectTableViewController.swift
//  iFarert
//
//  Created by TAKEDA, Noriyuki on 2015/03/23.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit


class TerminalSelectTableViewController: CSTableViewController, UISearchBarDelegate, UISearchDisplayDelegate {

    @IBOutlet weak var termSearchBar: UISearchBar!

    var termFilteredArray : [Int] = []
    var termCompanyPrefectArray : [[Int]] = RouteDataController.GetCompanyAndPrefects() as! [[Int]]

    override func viewDidLoad() {
        super.viewDidLoad()
    
        self.navigationController?.toolbarHidden = false
        
        let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate

        if (apd.context == FGD.CONTEXT_AUTOROUTE_VIEW) {
            //self.navigationController.title = @"着駅指定(最短経路)";
            self.title = "着駅指定(最短経路)"
        } else {
            //self.navigationController.title = @"発駅指定";
            self.title = "発駅指定"
        }
        // Uncomment the following line to preserve selection between presentations.
        self.clearsSelectionOnViewWillAppear = false
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem;
        
        //UIBarButtonItem *backButton = [[UIBarButtonItem alloc] initWithTitle:@"戻る" style:UIBarButtonItemStylePlain target:nil action:nil];
        //self.navigationItem.backBarButtonItem = backButton;
        //self.navigationItem.backBarButtonItem.title = @"戻る"; //これはよいけど（って効かないからだめだが）上は幅広バーになっちゃう
    }
    
    override func viewWillAppear(animated : Bool) {

        super.viewWillAppear(animated)
    
        // 最初に表示されたときに検索バーを非表示に
        //    [self.tableView setContentOffset:CGPointMake(0.0f, self.searchDisplayController.searchBar.frame.size.height)];
    
        // 戻ってきたときにセルの選択を解除
        if let idx : NSIndexPath = self.tableView.indexPathForSelectedRow {
            self.tableView.deselectRowAtIndexPath(idx, animated:false)
        }
    }
    

    // MARK: - Table view data source
    
    override func numberOfSectionsInTableView(tableView : UITableView) -> Int {
        // Return the number of sections.
        if (tableView == self.searchDisplayController?.searchResultsTableView) {
            return 1
        } else {
            return 2   /* Company, Prefect */
        }
    }
    
    override func tableView(tableView : UITableView, numberOfRowsInSection section : Int) -> Int {
        // Return the number of rows in the section.

        if (tableView == self.searchDisplayController?.searchResultsTableView) {
            return self.termFilteredArray.count
        } else {
            if ((section == 0) || (section == 1)) {
                let arys : [Int] = self.termCompanyPrefectArray[section] as [Int]
                return arys.count
            }
        }
        return 0;
    }
    
    override func tableView(tableView : UITableView, titleForHeaderInSection section : Int) -> String? {
        
        if (tableView != self.searchDisplayController?.searchResultsTableView) {
            switch (section) {
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
    
    
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {

        let cell_identifier : String = "termSearchbarCell"
        var cell : UITableViewCell!
        
        if (tableView == self.searchDisplayController?.searchResultsTableView) {
            cell = tableView.dequeueReusableCellWithIdentifier(cell_identifier)
            if (cell == nil) {
                cell = UITableViewCell(style: UITableViewCellStyle.Value1, reuseIdentifier: cell_identifier)
                if (cell == nil) {
                    return cell
                }
            }
            let ident : Int = self.termFilteredArray[indexPath.row]
            cell.textLabel?.text = RouteDataController.StationNameEx(ident)
            cell.detailTextLabel?.text = "\(RouteDataController.GetKanaFromStationId(ident))(\(RouteDataController.PrectNameByStation(ident)))"
        } else {
            cell = tableView.dequeueReusableCellWithIdentifier("termCompanyAndPrefectCell")
            let ident : Int = self.termCompanyPrefectArray[indexPath.section][indexPath.row]
            if cell != nil {
                cell.textLabel?.text = RouteDataController.CompanyOrPrefectName(ident)
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
    override func prepareForSegue(segue : UIStoryboardSegue, sender sender_ : AnyObject?) {
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        let segid : String! = segue.identifier
        
        if segid == "termLineSegue" {
            // 都道府県または会社に属する路線一覧
            let selectLineTblViewController : SelectLineTableViewController = segue.destinationViewController as! SelectLineTableViewController
            
            let section : Int = self.tableView.indexPathForSelectedRow?.section ?? 0
            let row : Int = self.tableView.indexPathForSelectedRow?.row ?? 0
            let iDs : [Int] = self.termCompanyPrefectArray[section] as [Int]
            selectLineTblViewController.companyOrPrefectId = iDs[row]
            selectLineTblViewController.baseStationId = 0
            selectLineTblViewController.lastLineId = 0
            
        } else if segid == "toHistorySegue" {
            // 過去指定 履歴駅一覧
            let dvc : CSTableViewController = segue.destinationViewController as! CSTableViewController
            dvc.transit_state = FGD.FA_TRANSIT_HISTORY
            
        } /* else termSelectDone(unwind)(検索結果から駅選択) */
        /* else termCancelSegue(unwind)(return to Main) */
    }
    
    
    
    // MARK: - Content Filtering
    func filterContentForSearchText(searchText : String, scope  scope_ : String) {
        // Update the filtered array based on the search text and scope.
        // Remove all objects from the filtered search array
        self.termFilteredArray.removeAll(keepCapacity: false)
        
        if (true != searchText.isEmpty) {
            self.termFilteredArray = RouteDataController.KeyMatchStations(searchText) as! [Int]
        }
    }
    
    // MARK: - UISearchDisplayController Delegate Methods
    func searchDisplayController(controller : UISearchDisplayController, shouldReloadTableForSearchString searchString : String?) -> Bool {
        // Tells the table data source to reload when text changes
        
        let selidx : Int = self.searchDisplayController?.searchBar.selectedScopeButtonIndex ?? 0
        var scope_str : String
        if let scope_titles : [String]? = self.searchDisplayController?.searchBar.scopeButtonTitles {
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
    
    func searchDisplayController(controller : UISearchDisplayController, shouldReloadTableForSearchScope searchOption : Int) -> Bool {
        
        var scope_str : String
        if let scope_titles : [AnyObject]? = self.searchDisplayController?.searchBar.scopeButtonTitles {
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
    
    
    override func tableView(tableView : UITableView, didSelectRowAtIndexPath indexPath : NSIndexPath) {
        
        if (tableView == self.searchDisplayController?.searchResultsTableView) {

//                if ([self.delegate respondsToSelector:@selector(terminalSelectTablebleViewDone:)]) {
//                [self.delegate terminalSelectTablebleViewDone:[[termFilteredArray objectAtIndex:[indexPath row]] intValue]];
//                }
//                self.terminalId = [NSNumber numberWithInt:indexPath.row];
//                [self dismissViewControllerAnimated:YES completion:NULL];

            let apd : AppDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
            apd.selectTerminalId = self.termFilteredArray[indexPath.row]
        
            self.performSegueWithIdentifier("termSelectDone", sender: self)
        } else {
            // @"Select comapny or prefect");
        }
    }

}
