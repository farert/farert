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

    data class Folder(val routeList: RouteList, var aggregateType: Aggregate)

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

    val aggregate_label = arrayListOf<String>("普通運賃","小児運賃","往復運賃","株割運賃","株割x2運賃","学割運賃","学割往復","無効")

    private var _routeList : MutableList<Folder> = arrayListOf()
    private var _totalFare : Int = 0
    private var _totalSalesKm : Int = 0

    fun MutableList<Folder>.swap(index_a : Int, index_b: Int) {
        val tmp = this[index_a]
        this[index_a] = this[index_b]
        this[index_b] = tmp
    }

    fun totalFare() : String {
        return if (_routeList.count() <= 0) {
            "¥---,---"
        } else {
            "¥${fareNumStr(_totalFare)}"
        }
    }

    fun totalSalesKm() : String {
        return if (_routeList.count() <= 0) {
            "---,--- km"
        } else {
            "${kmNumStr(_totalSalesKm)} km"
        }
    }

    fun add(context: Context, item : RouteList) : Boolean {
        if (MAX_HOLDER <= _routeList.count()) {
            return false
        }
        _routeList.add(Folder(item, Aggregate.NORMAL))
        calc()
        save(context)
        return true
    }

    fun swap(context: Context, from: Int, to: Int) : Boolean {
        if (from < 0 || to < 0 || _routeList.count() < from || _routeList.count() < to) {
            return false
        }
        _routeList.swap(from, to)
        calc()
        save(context)
        return true
    }

    //fun route(index : Int) -> cRoute {
    //    if index < _routeList.count {
    //        return cRoute(_routeList: _routeList[index]._routeList)
    //    } else {
    //        assert(false, "Index failure...")
    //        return cRoute()
    //   }
    //}

    fun routeItem(index : Int) : RouteList {
        return if (index < _routeList.count()) {
            _routeList[index].routeList
        } else {
            assert(false) // , "Index failure...")
            RouteList()
        }
    }

    fun routeItemFare(index : Int) : String {
        if (index < _routeList.count()) {
            val fare = calcFare(_routeList[index])
            return "¥${fareNumStr(fare.first)}"
        } else {
            assert(false)//, "Index failure...")
            return ""
        }
    }

    fun count() : Int {
        return _routeList.count()
    }

    fun aggregateType(index : Int) : Aggregate {
        if (index < _routeList.count()) {
            return _routeList[index].aggregateType
        } else {
            return Aggregate.NULLFARE
        }
    }

    public fun setAggregateType(context: Context, index : Int, aggr : Aggregate) : Unit {
        if (index < _routeList.count()) {
            _routeList[index].aggregateType = aggr
            calc()
            save(context)
        }
    }

    public fun remove(context: Context, index : Int, no_write: Boolean = false) {
        _routeList.removeAt(index)
        calc()
        if (!no_write) {
            save(context)
        }
    }

    public fun removeAll(context: Context) {
        _totalSalesKm = 0
        _totalFare = 0
        _routeList.removeAll { true }
        calc()
        save(context)
    }

    fun calc() {
        var ta : Int = 0
        var td : Int = 0
        for (route in this._routeList) {
            val result = calcFare(route)
            ta += result.first
            td += result.second
        }
        _totalFare = ta
        _totalSalesKm = td
    }

    fun save(context: Context) {
        val folders : MutableList<String> = mutableListOf()

        for (one in _routeList) {
            val rs = one.aggregateType.ordinal.toString() + "|" + one.routeList.route_script() + "\n"
            folders.add(rs)
        }
        saveParam(context, "folder", folders.toList())
    }

    fun load(context: Context) {
        val folders = readParams(context, "folder")
        _routeList.removeAll { true }

        for (o in folders) {
            val item = o.split("|")
            if (item.count() != 2) {
                continue
            }
            var ord : Int
            try {
                ord = item[0].toInt()
            } catch (e: NumberFormatException) {
                ord = 0
            }
            val agr: Routefolder.Aggregate = Routefolder.Aggregate.values()[ord]
            val script = item[1].trim { c -> c == '\n' }
            val route = Route()
            val rc = route.setup_route(script)
            if (rc < 0) {
                continue
            }
            val folder = Routefolder.Folder(route, agr)
            _routeList.add(folder)

            if (MAX_HOLDER <= _routeList.count()) {
                break
            }
        }
        calc()
    }

    fun calcFare(item : Folder) : Pair<Int, Int> {

        val cds : CalcRoute? = CalcRoute(item.routeList)
        cds?.let {
            val fareInfo = it.calcFareInfo()
            var fare : Int

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

    fun makeExportText() : String {
        var result = mutableListOf<String>()

        var ta : Int = 0
        var td : Int = 0
        for (one in this._routeList) {
            var cols = mutableListOf<String>()
            val fare = this.calcFare(one)
            ta += fare.first
            td += fare.second

            cols.add(aggregate_label[one.aggregateType.ordinal]) // 運賃
            cols.add("¥${fareNumStr(fare.first)}")
            cols.add("営業キロ")
            cols.add("${kmNumStr(fare.second)}km")

            val s = one.routeList.route_script().replace(",", " ")
            cols.add(s)
            result.add(cols.joinToString(","))
        }
        var cols = mutableListOf<String>()
        cols.add("総額")
        cols.add("¥${fareNumStr(ta)}")
        cols.add("総営業キロ")
        cols.add("${kmNumStr(td)}km")
        result.add(cols.joinToString(","))
        return result.joinToString("\n")
    }
}
    