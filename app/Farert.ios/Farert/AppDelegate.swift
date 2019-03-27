//
//  AppDelegate.swift
//  SingleAppTmp
//
//  Created by TAKEDA, Noriyuki on 2015/04/03.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?
    
    var selectTerminalId:Int!
    var selectLineId:Int!
    var context:Int!
    

    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {

        if window == nil {
            return true
        }
        if window?.rootViewController == nil {
            return true
        }
        if (!cRouteUtil.openDatabase()) {
            // Can't db open
            return false;
        }

        let storyboard = UIStoryboard(name: "Main", bundle: nil)

        let leftViewController = storyboard.instantiateViewController(withIdentifier: "LeftViewController") as! LeftTableViewController

        let nvc : UINavigationController = storyboard.instantiateViewController(withIdentifier: "MainNavi") as! UINavigationController
////        UINavigationBar.appearance().tintColor = UIColor(red: 0x68 / 255, green: 0x9F / 255, blue: 0x38 / 255, alpha: 1.0)
        
        leftViewController.mainViewController = nvc

        let slideMenuController = SlideMenuController(mainViewController: nvc, leftMenuViewController: leftViewController)   // , rightMenuViewController: rightViewController)
        self.window?.rootViewController = slideMenuController
        self.window?.makeKeyAndVisible()

        return true
    }

    func applicationWillResignActive(_ application: UIApplication) {
        // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
        // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    }

    func applicationDidEnterBackground(_ application: UIApplication) {
        // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
        // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
        cRouteUtil.close()
    }

    func applicationWillEnterForeground(_ application: UIApplication) {
        // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.

        cRouteUtil.openDatabase()
    }

    func applicationDidBecomeActive(_ application: UIApplication) {
        // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    }

    func applicationWillTerminate(_ application: UIApplication) {
        // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
        cRouteUtil.close()
    }
}
