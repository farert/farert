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
    
    enum CONTEXT {
        case  FIRST
        case  NOTHING
        case  TERMINAL_VIEW           // Terminal select series
        case  AUTOROUTE_VIEW          // End station select for Auto route select series
        case  ROUTESELECT_VIEW        // Route select
        case  AUTOROUTE_ACTION        // Action sheet
        case  BEGIN_TERMINAL_ACTION
        case  ROUTESETUP_VIEW
        case  TICKETHOLDER_VIEW
    }
    //static let  CONTEXT_AUTOROUTE_BUTTON 23943  // Auto route barbutton
    //static let  CONTEXT_JUNCTION_SWITCH_BUTTON 23945    // Arrive station barbutton
    
    enum TRANSIT {
        case  UNKNOWN
        case  HISTORY
        case  STA2JCT
        case  JCT2STA
        case  AUTOROUTE
    }
    /**** public ****/
    
    static let  ADDRC_LAST  = 0   // add() return code
    static let  ADDRC_OK    = 1
    // ADDRC_NG -1 to -N
    
    /*
    /* cooked flag for shoFare(), show_route() */
    // bit 15  (User->System)
    static let  FAREOPT_AVAIL_RULE_APPLIED          	= 0x8000	// 有効ビットマスク
    static let  FAREOPT_RULE_NO_APPLIED					= 0x8000	// 特別規則適用なし
    static let  FAREOPT_RULE_APPLIED					= 0		// 通常
    
    // bit 0-1 (User->System)
    static let  FAREOPT_AVAIL_APPLIED_START_TERMINAL 	= 1   // 有効ビットマスク
    static let  FAREOPT_APPLIED_START					= 1	// 名阪間 発駅を市内駅に適用
    static let  FAREOPT_APPLIED_TERMINAL				= 2	// 名阪間 着駅を市内駅に適用
    
    // bit 8-9 JR東海株主優待券
    static let FAREOPT_AVAIL_APPLIED_JRTOKAI_STOCK     = 0x40   // 有効ビットマスク
    static let FAREOPT_JRTOKAI_STOCK_APPLIED           = 0x40    // JR東海株主に適用
    static let FAREOPT_JRTOKAI_STOCK_NO_APPLIED        =   0     // JR東海株主に非適用
 */
}


