//
//  VersionInfoViewController.swift
//  iFarert 
//
//  Created by TAKEDA, Noriyuki on 2015/03/23.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit



class VersionInfoViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


    
    override func viewWillAppear(_ animated : Bool) {

        super.viewWillAppear(true)
    
        guard let dbverInf : DbSys = cRouteUtil.databaseVersion() else {return}

        if let lbl : UILabel = self.view.viewWithTag(1023432) as! UILabel? {
            lbl.text = "DB Rev. [\(dbverInf.name!)](\(dbverInf.create_date!))"
        }
        if let lbl2 : UILabel = self.view.viewWithTag(190915) as! UILabel? {
            lbl2.text = "消費税: \(dbverInf.tax)%"
        }
        let version = Bundle.main.object(forInfoDictionaryKey: "CFBundleShortVersionString") as! String
        if let lbl3 : UILabel = self.view.viewWithTag(19103005) as! UILabel? {
            lbl3.text = "Farert " + version
        }
    }
    
    
    @IBAction func gotoSupportURL(_ sender: AnyObject) {
    
        let url : URL = URL(string: "http://farert.blogspot.jp/")!
    
        if UIApplication.shared.canOpenURL(url) {
            UIApplication.shared.open(url)
        } else {
            // エラー処理
        }
    }
    

    // MARK: - Navigation
    /*
    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */
}
