package org.sutezo.farert

import android.annotation.SuppressLint
import android.content.Intent
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.activity.ComponentActivity
import android.widget.Toast
import org.sutezo.farert.ui.theme.FarertTheme
import org.sutezo.farert.ui.compose.ResultViewScreen
import org.sutezo.farert.ui.state.ResultViewUiState

/**
 * 運賃詳細結果ビューActivivty
 */
class ResultViewActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        // Extract parameters from intent
        val routeEndIndex = intent.getIntExtra("arrive", -1)
        val alertId = intent.getIntExtra("alert_title", -1)
        val stocktokai = intent.getIntExtra("stocktokai", ResultViewUiState.Option.N_A.ordinal)
        val neerest = intent.getIntExtra("neerest", ResultViewUiState.Option.N_A.ordinal)
        val sperule = intent.getIntExtra("sperule", ResultViewUiState.Option.N_A.ordinal)
        val meihancity = intent.getIntExtra("meihancity", ResultViewUiState.Option.N_A.ordinal)
        val longroute = intent.getIntExtra("longroute", ResultViewUiState.Option.N_A.ordinal)
        val rule115 = intent.getIntExtra("rule115", ResultViewUiState.Option.N_A.ordinal)
        
        setContent {
            FarertTheme {
                ResultViewScreen(
                    routeEndIndex = routeEndIndex,
                    alertId = alertId,
                    stocktokai = stocktokai,
                    neerest = neerest,
                    sperule = sperule,
                    meihancity = meihancity,
                    longroute = longroute,
                    rule115 = rule115,
                    onNavigateUp = { finish() },
                    onShare = { subject, text ->
                        shareText(subject, text)
                    }
                )
            }
        }
    }


    /**
     *  外部エクスポート用結果テキストをエクスポートする
     */
    private fun shareText(subject: String, text: String) {
        val shareIntent = Intent().apply {
            action = Intent.ACTION_SEND
            type = "text/plain"
            putExtra(Intent.EXTRA_TEXT, text)
            putExtra(Intent.EXTRA_SUBJECT, subject)
        }
        
        val chooserIntent = Intent.createChooser(
            shareIntent, 
            resources.getString(R.string.title_share_text)
        )
        
        if (chooserIntent.resolveActivity(packageManager) != null) {
            startActivity(chooserIntent)
        }
    }
}
