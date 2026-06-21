package org.sutezo.farert

import android.content.Context
import org.json.JSONArray
import org.json.JSONObject
import org.sutezo.alps.MAX_ARCHIVE_ROUTE
import org.sutezo.alps.MAX_HOLDER
import org.sutezo.alps.MAX_HISTORY
import org.sutezo.alps.Route
import org.sutezo.alps.readParams
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

    fun validateJson(jsonText: String) {
        if (jsonText.isBlank()) {
            throw IllegalArgumentException("クリップボードにバックアップ JSON がありません")
        }
        val root = try {
            JSONObject(jsonText)
        } catch (e: Exception) {
            throw IllegalArgumentException("クリップボードの内容は JSON として読み込めません")
        }
        if (root.optString("version") != VERSION) {
            throw IllegalArgumentException("未対応のバックアップバージョンです")
        }
        if (!root.has("storage") || root.optJSONObject("storage") == null) {
            throw IllegalArgumentException("Farert のバックアップ JSON ではありません")
        }
    }

    fun restoreJson(context: Context, jsonText: String): RestoreResult {
        validateJson(jsonText)
        val root = JSONObject(jsonText)
        val storage = root.getJSONObject("storage")
        val app = context.applicationContext as? FarertApp
        val snapshot = RestoreSnapshot(
            currentRoute = app?.ds?.route_script().orEmpty(),
            savedRoutes = readParams(context, KEY_ARCHIVE_ROUTE),
            ticketHolder = (app?.routefolder ?: Routefolder()).backupItems(context),
            stationHistory = readParams(context, KEY_HISTORY)
        )

        val currentRoute = validRouteScriptOrEmpty(storage.optString("currentRoute", ""))
        val savedRoutes = validRouteScripts(storage.optJSONArray("savedRoutes"), MAX_ARCHIVE_ROUTE)
        val holderItems = validTicketHolderItems(storage.optJSONArray("ticketHolder"))
        val history = validStationHistory(storage.optJSONArray("stationHistory"))

        try {
            if (!saveStringList(context, KEY_ARCHIVE_ROUTE, savedRoutes)) {
                throw IllegalStateException("保存経路を書き込めませんでした")
            }
            val holderRoutes = restoreTicketHolder(context, app, holderItems)
            if (!saveStringList(context, KEY_HISTORY, history)) {
                throw IllegalStateException("履歴を書き込めませんでした")
            }
            val currentRouteRestored = restoreCurrentRoute(app, currentRoute)

            return RestoreResult(
                currentRouteRestored = currentRouteRestored,
                savedRoutes = savedRoutes.size,
                ticketHolderRoutes = holderRoutes,
                stationHistory = history.size
            )
        } catch (e: Exception) {
            rollback(context, app, snapshot)
            throw e
        }
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
        app.ds.removeAll()
        return app.ds.setup_route(script) >= 0
    }

    private fun restoreTicketHolder(
        context: Context,
        app: FarertApp?,
        holderItems: List<Pair<String, String>>
    ): Int {
        val folder = app?.routefolder ?: Routefolder()
        return folder.restoreBackupItems(context, holderItems)
    }

    private fun validStationHistory(history: JSONArray?): List<String> {
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
        return restored
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

    private fun validTicketHolderItems(items: JSONArray?): List<Pair<String, String>> {
        val holderItems = mutableListOf<Pair<String, String>>()
        if (items != null) {
            for (i in 0 until items.length()) {
                val item = items.optJSONObject(i) ?: continue
                val routeScript = validRouteScriptOrEmpty(item.optString("routeScript"))
                if (routeScript.isNotEmpty()) {
                    holderItems.add(Pair(routeScript, item.optString("fareType")))
                }
                if (MAX_HOLDER <= holderItems.size) {
                    break
                }
            }
        }
        return holderItems
    }

    private fun validRouteScriptOrEmpty(routeScript: String): String {
        val script = routeScript.trim()
        if (script.isEmpty()) {
            return ""
        }
        val route = Route()
        return if (route.setup_route(script) >= 0) route.route_script() else ""
    }

    private fun rollback(context: Context, app: FarertApp?, snapshot: RestoreSnapshot) {
        saveStringList(context, KEY_ARCHIVE_ROUTE, snapshot.savedRoutes)
        restoreTicketHolder(context, app, snapshot.ticketHolder)
        saveStringList(context, KEY_HISTORY, snapshot.stationHistory)
        restoreCurrentRoute(app, snapshot.currentRoute)
    }

    private fun saveStringList(context: Context, key: String, values: List<String>): Boolean {
        val sharedPref = context.getSharedPreferences("settings", Context.MODE_PRIVATE)
        return sharedPref.edit()
            .putString(key, JSONArray(values).toString())
            .commit()
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

    private data class RestoreSnapshot(
        val currentRoute: String,
        val savedRoutes: List<String>,
        val ticketHolder: List<Pair<String, String>>,
        val stationHistory: List<String>
    )
}
