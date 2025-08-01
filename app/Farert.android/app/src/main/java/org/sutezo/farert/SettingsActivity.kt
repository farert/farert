package org.sutezo.farert

import android.app.Activity
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.activity.ComponentActivity
import org.sutezo.farert.ui.theme.FarertTheme
import org.sutezo.farert.ui.compose.SettingsScreen

class SettingsActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        setContent {
            FarertTheme {
                SettingsScreen(
                    onNavigateUp = { hasChanges ->
                        val resultCode = if (hasChanges) Activity.RESULT_OK else Activity.RESULT_CANCELED
                        setResult(resultCode, intent)
                        finish()
                    }
                )
            }
        }
    }
}

