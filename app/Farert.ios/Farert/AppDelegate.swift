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
    var context: FGD.CONTEXT!
    

    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        // Window / root view controller setup is done in SceneDelegate (UIScene life cycle).
        if (!cRouteUtil.openDatabase()) {
            // Can't db open
            return false;
        }
        return true
    }

    func applicationWillTerminate(_ application: UIApplication) {
        // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
        cRouteUtil.closeDatabase()
    }
}
