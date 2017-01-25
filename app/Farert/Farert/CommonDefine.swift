//
//  FGD.swift
//  Farert
//
//  Created by TAKEDA, Noriyuki on 2015/04/14.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import Foundation

struct FGD {
    static let  MAX_HISTORY                    = 20
    static let  MAX_ARCHIVE_ROUTE             = 100
    static let  CONTEXT_TERMINAL_VIEW         = 23900  // Terminal select series
    static let  CONTEXT_AUTOROUTE_VIEW        = 23901  // End station select for Auto route select series
    static let  CONTEXT_ROUTESELECT_VIEW      = 23902  // Route select
    static let  CONTEXT_AUTOROUTE_ACTION      = 34392  // Action sheet
    static let  CONTEXT_BEGIN_TERMINAL_ACTION = 34393
    static let  CONTEXT_ROUTESETUP_VIEW       = 34394
    
    //static let  CONTEXT_AUTOROUTE_BUTTON 23943  // Auto route barbutton
    //static let  CONTEXT_JUNCTION_SWITCH_BUTTON 23945    // Arrive station barbutton
    
    static let  FA_TRANSIT_HISTORY = 3
    static let  FA_TRANSIT_STA2JCT = 0
    static let  FA_TRANSIT_JCT2STA = 1
    static let  FA_TRANSIT_AUTOROUTE = 2

    /**** public ****/
    
    static let  ADDRC_LAST  = 0   // add() return code
    static let  ADDRC_OK    = 1
    // ADDRC_NG -1 to -N
    
    
    /* cooked flag for shoFare(), show_route() */
    // bit 15  (User->System)
    static let  FAREOPT_AVAIL_RULE_APPLIED          	= 0x8000	// 有効ビットマスク
    static let  FAREOPT_RULE_NO_APPLIED					= 0x8000	// 特別規則適用なし
    static let  FAREOPT_RULE_APPLIED					= 0		// 通常
    
    // bit 0-1 (User->System)
    static let  FAREOPT_AVAIL_APPLIED_START_TERMINAL 	= 1   // 有効ビットマスク
    static let  FAREOPT_APPLIED_START					= 1	// 名阪間 発駅を市内駅に適用
    static let  FAREOPT_APPLIED_TERMINAL				= 2	// 名阪間 着駅を市内駅に適用
}

struct DbId {
// cRouteUtilにも同じような定義あり、なんとかしたい
    static let DB_MIN_ID = 10
    
    static let DB_TAX5 = DB_MIN_ID
    static let DB_2014 = 11
    static let DB_2015 = 12
    static let DB_2016 = 13

    static let DB_MAX_ID = DB_2016
}
