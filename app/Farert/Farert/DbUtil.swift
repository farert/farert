//
//  DbUtil.swift
//  Farert
//
//  Created by TAKEDA, Noriyuki on 2015/04/16.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import Foundation

class DbUtil {
    // (System->User)
    class func IsMaihanCityStartTerminalEnable(flg: Int) -> Bool {
        return (flg & 0x03) != 0x00
    }

    class func IsMaihanCityStart(flg: Int) -> Bool {
        return (flg & 0x03) == 0x01
    }

    class func IsMaihanCityTerminal(flg: Int) -> Bool {
        return (flg & 0x03) == 0x02
    }
    
    // bit 2-3
    class func IsOsakakanDetourEnable(flg: Int) -> Bool {
        return (flg & 0x0c) != 0x00
    }
    
    // TRUE: Detour / FALSE: Shortcut
    class func IsOsakakanDetourShortcut(flg: Int) -> Bool {
        return (flg & 0x0c) == 0x08
    }
    
    // bit 4-5
    class func IsRuleAppliedEnable(flg: Int) -> Bool {
        return (flg & 0x30) != 0x00
    }

    class func IsRuleApplied(flg: Int) -> Bool {
        return (flg & 0x30) == 0x10
    }

    class func IsFareOptEnabled(flg: Int) -> Bool {
        return (flg & 0x3f) != 0x00
    }
}
