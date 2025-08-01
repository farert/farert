package org.sutezo.farert.ui.theme

import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.SideEffect
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.platform.LocalView
import androidx.core.view.WindowCompat

// 日本の鉄道をテーマにしたカラーパレット
private val DarkColorScheme = darkColorScheme(
    primary = Color(0xFF4FC3F7),         // JR東日本ブルー（明るめ）
    onPrimary = Color(0xFF0D47A1),       // ダークブルー
    primaryContainer = Color(0xFF1565C0),
    onPrimaryContainer = Color(0xFFE3F2FD),
    secondary = Color(0xFF81C784),       // 山手線グリーン
    onSecondary = Color(0xFF1B5E20),
    secondaryContainer = Color(0xFF388E3C),
    onSecondaryContainer = Color(0xFFE8F5E8),
    tertiary = Color(0xFFFF8A65),        // 中央線オレンジ
    onTertiary = Color(0xFFE65100),
    background = Color(0xFF121212),
    surface = Color(0xFF1E1E1E),
    onSurface = Color(0xFFE0E0E0)
)

private val LightColorScheme = lightColorScheme(
    primary = Color(0xFF1976D2),         // JR東日本ブルー
    onPrimary = Color.White,
    primaryContainer = Color(0xFFE3F2FD),
    onPrimaryContainer = Color(0xFF0D47A1),
    secondary = Color(0xFF4CAF50),       // 山手線グリーン
    onSecondary = Color.White,
    secondaryContainer = Color(0xFFE8F5E8),
    onSecondaryContainer = Color(0xFF1B5E20),
    tertiary = Color(0xFFFF6F00),        // 中央線オレンジ
    onTertiary = Color.White,
    tertiaryContainer = Color(0xFFFFE0B2),
    onTertiaryContainer = Color(0xFFE65100),
    background = Color(0xFFFAFAFA),
    surface = Color.White,
    onSurface = Color(0xFF1C1B1F),
    surfaceVariant = Color(0xFFF3F3F3),
    onSurfaceVariant = Color(0xFF49454F),
    outline = Color(0xFF79747E)
)

@Composable
fun FarertTheme(
    darkTheme: Boolean = isSystemInDarkTheme(),
    content: @Composable () -> Unit
) {
    val colorScheme = when {
        darkTheme -> DarkColorScheme
        else -> LightColorScheme
    }
    

    MaterialTheme(
        colorScheme = colorScheme,
        typography = Typography,
        content = content
    )
}