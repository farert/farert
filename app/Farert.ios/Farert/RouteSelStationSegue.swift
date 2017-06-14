//
//  RouteSelStationSegue.swift
//  iFarert
//
//  Created by TAKEDA, Noriyuki on 2015/03/23.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import UIKit


class RouteSelStationSegue: UIStoryboardSegue {

    override func perform() {
        
//        if self.identifier == "autoRouteSegue" {
//            /* 経路選択(路線選択)→最短経路 */
//            let sourceViewController : UITableViewController = self.sourceViewController as! UITableViewController
//            let destinationViewController : UINavigationController = self.destinationViewController as! UINavigationController
//
//            let navigationController : UINavigationController = sourceViewController.navigationController! as UINavigationController
//
//            //路線選択 - 最短経路
//            navigationController.popViewControllerAnimated(false)
//
//            UIView.transitionWithView(navigationController.view, duration:0.3, options:UIViewAnimationOptions.TransitionCurlDown, animations: {
//                navigationController.pushViewController(destinationViewController, animated: false)
//                }, completion: nil)
//            return
//        }
// iOS7.1で飛ぶので辞めました.  CustomでなくPresent Modallyにしました.
        
        let sourceViewController : CSTableViewController = self.source as! CSTableViewController
        let destinationViewController : CSTableViewController = self.destination as! CSTableViewController
        
        let navigationController : UINavigationController = sourceViewController.navigationController! as UINavigationController
        
        if (destinationViewController.transit_state == FGD.FA_TRANSIT_AUTOROUTE) {
            destinationViewController.modalTransitionStyle = UIModalTransitionStyle.partialCurl;
            sourceViewController.present(destinationViewController, animated:true, completion:nil)
            
        } else if (destinationViewController.transit_state == FGD.FA_TRANSIT_HISTORY) {
            /* 履歴 (Jan.2017,4th - removed) */
            UIView.transition(with: navigationController.view, duration:0.3, options:UIViewAnimationOptions.transitionCrossDissolve,
                animations: {
                 navigationController.pushViewController(destinationViewController, animated: false)
                }, completion: nil)
        } else {
            if (destinationViewController.transit_state == FGD.FA_TRANSIT_STA2JCT) {
                // Unwind segueの場合は、右からflipするアニメーション
                // 着駅→分岐駅
                navigationController.popViewController(animated: false)
                UIView.transition(with: navigationController.view, duration:0.3, options:UIViewAnimationOptions.transitionFlipFromRight, animations: {
                    navigationController.pushViewController(destinationViewController, animated: false)
                    }, completion: nil)
                //[navigationController popViewControllerAnimated:NO];
                
            } else {    // segueの場合は、左からflipするアニメーション
                // 分岐駅→着駅
                navigationController.popViewController(animated: false)
                UIView.transition(with: navigationController.view, duration:0.3, options:UIViewAnimationOptions.transitionFlipFromLeft, animations: {
                     navigationController.pushViewController(destinationViewController, animated: false)
                    }, completion: nil)
                //[navigationController popViewControllerAnimated:NO];
            }
        }
    }
}
