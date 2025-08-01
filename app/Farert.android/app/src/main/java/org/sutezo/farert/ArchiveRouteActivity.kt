package org.sutezo.farert

import android.app.AlertDialog
import android.content.ClipboardManager
import android.content.Context
import android.content.Intent
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.activity.ComponentActivity
import org.sutezo.farert.ui.theme.FarertTheme
import org.sutezo.farert.ui.compose.ArchiveRouteScreen
import org.sutezo.alps.*

/**
 * 経路保存画面
 */
class ArchiveRouteActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        setContent {
            FarertTheme {
                ArchiveRouteScreen(
                    onNavigateUp = { finish() },
                    onNavigateToMain = { routeScript ->
                        navigateToMain(routeScript)
                    },
                    onImportFromClipboard = {
                        getClipboardText()
                    },
                    onExport = { exportText ->
                        shareText(exportText)
                    },
                    onShowRouteChangeDialog = { routeScript, callback ->
                        showRouteChangeDialog(routeScript, callback)
                    }
                )
            }
        }
    }

    private fun navigateToMain(routeScript: String) {
        // 選択した経路が保存済みで先頭にない場合は先頭に移動して保存し直す
        val listItems = readParams(this, "archive_route")
        if (listItems.contains(routeScript)) {
            val s = listItems.getOrNull(0)
            if (routeScript != s) {
                val wl = listItems.toMutableList()
                wl.remove(routeScript)
                wl.add(0, routeScript)
                saveParam(this, "archive_route", wl)
            }
        }

        // メイン画面へ遷移する（選択した経路を表示する）
        val intent = Intent(this, MainActivity::class.java).apply {
            putExtra("mode", "archive")
            putExtra("script", routeScript)
            addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_SINGLE_TOP)
        }
        finish()
        startActivity(intent)
    }

    private fun getClipboardText(): String {
        val clm = getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
        return (clm.primaryClip?.getItemAt(0)?.text ?: "").toString()
    }

    private fun shareText(exportText: String) {
        val shareIntent = Intent().apply {
            action = Intent.ACTION_SEND
            type = "text/plain"
            putExtra(Intent.EXTRA_TEXT, exportText)
            putExtra(Intent.EXTRA_SUBJECT, "チケットフォルダ")
        }
        
        val chooserIntent = Intent.createChooser(
            shareIntent, 
            resources.getString(R.string.title_share_text)
        )
        
        if (chooserIntent.resolveActivity(this.packageManager) != null) {
            startActivity(chooserIntent)
        }
    }

    private fun showRouteChangeDialog(routeScript: String, callback: () -> Unit) {
        AlertDialog.Builder(this).apply {
            setTitle(R.string.main_alert_query_route_update_title)
            setMessage(R.string.main_alert_query_route_update_mesg)
            setPositiveButton("Yes") { _, _ ->
                callback()
            }
            setNegativeButton("No") { _, _ ->
                // Do nothing
            }
            setCancelable(false)
            setIcon(R.drawable.ic_question_answer)
            create()
            show()
        }
    }

}

