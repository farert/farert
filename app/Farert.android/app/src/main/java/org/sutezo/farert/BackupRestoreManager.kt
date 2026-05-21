package org.sutezo.farert

import android.content.Context
import org.json.JSONArray
import org.json.JSONObject
import org.sutezo.alps.MAX_ARCHIVE_ROUTE
import org.sutezo.alps.MAX_HISTORY
import org.sutezo.alps.Route
import org.sutezo.alps.readParams
import org.sutezo.alps.saveParam
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.TimeZone

/**
 * Farert 共通バックアップ JSON を生成・復元する。
 * Android 内部の保存形式とプラットフォーム共通形式の変換を担当する。
 */
object BackupRestoreManager {
    private const val VERSION = "1.0"
    private const val KEY_ARCHIVE_ROUTE = "archive_route"
    private const val KEY_HISTORY = "history"

    fun exportJson(context: Context): String {
        val app = context.applicationContext as? FarertApp
        val storage = JSONObject()
            .put("currentRoute", app?.ds?.route_script().orEmpty())
            .put("savedRoutes", JSONArray(readParams(context, KEY_ARCHIVE_ROUTE)))
            .put("ticketHolder", ticketHolderJson(context, app))
            .put("stationHistory", JSONArray(readParams(context, KEY_HISTORY)))

        return JSONObject()
            .put("version", VERSION)
            .put("exportedAt", exportedAt())
            .put("storage", storage)
            .toString(2)
    }

    fun restoreJson(context: Context, jsonText: String): RestoreResult {
        val root = JSONObject(jsonText)
        if (root.optString("version") != VERSION) {
            throw IllegalArgumentException("未対応のバックアップバージョンです")
        }

        val storage = root.getJSONObject("storage")
        val app = context.applicationContext as? FarertApp

        val currentRouteRestored = restoreCurrentRoute(app, storage.optString("currentRoute", ""))
        val savedRoutes = restoreSavedRoutes(context, storage.optJSONArray("savedRoutes"))
        val holderRoutes = restoreTicketHolder(context, app, storage.optJSONArray("ticketHolder"))
        val history = restoreStationHistory(context, storage.optJSONArray("stationHistory"))

        return RestoreResult(
            currentRouteRestored = currentRouteRestored,
            savedRoutes = savedRoutes,
            ticketHolderRoutes = holderRoutes,
            stationHistory = history
        )
    }

    private fun ticketHolderJson(context: Context, app: FarertApp?): JSONArray {
        val folder = app?.routefolder ?: Routefolder()
        val items = folder.backupItems(context)
        val array = JSONArray()
        items.forEachIndexed { index, item ->
            array.put(
                JSONObject()
                    .put("order", index + 1)
                    .put("routeScript", item.first)
                    .put("fareType", item.second)
            )
        }
        return array
    }

    private fun restoreCurrentRoute(app: FarertApp?, routeScript: String): Boolean {
        val script = routeScript.trim()
        if (app == null || script.isEmpty()) {
            return false
        }
        val route = Route()
        if (route.setup_route(script) < 0) {
            return false
        }
        app.ds.removeAll()
        return app.ds.setup_route(script) >= 0
    }

    private fun restoreSavedRoutes(context: Context, routes: JSONArray?): Int {
        val restored = validRouteScripts(routes, MAX_ARCHIVE_ROUTE)
        saveParam(context, KEY_ARCHIVE_ROUTE, restored)
        return restored.size
    }

    private fun restoreTicketHolder(context: Context, app: FarertApp?, items: JSONArray?): Int {
        val holderItems = mutableListOf<Pair<String, String>>()
        if (items != null) {
            for (i in 0 until items.length()) {
                val item = items.optJSONObject(i) ?: continue
                holderItems.add(Pair(item.optString("routeScript"), item.optString("fareType")))
            }
        }
        val folder = app?.routefolder ?: Routefolder()
        return folder.restoreBackupItems(context, holderItems)
    }

    private fun restoreStationHistory(context: Context, history: JSONArray?): Int {
        val restored = mutableListOf<String>()
        if (history != null) {
            for (i in 0 until history.length()) {
                val station = history.optString(i).trim()
                if (station.isNotEmpty() && !restored.contains(station)) {
                    restored.add(station)
                }
                if (MAX_HISTORY <= restored.size) {
                    break
                }
            }
        }
        saveParam(context, KEY_HISTORY, restored)
        return restored.size
    }

    private fun validRouteScripts(routes: JSONArray?, max: Int): List<String> {
        val restored = mutableListOf<String>()
        if (routes != null) {
            for (i in 0 until routes.length()) {
                val script = routes.optString(i).trim()
                if (script.isEmpty() || restored.contains(script)) {
                    continue
                }
                val route = Route()
                if (route.setup_route(script) >= 0) {
                    restored.add(route.route_script())
                }
                if (max <= restored.size) {
                    break
                }
            }
        }
        return restored
    }

    private fun exportedAt(): String {
        val formatter = SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'", Locale.US)
        formatter.timeZone = TimeZone.getTimeZone("UTC")
        return formatter.format(Date())
    }

    data class RestoreResult(
        val currentRouteRestored: Boolean,
        val savedRoutes: Int,
        val ticketHolderRoutes: Int,
        val stationHistory: Int
    )
}
