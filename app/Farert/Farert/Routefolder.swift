//
//  Routefolder.swift
//  Farert
//
//  Created by TAKEDA, Noriyuki on 2017/04/25.
//  Copyright © 2017年 TAKEDA, Noriyuki. All rights reserved.
//

import Foundation

public enum Aggregate: Int {
    case NORMAL = 0   // 普通運賃
    case CHILD     // 小児運賃
    case ROUNDTRIP // 往復
    case STOCK     // 株割
    case STOCKW    // 株割り4割
    case ACADEMIC   // 学割
    case ACADEMIC_ROUNDTRIP   // 学割往復
    case NULLFARE   // 無効
}



struct folder {
    var routeList : cRouteList
    var indexOfAggregate : Int
}

public class Routefolder {
    
    //private static let onlyObj = Routefolder()
    
    private var routeList : [folder] = []
    private var _totalFare : Int = 0
    private var _totalSalesKm : Int = 0

    var totalFare : String {
        get {
            if routeList.count <= 0 {
                return "¥---,---"
            } else {
                let formatter = NumberFormatter()
                formatter.numberStyle = NumberFormatter.Style.decimal
                formatter.groupingSeparator = ","
                formatter.groupingSize = 3
                return "¥\(formatter.string(from: NSNumber(value: _totalFare)) ?? "")"
                //return "¥\(String(describing: formatter.string(from: NSNumber(value: _totalFare))))"
            }
        }
    }
    
    var totalSalesKm : String {
        get {
            if routeList.count <= 0 {
                return "---,--- km"
            } else {
                return "\(String(describing: cRouteUtil.kmNumStr(_totalSalesKm)!)) km"
            }
        }
    }

    func add(item : cRouteList) -> Bool {
        if Int(MAX_HOLDER) <= routeList.count {
            return false
        }
        routeList.append(folder(routeList: item, indexOfAggregate: Aggregate.NORMAL.rawValue))
        calc()
        save()
        return true
    }
    
    //func route(index : Int) -> cRoute {
    //    if index < routeList.count {
    //        return cRoute(routeList: routeList[index].routeList)
    //    } else {
    //        assert(false, "Index failure...")
    //        return cRoute()
    //   }
    //}
    
    func routeItem(index : Int) -> cRouteList {
        if index < routeList.count {
            return routeList[index].routeList
        } else {
            assert(false, "Index failure...")
            return cRouteList()
        }
    }

    func routeItemFare(index : Int) -> String {
        if index < routeList.count {
            let fare = self.calcFare(item: routeList[index])
            let formatter = NumberFormatter()
            formatter.numberStyle = NumberFormatter.Style.decimal
            formatter.groupingSeparator = ","
            formatter.groupingSize = 3
            return "¥\(formatter.string(from: NSNumber(value: fare.fare)) ?? "")"
        } else {
            assert(false, "Index failure...")
            return ""
        }
    }
    
    func count() -> Int {
        return routeList.count
    }
 
    func aggregateType(index : Int) -> Int {
        if index < routeList.count {
            return routeList[index].indexOfAggregate
        } else {
            return 0
        }
    }

    func setAggregateType(index : Int, aggr : Int) -> Void {
        if index < routeList.count {
            routeList[index].indexOfAggregate = aggr
            calc()
            save()
        }
    }
    
    func remove(index : Int) {
        routeList.remove(at: index)
        calc()
        save()
    }
    
    func removeAll() -> Void {
        _totalSalesKm = 0
        _totalFare = 0
        routeList.removeAll()
        calc()
        save()
    }
   
    /*  Exchange content index_a and index b
     *
     */
    func exchange(index_source : Int, index_destination : Int) {
        let obj = routeList[index_source]
        routeList.remove(at: index_source)
        routeList.insert(obj, at: index_destination)
        save()
    }
    

    func calc() {
        var ta : Int = 0
        var td : Int = 0
        for route in self.routeList {
            let result = self.calcFare(item: route)
            ta += result.fare
            td += result.sales_km
        }
        self._totalFare = ta
        self._totalSalesKm = td
    }
    
    private var filePath : String {
        get {
            let documentsPath = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0] as String
            return documentsPath + "/routeholder.txt"
        }
    }

    func save() {
        print(filePath)
        if let os = OutputStream(toFileAtPath: filePath, append: false) {
            os.open()
            for one in routeList {
                let rs = String(one.indexOfAggregate) + "|" + one.routeList.routeScript() + "\n"
                os.write(rs, maxLength: rs.lengthOfBytes(using: String.Encoding.utf8))
            }
            os.close()
        }
    }
    
    func load() {
        do {
            var num : Int = 0
            let text = try String(contentsOf: URL(fileURLWithPath: filePath), encoding: String.Encoding.utf8)
            text.enumerateLines { line, stop in
                if num < Int(MAX_ARCHIVE_ROUTE) {
                    let record : [String] = line.components(separatedBy: "|")
                    if record.count == 2 {
                        if let agr : Int = Int(record[0]) {
                            if let rt = cRoute() {
                                let result = rt.setupRoute(record[1])
                                if 0 <= result {
                                    self.routeList.append(folder(routeList: cRouteList(route: rt), indexOfAggregate: agr))
                                    num += 1
                                }
                            }
                        }
                    }
                }
            }
        } catch {
            // Failed to read file
            print("read fail!!!!!!!!!")
        }
        self.calc()
    }

    func calcFare(item : folder) -> (fare : Int, sales_km : Int) {

        if let cds = cCalcRoute(routeList: item.routeList) {
            if let fareInfo : FareInfo = cds.calcFare() {
                var fare : Int = 0

                switch item.indexOfAggregate {
                case Aggregate.NORMAL.rawValue:   // 普通運賃
                    fare = fareInfo.fare
                case Aggregate.CHILD.rawValue:     // 小児運賃
                    fare = fareInfo.childFare
                case Aggregate.ROUNDTRIP.rawValue: // 往復
                    fare = fareInfo.roundTripFareWithCompanyLine
                case Aggregate.STOCK.rawValue:     // 株割
                    if fareInfo.availCountForFareOfStockDiscount < 1 {
                        fare = fareInfo.fare
                    } else {
                        fare = fareInfo.fare(forStockDiscount: 0)
                    }
                case Aggregate.STOCKW.rawValue:    // 株割り4割
                    if fareInfo.availCountForFareOfStockDiscount < 1 {
                        fare = fareInfo.fare
                    } else if 1 < fareInfo.availCountForFareOfStockDiscount {
                        fare = fareInfo.fare(forStockDiscount: 0)
                    } else {
                        fare = fareInfo.fare(forStockDiscount: 1)
                    }
                case Aggregate.ACADEMIC.rawValue:    // 学割
                    if fareInfo.isAcademicFare {
                        fare = fareInfo.academicFare
                    } else {
                        fare = fareInfo.fare
                    }
                case Aggregate.ACADEMIC_ROUNDTRIP.rawValue:    // 学割往復
                    if fareInfo.isAcademicFare {
                        fare = fareInfo.roundtripAcademicFare
                    } else {
                        fare = fareInfo.roundTripFareWithCompanyLine
                    }
                default:    // 無効
                    return (0, 0)
                }
                return (fare, fareInfo.totalSalesKm)
            }
        }
        return (0, 0)
    }

}



















