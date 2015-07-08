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


    
    override func viewWillAppear(animated : Bool) {

        super.viewWillAppear(true)
    
        if let lbl : UILabel = self.view.viewWithTag(1023432) as! UILabel? {
            let dbverInf : DbSys = RouteDataController.DatabaseVersion()
            lbl.text = "DB Rev. [\(dbverInf.name)](\(dbverInf.create_date))"
        }
    }
    
    
    @IBAction func gotoSupportURL(sender: AnyObject) {
    
        let url : NSURL = NSURL(string: "http://farert.blogspot.jp/")!
    
        if UIApplication.sharedApplication().canOpenURL(url) {
            UIApplication.sharedApplication().openURL(url)
        } else {
            // エラー処理
        }

        // Share Extensionでの真のOpen URL
        //let webView : UIWebView  = UIWebView()
        //self.view.addSubview(webView)           // ロード完了まで保持しておきたいので適当に貼り付けておく
        //let urq : NSURLRequest = NSURLRequest(URL: url)
        //webView.loadRequest(urq)
    }
    
    func webView(webView: UIWebView!, shouldStartLoadWithRequest request: NSURLRequest!, navigationType: UIWebViewNavigationType) -> Bool {
        return true
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
