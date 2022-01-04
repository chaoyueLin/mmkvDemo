package com.example.chaoyue.cmmkv

import android.content.Context

class MMKV {

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


    // call on program start
    fun initialize(context: Context): String? {
        val rootDir = context.filesDir.absolutePath + "/mmkv"
        initialize(rootDir)
        return rootDir
    }

    fun mmkvWithID(mmapID: String): Long? {
        val handle: Long = getMMKVWithID(mmapID)
        return handle
    }

    fun mmkvGetValue():Int{
        return getMMKVValue()
    }



    external fun testLog()

    external fun testLock()

    private external fun initialize(rootDir: String)

    private external fun getMMKVWithID(mmapID: String): Long

    private external fun getMMKVValue():Int
}