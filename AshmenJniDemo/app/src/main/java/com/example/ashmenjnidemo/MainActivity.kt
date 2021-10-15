package com.example.ashmenjnidemo

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    var edpos: EditText? = null
    var edval:EditText? = null
    var bn: Button? = null
    var tv: TextView? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        ShmLib.OpenSharedMem("sh1", 1000, true)

        edpos = findViewById(R.id.ed2) as EditText
        edval = findViewById(R.id.ed) as EditText

        tv = findViewById(R.id.tv) as TextView
        bn = findViewById(R.id.btnSet) as Button
        bn!!.setOnClickListener { ShmLib.setValue("sh1", edpos!!.text.toString().toInt(), edval!!.getText().toString().toInt()) }
        val bget = findViewById(R.id.btnGet) as Button
        bget.setOnClickListener {
            val res = ShmLib.getValue("sh1", edpos!!.text.toString().toInt())
            tv!!.text = "res:$res"
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}