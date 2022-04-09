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

    data class Folder(val routeList: RouteList, var aggregateType: Aggregate, var fare: Int? = null, var salesKm: Int? = null)

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

    private val aggregate_label = arrayListOf("普通運賃","小児運賃","往復運賃","株割運賃","株割x2運賃","学割運賃","学割往復","無効")

    private var _routeList : MutableList<Folder> = arrayListOf()
    private var _totalFare : Int = 0
    private var _totalSalesKm : Int = 0

    private fun MutableList<Folder>.swap(index_a : Int, index_b: Int) {
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

    fun routeItemFareKm(index : Int) : Pair<Int, Int> {
        return if (index < _routeList.count()) {
            if ((_routeList[index].fare != null) && (_routeList[index].salesKm != null)) {
                Pair(_routeList[index].fare!!, _routeList[index].salesKm!!)
            } else {
                val fare = calcFare(_routeList[index])
                fare
            }
        } else {
            assert(false)//, "Index failure...")
            Pair(0, 0)
        }
    }

    fun count() : Int {
        return _routeList.count()
    }

    fun aggregateType(index : Int) : Aggregate {
        return if (index < _routeList.count()) {
            _routeList[index].aggregateType
        } else {
            Aggregate.NULLFARE
        }
    }

    fun setAggregateType(context: Context, index : Int, aggr : Aggregate) {
        if ((index < _routeList.count()) && (_routeList[index].aggregateType != aggr)) {
            _routeList[index].aggregateType = aggr
            // do re-calc fare
            _routeList[index].fare = null
            _routeList[index].salesKm = null

            calc()
            save(context)
        }
    }

    fun remove(context: Context, index : Int, no_write: Boolean = false) {
        _routeList.removeAt(index)
        calc()
        if (!no_write) {
            save(context)
        }
    }

    fun removeAll(context: Context) {
        _totalSalesKm = 0
        _totalFare = 0
        _routeList.removeAll { true }
        calc()
        save(context)
    }

    private fun calc() {
        var fare_sum = 0
        var salesKm_sum = 0
        for (route in this._routeList) {
            if ((route.fare != null) && (route.salesKm != null)) {
                fare_sum += route.fare!!
                salesKm_sum += route.salesKm!!
            } else {
                val result = calcFare(route)
                fare_sum += result.first
                salesKm_sum += result.second
                route.fare = result.first
                route.salesKm = result.second
            }
        }
        _totalFare = fare_sum
        _totalSalesKm = salesKm_sum
    }

    fun save(context: Context) {
        val folders : MutableList<String> = mutableListOf()
        val dbver = DatabaseOpenHelper.DATABASE_VERSION
        val strDbVer = "DBVer|$dbver\n"
        folders.add(strDbVer)
        for (one in _routeList) {
            val rs = if ((one.fare != null) && (one.salesKm != null)) {
                one.aggregateType.ordinal.toString() + "|" +
                        one.routeList.route_script() + "|" +
                        one.fare.toString() + "|" + one.salesKm.toString() + "\n"
            } else {
                one.aggregateType.ordinal.toString() + "|" +
                        one.routeList.route_script() + "\n"
            }
            folders.add(rs)
        }
        saveParam(context, "folder", folders.toList())
    }

    fun load(context: Context, doCalc: Boolean? = false) {
        val folders = readParams(context, "folder")
        var isDbChanged : Boolean = doCalc ?: false
        _routeList.removeAll { true }

        for (o in folders) {
            val item = o.split("|")
            val nCol = item.count()
            if ((nCol != 2) && (nCol != 4)) {
                continue
            }
            if ((nCol == 2) && (0 == _routeList.count())) {
                if (item[0] == "DBVer") {
                    val dbver = DatabaseOpenHelper.DATABASE_VERSION
                    val folderDbVer = item[1].trim { c -> c == '\n'}.toInt()
                    if (dbver != folderDbVer) {
                        isDbChanged = true
                    }
                    continue
                }
            }
            val ord : Int = try {
                item[0].toInt()
            } catch (e: NumberFormatException) {
                0
            }
            val agr: Aggregate = Aggregate.values()[ord]
            val script = item[1].trim { c -> c == '\n' }
            val route = Route()
            val rc = route.setup_route(script)
            if (rc < 0) {
                continue
            }
            var fare : Int? = null
            var salesKm : Int? = null
            if (!isDbChanged && (nCol == 4)) {
                fare = Integer.parseInt(item[2])
                salesKm = Integer.parseInt(item[3].trim { c -> c == '\n' })
            }
            val folder = Folder(route, agr, fare, salesKm)
            _routeList.add(folder)

            if (MAX_HOLDER <= _routeList.count()) {
                break
            }
        }
        calc()
    }

    fun calcFare(item : Folder) : Pair<Int, Int> {

        val cds = CalcRoute(item.routeList)
        cds.let {
            val fareInfo = it.calcFareInfo()
            val fare : Int

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
                    fare = if (fareInfo.isAcademicFare) {
                        fareInfo.academicFare
                    } else {
                        fareInfo.fare
                    }
                }
                Aggregate.ACADEMIC_ROUNDTRIP -> {    // 学割往復
                    fare = if (fareInfo.isAcademicFare) {
                        fareInfo.roundtripAcademicFare
                    } else {
                        fareInfo.roundTripFareWithCompanyLine
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
        val result = mutableListOf<String>()

        var fare_sum = 0
        var salesKm_sum = 0
        for (route in this._routeList) {
            val cols = mutableListOf<String>()
            var fare : Int
            var salesKm : Int
            if ((route.fare != null) && (route.salesKm != null)) {
                fare = route.fare!!
                salesKm = route.salesKm!!
            } else {
                val fare_salesKm: Pair<Int, Int> = this.calcFare(route)
                fare = fare_salesKm.first
                salesKm = fare_salesKm.second
            }
            fare_sum += fare
            salesKm_sum += salesKm

            cols.add(aggregate_label[route.aggregateType.ordinal]) // 運賃
            cols.add("¥${fareNumStr(fare)}")
            cols.add("営業キロ")
            cols.add("${kmNumStr(salesKm)}km")

            val s = route.routeList.route_script().replace(",", " ")
            cols.add(s)
            result.add(cols.joinToString(","))
        }
        val cols = mutableListOf<String>()
        cols.add("総額")
        cols.add("¥${fareNumStr(fare_sum)}")
        cols.add("総営業キロ")
        cols.add("${kmNumStr(salesKm_sum)}km")
        result.add(cols.joinToString(","))
        return result.joinToString("\n")
    }
}
    