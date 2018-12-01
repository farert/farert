package org.sutezo.farert

import android.content.Context
import org.sutezo.alps.*

//
//  Routefolder.swift
//  Farert
//
//  Created by TAKEDA, Noriyuki on 2017/04/25.
//  Copyright © 2017年 TAKEDA, Noriyuki. All rights reserved.
//



class Routefolder {

    class Folder(val routeList : RouteList = RouteList(),
                var aggregateType : Aggregate = Aggregate.NORMAL) 

    //private static let onlyObj = Routefolder()
    
    enum class Aggregate {
        NORMAL,   // 普通運賃
        CHILD,     // 小児運賃
        ROUNDTRIP, // 往復
        STOCK,     // 株割
        STOCKW,    // 株割り4割
        ACADEMIC,   // 学割
        ACADEMIC_ROUNDTRIP,   // 学割往復
        NULLFARE,   // 無効
    }

    private var routeList : MutableList<Folder> = arrayListOf<Folder>()
    private var _totalFare : Int = 0
    private var _totalSalesKm : Int = 0

    public fun totalFare() : String {
        return if (routeList.count() <= 0) {
            "¥---,---"
        } else {
            "¥${fareNumStr(_totalFare)}"
        }
    }

    public fun totalSalesKm() : String {
        return if (routeList.count() <= 0) {
            "---,--- km"
        } else {
            "${kmNumStr(_totalSalesKm)} km"
        }
    }

    public fun add(context: Context, item : RouteList) : Boolean {
        if (MAX_HOLDER <= routeList.count()) {
            return false
        }
        routeList.add(Folder(item, Aggregate.NORMAL))
        calc()
        save(context)
        return true
    }

    //fun route(index : Int) -> cRoute {
    //    if index < routeList.count {
    //        return cRoute(routeList: routeList[index].routeList)
    //    } else {
    //        assert(false, "Index failure...")
    //        return cRoute()
    //   }
    //}

    fun routeItem(index : Int) : RouteList {
        return if (index < routeList.count()) {
            routeList[index].routeList
        } else {
            assert(false) // , "Index failure...")
            RouteList()
        }
    }

    fun routeItemFare(index : Int) : String {
        if (index < routeList.count()) {
            val fare = calcFare(routeList[index])
            return "¥${fareNumStr(fare.first)}"
        } else {
            assert(false)//, "Index failure...")
            return ""
        }
    }

    fun count() : Int {
        return routeList.count()
    }

    fun aggregateType(index : Int) : Aggregate {
        if (index < routeList.count()) {
            return routeList[index].aggregateType
        } else {
            return Aggregate.NULLFARE
        }
    }

    public fun setAggregateType(context: Context, index : Int, aggr : Aggregate) : Unit {
        if (index < routeList.count()) {
            routeList[index].aggregateType = aggr
            calc()
            save(context)
        }
    }

    public fun remove(context: Context, index : Int) {
        routeList.removeAt(index)
        calc()
        save(context)
    }

    public fun removeAll(context: Context) {
        _totalSalesKm = 0
        _totalFare = 0
        routeList.removeAll { true }
        calc()
        save(context)
    }

    /*  Exchange content index_a and index b
     *
     */
    fun exchange(context: Context, index_source : Int, index_destination : Int) {
        val obj = routeList[index_source]
        routeList.removeAt(index_source)
        routeList.add(index_destination, obj)
        save(context)
    }


    fun calc() {
        var ta : Int = 0
        var td : Int = 0
        for (route in this.routeList) {
            val result = calcFare(route)
            ta += result.first
            td += result.second
        }
        _totalFare = ta
        _totalSalesKm = td
    }

    fun save(context: Context) {
        val folders : MutableList<String> = mutableListOf()

        for (one in routeList) {
            val rs = one.aggregateType.ordinal.toString() + "|" + one.routeList.route_script() + "\n"
            folders.add(rs)
        }
        saveParam(context, "folder", folders.toList())
    }

    fun load(context: Context) {
        val folders = readParams(context, "folder")
        routeList.removeAll { true }

        for (o in folders) {
            val item = o.split("|")
            if (item.count() != 2) {
                continue
            }
            var ord: Int = 0
            try {
                ord = item[0].toInt()
            } catch (e: NumberFormatException) {
                ord = 0
            }
            val agr: Routefolder.Aggregate = Routefolder.Aggregate.values()[ord]
            val script = item[1]
            val route = Route()
            val rc = route.setup_route(script)
            if (rc < 0) {
                continue
            }
            val folder = Routefolder.Folder(route, agr)
            routeList.add(folder)

            if (MAX_HOLDER <= routeList.count()) {
                break
            }
        }
        calc()
    }

        fun calcFare(item : Folder) : Pair<Int, Int> {

            val cds : CalcRoute? = CalcRoute(item.routeList)
            cds?.let {
                val fareInfo = it.calcFareInfo()
                var fare : Int = 0

                when (item.aggregateType) {
                    Aggregate.NORMAL -> {   // 普通運賃
                        fare = fareInfo.fare
                    }
                    Aggregate.CHILD -> {     // 小児運賃
                        fare = fareInfo.childFare
                    }
                    Aggregate.ROUNDTRIP -> { // 往復
                        fare = fareInfo.roundTripFareWithCompanyLine
                    }
                    Aggregate.STOCK -> {     // 株割
                        val stocks = fareInfo.fareForStockDiscounts ?: listOf()
                            fare =
                                if (1 <= stocks.count()) {
                                    // title, normal_fare, 114applied_fare
                                    if (fareInfo.isRule114Applied) stocks[0].third else stocks[0].second    // thirdは114適用
                                } else {
                                    fareInfo.fare
                                }
                    }
                    Aggregate.STOCKW -> {    // 株割り4割
                        val stocks = fareInfo.fareForStockDiscounts ?: listOf()
                        fare =
                            if (2 <= stocks.count()) {
                                if (fareInfo.isRule114Applied) stocks[1].third else stocks[1].second
                            } else if (1 <= stocks.count()) {
                                if (fareInfo.isRule114Applied) stocks[0].third else stocks[0].second
                            } else {
                                fareInfo.fare
                            }
                    }
                    Aggregate.ACADEMIC -> {    // 学割
                        if (fareInfo.isAcademicFare) {
                            fare = fareInfo.academicFare
                        } else {
                            fare = fareInfo.fare
                        }
                    }
                    Aggregate.ACADEMIC_ROUNDTRIP -> {    // 学割往復
                        if (fareInfo.isAcademicFare) {
                            fare = fareInfo.roundtripAcademicFare
                        } else {
                            fare = fareInfo.roundTripFareWithCompanyLine
                        }
                    }
                    else -> {    // 無効
                        return Pair(0, 0)
                    }
                }
                return Pair(fare, fareInfo.totalSalesKm)
            }
            return Pair(0, 0)
        }
    }
    