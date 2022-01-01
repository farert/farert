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

public let aggregate_label = [
"普通運賃", "小児運賃", "往復運賃", "株割運賃", "株割x2運賃",
                    "学割運賃", "学割往復" , "無効"]

struct folder {
    var routeList : cRouteList
    var indexOfAggregate : Int
    var fare : Int?
    var salesKm : Int?
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

    func routeItemFare(index : Int) -> (fare : String, salesKm : String) {
        if index < routeList.count {
            var fare : Int
            var salesKm : Int

            if ((routeList[index].fare != nil) && (routeList[index].salesKm != nil)) {
                fare = routeList[index].fare!
                salesKm = routeList[index].salesKm!
            } else {
                let fareSalesKm = self.calcFare(item: routeList[index])
                fare = fareSalesKm.fare
                salesKm = fareSalesKm.sales_km
            }
            let formatter = NumberFormatter()
            formatter.numberStyle = NumberFormatter.Style.decimal
            formatter.groupingSeparator = ","
            formatter.groupingSize = 3
            return ("¥\(formatter.string(from: NSNumber(value: fare)) ?? "")-",
                    "\(String(describing: cRouteUtil.kmNumStr(salesKm)!)) km")
        } else {
            assert(false, "Index failure...")
            return ("", "")
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
        if ((index < routeList.count) && (routeList[index].indexOfAggregate != aggr)) {
            routeList[index].indexOfAggregate = aggr
            // do re-calc fare
            routeList[index].fare = nil
            routeList[index].salesKm = nil
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
        var fare_sum : Int = 0
        var salesKm_sum : Int = 0
        for (i, route) in self.routeList.enumerated() {
            if ((route.fare != nil) && (route.salesKm != nil)) {
                fare_sum += route.fare!
                salesKm_sum += route.salesKm!
            } else {
                let result = self.calcFare(item: route)
                fare_sum += result.fare
                salesKm_sum += result.sales_km

                self.routeList[i].fare = result.fare
                self.routeList[i].salesKm = result.sales_km
            }
        }
        self._totalFare = fare_sum
        self._totalSalesKm = salesKm_sum
    }
    
    private var filePath : String {
        get {
            let documentsPath = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0] as String
            return documentsPath + "/routeholder.txt"
        }
    }

    func save() {
        //print(filePath)
        if let os = OutputStream(toFileAtPath: filePath, append: false) {
            os.open()
            let dbver = cRouteUtil.getDatabaseId().rawValue
            let strDbVer = "DBVer|" + String(dbver) + "\n"
            os.write(strDbVer, maxLength: strDbVer.count)
            for one in routeList {
                let rs : String
                if ((one.fare != nil) && (one.salesKm != nil)) {
                    rs = String(one.indexOfAggregate) + "|" + one.routeList.routeScript()
                     + "|" + String(one.fare!) + "|" + String(one.salesKm!) + "\n"
                } else {
                    rs = String(one.indexOfAggregate) + "|" + one.routeList.routeScript() + "\n"
                }
                os.write(rs, maxLength: rs.lengthOfBytes(using: String.Encoding.utf8))
            }
            os.close()
        }
    }
    
    func load(_ doCalc : Bool? = nil) {
        do {
            self.routeList.removeAll()
            var num : Int = 0
            var isDbChanged : Bool = doCalc ?? false
            let text = try String(contentsOf: URL(fileURLWithPath: filePath), encoding: String.Encoding.utf8)
            text.enumerateLines { line, stop in
                if num < Int(MAX_ARCHIVE_ROUTE) {
                    let record : [String] = line.components(separatedBy: "|")
                    if 2 <= record.count {
                        if ((num == 0) && ("DBVer" == String(record[0]))) {
                            let dbVer = cRouteUtil.getDatabaseId().rawValue
                            if dbVer != Int(record[1]) {
                                isDbChanged = true
                            }
                        } else {
                            if let agr : Int = Int(record[0]) {
                                if let rt = cRoute() {
                                    let result = rt.setupRoute(record[1])
                                    if 0 <= result {
                                        if (!isDbChanged && (4 == record.count)) {
                                            let fare = Int(record[2])
                                            let salesKm = Int(record[3])
                                            self.routeList.append(
                                                folder(routeList: cRouteList(route: rt),
                                                       indexOfAggregate: agr,
                                                       fare: fare,
                                                       salesKm: salesKm))
                                        } else {
                                            self.routeList.append(folder(routeList: cRouteList(route: rt), indexOfAggregate: agr))
                                        }
                                        num += 1
                                    }
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
                        // N/A
                        fare = fareInfo.fare
                    } else {
                        if (fareInfo.isRule114Applied) {
                            //株割 有効 114条有効
                            fare = fareInfo.fare(forStockDiscount: 0+2)
                        } else {
                            //株割 有効 114条無効
                            fare = fareInfo.fare(forStockDiscount: 0)
                        }
                    }
                case Aggregate.STOCKW.rawValue:    // 株割り4割
                    if fareInfo.availCountForFareOfStockDiscount < 1 {
                        // N/A
                        fare = fareInfo.fare
                    } else if 1 < fareInfo.availCountForFareOfStockDiscount {
                        // 株割4
                        if (fareInfo.isRule114Applied) {
                            fare = fareInfo.fare(forStockDiscount: 1+2)
                        } else {
                            fare = fareInfo.fare(forStockDiscount: 1)
                        }
                    } else {
                        // 株割4は無効で株割2のみ有効
                        if (fareInfo.isRule114Applied) {
                            fare = fareInfo.fare(forStockDiscount: 0+2)
                        } else {
                            fare = fareInfo.fare(forStockDiscount: 0)
                        }
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
    
    func conmaNumStr(_ num : Int) -> String {
        let formatter = NumberFormatter()
        formatter.numberStyle = NumberFormatter.Style.decimal
        formatter.groupingSeparator = ","
        formatter.groupingSize = 3
        return "¥\(formatter.string(from: NSNumber(value: num)) ?? "")"
    }
    
    func makeExportText() -> String {
        var result : String = ""
        
        var fare_sum : Int = 0
        var salesKm_sum : Int = 0
        for folder_ in self.routeList {
            let fare : Int
            let salesKm : Int
            if ((folder_.salesKm != nil) && (folder_.fare != nil)) {
                fare = folder_.fare!
                salesKm = folder_.salesKm!
            } else {
                let fare_ = self.calcFare(item: folder_)
                fare = fare_.fare
                salesKm = fare_.sales_km
            }
            var cols = Array<String>()
            fare_sum += fare
            salesKm_sum += salesKm

            cols.append(aggregate_label[folder_.indexOfAggregate]) // 運賃
            cols.append(conmaNumStr(fare))
            cols.append("営業キロ")
            cols.append("\(String(describing: cRouteUtil.kmNumStr(salesKm_sum)!)) km")

            let s = folder_.routeList.routeScript()?.replacingOccurrences(of: ",", with: " ", options: .literal, range: nil) ?? ""
            cols.append(s)
            result += (cols.joined(separator: ",") + "\n")
        }
        var cols = Array<String>()
        cols.append("総額")
        cols.append(conmaNumStr(fare_sum))
        cols.append("総営業キロ")
        cols.append("\(String(describing: cRouteUtil.kmNumStr(salesKm_sum)!)) km")
        result += (cols.joined(separator: ",") + "\n")
        return result
    }

}
