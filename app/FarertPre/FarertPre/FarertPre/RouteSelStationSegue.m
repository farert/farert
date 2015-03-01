//
//  RouteSelStationSegue.m
//  FarertPre
//
//  Created by TAKEDA, Noriyuki on 2014/08/08.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "RouteSelStationSegue.h"
#import "CSTableViewController.h"

@implementation RouteSelStationSegue


- (void)perform
{
    CSTableViewController *sourceViewController = self.sourceViewController;
    CSTableViewController *destinationViewController = self.destinationViewController;

    UINavigationController *navigationController = sourceViewController.navigationController;

    if (destinationViewController.transit_state == FA_TRANSIT_AUTOROUTE) {
        destinationViewController.modalTransitionStyle = UIModalTransitionStylePartialCurl;
        [sourceViewController presentViewController:destinationViewController animated:YES completion:NULL];

    } else if (destinationViewController.transit_state == FA_TRANSIT_HISTORY) {
        /* 履歴 */
        [UIView transitionWithView:navigationController.view duration:0.3f options:UIViewAnimationOptionTransitionCrossDissolve
                        animations:^{
            [navigationController pushViewController:destinationViewController animated:NO];
        } completion:nil];
    } else {
        if (destinationViewController.transit_state == FA_TRANSIT_STA2JCT) {
            // Unwind segueの場合は、右からflipするアニメーション
            // 着駅→分岐駅
            [navigationController popViewControllerAnimated:NO];
            [UIView transitionWithView:navigationController.view duration:0.3f options:UIViewAnimationOptionTransitionFlipFromRight animations:^{
                [navigationController pushViewController:destinationViewController animated:NO];
            } completion:nil];
            //[navigationController popViewControllerAnimated:NO];

        } else {    // segueの場合は、左からflipするアニメーション
            // 分岐駅→着駅
            [navigationController popViewControllerAnimated:NO];
            [UIView transitionWithView:navigationController.view duration:0.3f options:UIViewAnimationOptionTransitionFlipFromLeft animations:^{
                [navigationController pushViewController:destinationViewController animated:NO];
            } completion:nil];
            //[navigationController popViewControllerAnimated:NO];
        }
    }
}
@end
