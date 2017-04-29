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

class LeftTableViewController: UIViewController
{

    @IBOutlet weak var editButton: UIButton!
    @IBOutlet weak var tableView: UITableView!

    var routeFolder : Routefolder! = Routefolder()
    var mainViewController: UIViewController!
 
    var delegate: MainTableViewControllerDelegate!
    
    let aggregate_label = [ "普通運賃", "小児運賃", "往復運賃", "株割運賃", "株4割運賃" ]
    

    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()

        self.tableView.separatorColor = UIColor(red: 224/255, green: 224/255, blue: 224/255, alpha: 1.0)

        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        self.navigationItem.leftBarButtonItem = self.editButtonItem
        
        self.tableView.dataSource = self
        self.tableView.delegate = self
        
//        self.tableView.reloadData()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        if let indexPathForSelectedRow = tableView.indexPathForSelectedRow {
            tableView.deselectRow(at: indexPathForSelectedRow, animated: true)
        }
    }
    
    func changeViewController() {
        self.slideMenuController()?.changeMainViewController(self.mainViewController, close: true)
    }

    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        ///self.imageHeaderView.frame = CGRect(x: 0, y: 0, width: self.view.frame.width, height: 160)
        self.view.layoutIfNeeded()
    }
    
    @IBAction func actionAddButton(_ sender: UIButton) {
 
        if let route  = delegate.getRoute() {
            routeFolder.add(item: route)
            self.tableView.reloadData()
        }
        print("leftviewidth=\(self.view.bounds.size.width)")
    }
}

extension LeftTableViewController : UITableViewDelegate {
    ///func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
    ///    indexPath.row
    ///    return 0
    ///}
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        delegate.changeRoute(route: self.routeFolder.routeItem(index: indexPath.row))
        self.changeViewController()
    }
    
    //func scrollViewDidScroll(_ scrollView: UIScrollView) {
    //    if self.tableView == scrollView {
    //
    //    }
    //}
}

extension LeftTableViewController : UITableViewDataSource {
    
    func numberOfSections(in tableView: UITableView) -> Int {
        // #warning Potentially incomplete method implementation.
        // Return the number of sections.
        return 1
    }

    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if section == 0 {
print("lines=\(routeFolder.count())")
            return routeFolder.count()
        } else {
            return 0
        }
    }
    
    // 701 702 710 711
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        var cell : UITableViewCell
        
        cell = tableView.dequeueReusableCell(withIdentifier: "holderCell", for: indexPath)
        if indexPath.row < routeFolder.count() {
            let rt : cRouteList = self.routeFolder.routeItem(index: indexPath.row)
            let bi : Int = self.routeFolder.aggregateType(index: indexPath.row)
            
            let lbl : UILabel = cell.viewWithTag(710) as! UILabel

            let begin : Int = rt.startStationId()
            let arrive : Int = rt.lastStationId()
            lbl.text = "\(cRouteUtil.terminalName(begin)!) → \(cRouteUtil.terminalName(arrive)!)"
            
            let btn : UIButton = cell.viewWithTag(711) as! UIButton
            let s : String = aggregate_label[bi]
            print("\(s) -- \(indexPath.row)")
            btn.titleLabel?.text = s
        }
        return cell
    }
    
    
}
