package org.sutezo.farert

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.MenuItem
import android.view.View
import android.widget.AdapterView
import kotlinx.android.synthetic.main.activity_settings.*
import org.sutezo.alps.readParam
import org.sutezo.alps.saveParam
import org.sutezo.farert.R.id.spinner
import org.sutezo.farert.R.id.swKokuraShinZai
import java.lang.NumberFormatException

class SettingsActivity : AppCompatActivity() {

    private var mSelIndex : Int = -1
    private var mKokuraSZopt : Boolean = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_settings)
        // back arrow button(戻るボタン有効)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        mKokuraSZopt = (readParam(this, "kokura_hakata_shinzai") == "true")
        val selDatabase = readParam(this, "datasource")

        val adapter = spinner.adapter

        try {
            mSelIndex = Integer.parseInt(selDatabase)
        } catch (e: NumberFormatException) {
            mSelIndex = -1
        }

        if (mSelIndex < 0) {
            mSelIndex = adapter.count - 1
        }
        spinner.setSelection(mSelIndex)
        spinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {

            }

            override fun onNothingSelected(parent: AdapterView<*>?) {

            }
        }
    }

    override fun onStop() {
        super.onStop()

        // DB ver
        val cSelIdx = spinner.selectedItemPosition
        if (mSelIndex != cSelIdx) {
            saveParam(this, "datasource", cSelIdx.toString())
            mSelIndex = cSelIdx
            (application as FarertApp).changeDatabase(cSelIdx)
        }

        // 小倉-博多間新幹線在来線同一視
        val value = swKokuraShinZai.isChecked
        if (mKokuraSZopt != value) {
            val svalue = if (value) "true" else "false"
            saveParam(this, "kokura_hakata_shinzai", svalue)
            mKokuraSZopt = value
        }
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id = item.itemId
        when (id) {
            android.R.id.home -> {
                finish()
                return true
            }
        }
        return super.onOptionsItemSelected(item)
    }
}

