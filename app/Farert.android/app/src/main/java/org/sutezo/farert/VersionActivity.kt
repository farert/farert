package org.sutezo.farert

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.appcompat.app.AppCompatActivity
import androidx.compose.foundation.layout.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.sutezo.alps.RouteDB
import org.sutezo.farert.ui.theme.FarertTheme
import org.sutezo.farert.ui.components.farertTopAppBarColors

class VersionActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        setContent {
            FarertTheme {
                VersionScreen(
                    onNavigateUp = { finish() }
                )
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun VersionScreen(
    onNavigateUp: () -> Unit
) {
    val context = LocalContext.current
    
    val packageInfo = remember {
        context.packageManager.getPackageInfo(context.packageName, 0)
    }
    
    val versionText = stringResource(R.string.version_number, packageInfo.versionName ?: "不明")
    val dbVersionText = stringResource(
        R.string.db_version_name,
        RouteDB.getInstance().name(),
        RouteDB.getInstance().dbDate(),
        RouteDB.getInstance().tax()
    )
    
    Column(
        modifier = Modifier
            .fillMaxSize()
    ) {
        TopAppBar(
            title = {
                Text(stringResource(R.string.title_version_info))
            },
            navigationIcon = {
                IconButton(onClick = onNavigateUp) {
                    Icon(
                        imageVector = Icons.AutoMirrored.Filled.ArrowBack,
                        contentDescription = "戻る"
                    )
                }
            },
            colors = farertTopAppBarColors()
        )
        
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(16.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.spacedBy(16.dp)
        ) {
            Spacer(modifier = Modifier.height(16.dp))
            
            Text(
                text = versionText,
                textAlign = TextAlign.Center,
                modifier = Modifier.fillMaxWidth()
            )
            
            Text(
                text = stringResource(R.string.title_version_info),
                fontSize = 20.sp,
                fontWeight = FontWeight.Bold,
                textAlign = TextAlign.Center
            )
            
            Text(
                text = dbVersionText,
                textAlign = TextAlign.Center,
                modifier = Modifier.padding(horizontal = 8.dp)
            )
            
            Text(
                text = stringResource(R.string.copyright),
                textAlign = TextAlign.Center
            )
            
            Text(
                text = stringResource(R.string.eula),
                textAlign = TextAlign.Center,
                modifier = Modifier
                    .padding(horizontal = 8.dp)
                    .widthIn(max = 280.dp)
            )
            
            Spacer(modifier = Modifier.weight(1f))
            
            TextButton(
                onClick = {
                    val url = context.getString(R.string.support_site_url_body)
                    val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
                    context.startActivity(intent)
                }
            ) {
                Text(
                    text = stringResource(R.string.support_site_url),
                    fontWeight = FontWeight.Bold
                )
            }
            
            Button(
                onClick = onNavigateUp,
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(bottom = 8.dp)
            ) {
                Text(
                    text = stringResource(android.R.string.VideoView_error_button),
                    fontWeight = FontWeight.Bold,
                    fontSize = 16.sp
                )
            }
        }
    }
}
