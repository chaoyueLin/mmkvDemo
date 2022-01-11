package com.example.cmmkv

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.chaoyue.cmmkv.MMKV
import com.example.cmmkv.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method

        MMKV.initialize(this.applicationContext)
//        binding.sampleText.text = mmkv.stringFromJNI()

        val mmkv = MMKV.mmkvWithID("unitTest")

        binding.bMmap.setOnClickListener {
//            mmkv.encode("bool", true)
            System.out.println("bool: " + mmkv.decodeBool("bool"))
        }
    }


}