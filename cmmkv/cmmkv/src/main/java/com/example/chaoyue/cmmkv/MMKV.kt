package com.example.chaoyue.cmmkv

import android.content.Context

class MMKV private constructor(val id:Long){

    private val nativeHandle: Long = id

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

        val instance=MMKV(0)
        // call on program start
        fun initialize(context: Context): String? {
            val rootDir = context.filesDir.absolutePath + "/mmkv"
            instance.initialize(rootDir)
            return rootDir
        }
        fun mmkvWithID(mmapID: String): MMKV {
            val handle: Long = instance.getMMKVWithID(mmapID)
            return MMKV(handle)
        }
    }




    private fun mmkvWithID(mmapID: String): Long? {
        val handle: Long = getMMKVWithID(mmapID)
        return handle
    }
    fun encode(key: String?, value: Boolean): Boolean {
        return encodeBool(nativeHandle, key!!, value)
    }

    fun decodeBool(key: String?): Boolean {
        return decodeBool(nativeHandle, key!!, false)
    }

    fun decodeBool(key: String?, defaultValue: Boolean): Boolean {
        return decodeBool(nativeHandle, key!!, defaultValue)
    }

    private external fun initialize(rootDir: String)

    private external fun getMMKVWithID(mmapID: String): Long


    private external fun encodeBool(handle: Long, key: String, value: Boolean): Boolean

    private external fun decodeBool(handle: Long, key: String, defaultValue: Boolean): Boolean


}