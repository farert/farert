//
//  SelectStationTableViewController.swift
//  iFarert Select station 2nd right view
//
//  Created by TAKEDA, Noriyuki on 2015/03/30.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

class SelectStationTableViewController: CSTableViewController {
    // in
    var companyOrPrefectId : Int = 0
    var lineId : Int = 0
    var lastStationId : Int = 0
    var startStationId : Int = 0

    //
    var stations : [Int] = [];

    override func viewDidLoad() {
        super.viewDidLoad()
        
        //NSLog(@"didload begin: transit %d", self.transit_state);

        if #available(iOS 13.0, *) {
            self.view.backgroundColor = UIColor.systemBackground
        } else {
            // Fallback on earlier versions
        }

        // Uncomment the following line to preserve selection between presentations.
        self.clearsSelectionOnViewWillAppear = false
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem;
        let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate

        if (apd.context == FGD.CONTEXT.AUTOROUTE_VIEW) || (apd.context == FGD.CONTEXT.TERMINAL_VIEW) {
            self.navigationItem.setRightBarButtonItems(nil, animated:true)
            stations = cRouteUtil.stationsWith(inCompanyOrPrefectAnd: companyOrPrefectId, lineId: lineId) as! [Int]
            if (apd.context == FGD.CONTEXT.AUTOROUTE_VIEW) {
                self.title = "着駅指定";
            } else {
                self.title = "発駅指定";
            }
        } else { //CONTEXT_ROUTESELECT_VIEW
            if (FGD.TRANSIT.STA2JCT != self.transit_state) {
                self.stations = cRouteUtil.stationsIds(ofLineId: lineId) as! [Int]
                self.title = "着駅指定"
                self.navigationItem.rightBarButtonItem!.title = "分岐駅指定";
            } else {
                stations = cRouteUtil.junctionIds(ofLineId: lineId, stationId: startStationId) as! [Int]
                self.title = "分岐駅指定"
                self.navigationItem.rightBarButtonItem!.title = "着駅指定"
            }
        }
        //NSLog(@"didload last:transit %d", self.transit_state);
    }
    
    
    @IBAction func terminalBarButtonAction(_ sender: AnyObject) {
        self.performSegue(withIdentifier: "stationViewSegue", sender: self)
    }
   
    
    // mark - Table view data source
    
    override func numberOfSections(in tableView: UITableView) -> Int {
        // Return the number of sections.
        return 1;
    }
    
    
    override func tableView(_ tableView : UITableView, titleForHeaderInSection section : Int) -> String? {
        if (0 < self.companyOrPrefectId) {
            let companyOrPrefctString : String = cRouteUtil.companyOrPrefectName(companyOrPrefectId)
            return "\(companyOrPrefctString) - \(cRouteUtil.lineName(self.lineId)!)" as String

        } else {
            return cRouteUtil.lineName(lineId) as String
        }
    }

    
    
    override func tableView(_ tableView : UITableView, numberOfRowsInSection section : Int) -> Int {
        // Return the number of rows in the section.
        return stations.count
    }
    
    
    override func tableView(_ tableView : UITableView, cellForRowAt indexPath : IndexPath) -> UITableViewCell {

        let stationId : Int = stations[indexPath.row]
        var cell : UITableViewCell! = nil
        
        let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate
        if ((apd.context == FGD.CONTEXT.ROUTESELECT_VIEW) && (self.lastStationId == stationId)) {
            //インデント付き＋チェックマーク付き（起点駅)
            cell = tableView.dequeueReusableCell(withIdentifier: "terminalStationCell2", for: indexPath) 
        } else {
            cell = tableView.dequeueReusableCell(withIdentifier: "terminalStationCell", for: indexPath) 
        }
        
        // Configure the cell...;
        if (self.startStationId == stationId) {
            let t = cRouteUtil.stationName(stationId)!
            cell!.textLabel?.text = "＞ \(t)"        // 発駅
        } else {
            cell!.textLabel?.text = cRouteUtil.stationName(stationId)
        }
        
        var details : String = "(\(cRouteUtil.getKanaFromStationId(stationId)!))"
        if (cRouteUtil.isJunction(stationId) &&
            !cRouteUtil.isSpecificJunction(self.lineId, stationId: stationId)) {
                /* junction(lflag.bit12 on) */

            for lid in cRouteUtil.lineIds(fromStation: stationId) as! [Int] {
                if ((lid != self.lineId)  &&
                    !cRouteUtil.isSpecificJunction(lid, stationId: stationId)) {
                    details += "/" + cRouteUtil.lineName(lid)
                }
            }
            //if ("/" == details[details.endIndex.predecessor()]) {
            //    details += cRouteUtil.LineName(self.lineId)
            //}
        }

        cell.detailTextLabel?.text = details
        
        return cell;
    }
    
    // macOS用：セクションヘッダーの高さを調整
    override func tableView(_ tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        #if targetEnvironment(macCatalyst)
        return 60  // macOSでは高めに設定（デフォルト22の約3倍）
        #else
        return 28  // iOS/iPadOS
        #endif
    }

    // macOS用：セクションヘッダーに上部マージンを追加
    override func tableView(_ tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        let headerView = UIView()
        if #available(iOS 13.0, *) {
            headerView.backgroundColor = .systemBackground
        } else {
            // Fallback on earlier versions
        }
        
        let label = UILabel()
        label.text = self.tableView(tableView, titleForHeaderInSection: section)
        label.font = UIFont.preferredFont(forTextStyle: .headline)
        if #available(iOS 13.0, *) {
            label.textColor = .secondaryLabel
        } else {
            // Fallback on earlier versions
        }
        label.translatesAutoresizingMaskIntoConstraints = false
        
        headerView.addSubview(label)
        
        #if targetEnvironment(macCatalyst)
        // macOS用：上部に大きめの余白を追加
        NSLayoutConstraint.activate([
            label.leadingAnchor.constraint(equalTo: headerView.leadingAnchor, constant: 16),
            label.trailingAnchor.constraint(equalTo: headerView.trailingAnchor, constant: -16),
            label.topAnchor.constraint(equalTo: headerView.topAnchor, constant: 30), // 上部に30pt余白
            label.bottomAnchor.constraint(equalTo: headerView.bottomAnchor, constant: -6)
        ])
        #else
        NSLayoutConstraint.activate([
            label.leadingAnchor.constraint(equalTo: headerView.leadingAnchor, constant: 16),
            label.trailingAnchor.constraint(equalTo: headerView.trailingAnchor, constant: -16),
            label.topAnchor.constraint(equalTo: headerView.topAnchor, constant: 6),
            label.bottomAnchor.constraint(equalTo: headerView.bottomAnchor, constant: -6)
        ])
        #endif
        
        return headerView
    }

    // 戻ってきたときにセルの選択を解除
    override func viewWillAppear(_ animated : Bool) {
        super.viewWillAppear(animated)
        if let idx : IndexPath = self.tableView.indexPathForSelectedRow {
            self.tableView.deselectRow(at: idx, animated:false)
        }
    }
    
    // MARK: - Navigation
    
    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
        let segid : String = segue.identifier!
        
        if (segid == "terminalSelectDoneSegue") {
            let apd : AppDelegate = UIApplication.shared.delegate as! AppDelegate
            apd.selectTerminalId = 0xffff & self.stations[self.tableView.indexPathForSelectedRow!.row]
            apd.selectLineId = self.lineId;
            
        } else if (segid == "stationViewSegue") {
            let thisViewController : SelectStationTableViewController = segue.destination as! SelectStationTableViewController
            //NSLog(@"prepare segue: transit set %d-> ", self.transit_state);
            thisViewController.transit_state = (self.transit_state == FGD.TRANSIT.STA2JCT) ? FGD.TRANSIT.JCT2STA : FGD.TRANSIT.STA2JCT;
            thisViewController.lineId = self.lineId;
            thisViewController.companyOrPrefectId = self.companyOrPrefectId;
            assert(self.companyOrPrefectId == 0, "bug")
            thisViewController.lastStationId = self.lastStationId;
            thisViewController.startStationId = self.startStationId;
        }
    }
}
