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
}



struct folder {
    var routeList : cRouteList
    var indexOfAggregate : Int
}

public class Routefolder {
    
    //private static let onlyObj = Routefolder()
    
    private var routeList : [folder] = []
    
    func add(item : cRouteList) {
        routeList.append(folder(routeList: item, indexOfAggregate: Aggregate.NORMAL.rawValue))
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
    
    func remove(index : Int) {
        routeList.remove(at: index)
    }
    
    /*  Exchange content index_a and index b
     *
     */
    func exchange(index_a : Int, index_b : Int) {
        routeList.insert(routeList[index_b], at: index_a)
        routeList.remove(at: index_a + 1)
        routeList.insert(routeList[index_a], at: index_b)
        routeList.remove(at: index_b + 1)
    }
    
    //class func data() -> Routefolder{
    //    return Routefolder.onlyObj
    //}
}
