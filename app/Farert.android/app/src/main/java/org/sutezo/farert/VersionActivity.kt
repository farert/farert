package org.sutezo.farert

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.MenuItem

import kotlinx.android.synthetic.main.activity_version.*
import kotlinx.android.synthetic.main.content_version.*
import org.sutezo.alps.RouteDB



class VersionActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_version)
        setSupportActionBar(toolbar)

        // back arrow button(戻るボタン有効)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        textDbVer.text = resources.getString(R.string.db_version_name, RouteDB.getInstance().name(), RouteDB.getInstance().dbDate())
        val pkgman = this.packageManager
        val packageInfo = pkgman.getPackageInfo(this.packageName, 0)
        val ver = packageInfo.versionName
        textViewVer.text = resources.getString(R.string.version_number, ver)
        buttonUrl.setOnClickListener {
            val url = resources.getString(R.string.support_site_url_body)
            val i = Intent(Intent.ACTION_VIEW)
            i.data = Uri.parse(url)
            startActivity(i)
        }

        button_version_close.setOnClickListener {
            finish()
        }
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id = item.itemId
        if (id == android.R.id.home) {
            finish()
            return true
        }
        return super.onOptionsItemSelected(item)
    }
}
