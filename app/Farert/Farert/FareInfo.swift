//
//  FareInfo.swift
//  Farert
//
//  Created by TAKEDA, Noriyuki on 2015/05/04.
//  Copyright (c) 2015年 TAKEDA, Noriyuki. All rights reserved.
//

import Foundation

class FareInfo {

    // MARK: - Public property

    var fareForStockDiscounts : [Int] = [0, 0]
    var fareForStockDiscountNames : [String?] = ["", ""]

    
    var result : Int = 0    /* 0: success, 1: KOKURA-pending, 2:Company only, -1: failure */
    var calcResultFlag : Int = 0
    //	calcResultFlag
    //     & 0x80 = 0    : 該当なし
    //     & 0x80 = 0x80 : empty or non calc.
    //     & 0x03 = 0 : 無し(通常)(発・着が特定都区市内駅で特定都区市内間が100/200km以下ではない)
    //			 (以下、発・着が特定都区市内駅で特定都区市内間が100/200kmを越える)
    //	   & 0x03 = 0x01 : 結果表示状態は{特定都区市内 -> 単駅} (「発駅を単駅に指定」と表示)
    //	   & 0x03 =	0x02 : 結果表示状態は{単駅 -> 特定都区市内} (「着駅を単駅に指定」と表示)
    //     & 0x0c = 0x04 : 大阪環状線1回通過(近回り)(規定)
    //     & 0x0c = 0x08 : 大阪環状線1回通過(遠回り)
    
    var beginStationId : Int = 0
    var endStationId : Int = 0
    
    var isBeginInCiry : Bool = false
    var isEndInCiry : Bool = false
    
    var availCountForFareOfStockDiscount : Int = 0
    var rule114_fare : Int = 0
    var rule114_salesKm : Int = 0
    var rule114_calcKm : Int = 0
    var isArbanArea : Bool = false
    
    
    var totalSalesKm : Int = 0
    var jrCalcKm : Int = 0
    var jrSalesKm : Int = 0
    /* 会社線有無はtotalSalesKm != jrSalesKm */
    
    var companySalesKm : Int = 0
    var salesKmForHokkaido : Int = 0
    var calcKmForHokkaido : Int = 0
    
    
    var salesKmForShikoku : Int = 0
    var calcKmForShikoku : Int = 0
    
    
    var salesKmForKyusyu : Int = 0
    var calcKmForKyusyu : Int = 0
    
    
    // 往復割引有無
    var isRoundtripDiscount : Bool = false
    
    // 会社線部分の運賃
    var fareForCompanyline : Int = 0
    
    // 普通運賃
    var fareForJR : Int = 0
    
    // 普通運賃は, fareForJR + fareForCompanyline;
    // 往復
    var roundTripFareWithComapnyLine : Int = 0
    
    // IC運賃
    var fareForIC : Int = 0
    
   
    // 有効日数
    var ticketAvailDays : Int = 0
    
    // 経路
    var routeList : String = ""
    

    func setFareForeStockDiscounts(discount1 : Int, title1 title1_ : String, Discount2 discount2_ : Int, title2 title2_ : String) {
        if (discount1 <= 0) {
            self.availCountForFareOfStockDiscount = 0;

            fareForStockDiscountNames[0] = nil;
            fareForStockDiscountNames[1] = nil;
            fareForStockDiscounts[0] = 0;
            fareForStockDiscounts[1] = 0;
        } else {
            if (discount2_ <= 0) {
                self.availCountForFareOfStockDiscount = 1;
                fareForStockDiscounts[1] = 0;
                fareForStockDiscountNames[1] = nil;
            } else {
                self.availCountForFareOfStockDiscount = 2;
                fareForStockDiscounts[1] = discount2_;
                fareForStockDiscountNames[1] = title2_;
            }
            fareForStockDiscounts[0] = discount1;
            fareForStockDiscountNames[0] = title1_;
        }
    }
    
    // 株主割引
    func fareForStockDiscount(index : Int) -> Int {
        if (1 < index) {
            return 0;
        }
        return fareForStockDiscounts[index];
    }
    
    func fareForStockDiscountTitle(index : Int) -> String {
        if (1 < index) {
            return ""
        }
        return fareForStockDiscountNames[index]!
    }
}