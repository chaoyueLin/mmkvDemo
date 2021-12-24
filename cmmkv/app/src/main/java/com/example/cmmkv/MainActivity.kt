package com.example.cmmkv

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.example.chaoyue.cmmkv.MMKV
import com.example.cmmkv.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        val mmkv: MMKV = MMKV()
        binding.sampleText.text = mmkv.stringFromJNI()
        mmkv.testLog()
        Thread {
            mmkv.testLock()

        }.start()
    }


}