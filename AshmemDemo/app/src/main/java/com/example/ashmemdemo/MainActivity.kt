package com.example.ashmemdemo

import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.os.Bundle
import android.os.IBinder
import android.os.Parcel
import android.util.Log
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import java.io.BufferedReader
import java.io.FileDescriptor
import java.io.FileReader


class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val text = findViewById<TextView>(R.id.tv)
        //绑定服务
        //绑定服务
        val intent = Intent(this, RemoteService::class.java)
        bindService(intent, object : ServiceConnection {
            override fun onServiceConnected(name: ComponentName, service: IBinder) {
                val data = Parcel.obtain()
                val reply = Parcel.obtain()
                try {
                    //通过binder机制跨进程调用服务端的接口
                    service.transact(1, data, reply, 0)
                    //获得RemoteService创建的匿名共享内存的fd
                    val fd: FileDescriptor = reply.readFileDescriptor().fileDescriptor
                    //读取匿名共享内存中的数据，并打印log
                    val br = BufferedReader(FileReader(fd))
                    val result = br.readLine()
                    Log.v("kobe-result", result)
                    text.post { text.text = result }
                } catch (e: Exception) {
                    e.printStackTrace()
                }
            }

            override fun onServiceDisconnected(name: ComponentName) {}
        }, Context.BIND_AUTO_CREATE)
    }
}