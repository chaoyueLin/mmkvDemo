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


        System.out.println("MMKV string: " + mmkv.decodeString("string"))
        binding.bMmap.setOnClickListener {
//            mmkv.encode("bool", true)
            mmkv.encode("string", "Hello from mmkv")
//            mmkv.encode("int", Int.MIN_VALUE)
//
//            mmkv.encode("long", Long.MAX_VALUE)
//
//            mmkv.encode("float", -3.14f);
//
//            mmkv.encode("double", Double.MIN_VALUE);


        }
    }


}