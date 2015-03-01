//
//  AppDelegate.m
//  alps_ios
//
//  Created by TAKEDA, Noriyuki on 2014/06/09.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "AppDelegate.h"
#import "RouteDataController.h"

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Override point for customization after application launch.
    if ([[NSUserDefaults standardUserDefaults] boolForKey:@"HasLaunchedOnce"]) {
        // ２回目以降の起動時
    } else {
        // 初回起動時
        NSString* bodyStr = @"本アプリで表示される結果は必ずしも正確な情報ではないことがありえます。実際のご旅行での費用とは異なることがありえますことをご理解の上ご利用ください。\n本アプリにより発生したあらゆる損害は作者は負いません\n本アプリ及び提供元はJRグループ各社とはなんの関係もございません";
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"ダウンロードありがとうございます"
                                                            message:bodyStr
                                                           delegate:self
                                                  cancelButtonTitle:@"了解"
                                                  otherButtonTitles:nil];
        [alertView show];

        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"HasLaunchedOnce"];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }

    NSLog(@"int=%lu, %lu", sizeof(int), sizeof(NSInteger));
    if (![RouteDataController OpenDatabase]) {
        // Can't db open
        return NO;
    }

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.

    NSLog(@"database close.");
    [RouteDataController Close];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    NSLog(@"database open");
    [RouteDataController OpenDatabase];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.

    NSLog(@"database close");
    [RouteDataController Close];
}

@end
