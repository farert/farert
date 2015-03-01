//
//  VersionInfoViewController.m
//  FarertPre バージョン情報ビュー
//
//  Created by TAKEDA, Noriyuki on 2014/08/31.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#import "VersionInfoViewController.h"
#import "RouteDataController.h"
#import "DbSys.h"

@interface VersionInfoViewController ()

@end

@implementation VersionInfoViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}


- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:YES];
    
    UILabel* lbl = (UILabel*)[self.view viewWithTag:1023432];
    DbSys* dbverInf = [RouteDataController DatabaseVersion];
    lbl.text = [NSString stringWithFormat:@"DB Rev. [%@](%@)", dbverInf.name, dbverInf.create_date];
}


- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    NSLog(@"memory war in version view");
}

- (IBAction)gotoSupportURL:(id)sender {
    
    NSURL *url = [NSURL URLWithString:@"http://farert.blogspot.jp"];
    
    if ([[UIApplication sharedApplication] canOpenURL:url]) {
        [[UIApplication sharedApplication] openURL:url];
    } else {
        // エラー処理
    }
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
