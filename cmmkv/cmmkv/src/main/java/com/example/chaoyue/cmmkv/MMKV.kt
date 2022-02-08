package com.example.chaoyue.cmmkv

import android.content.Context

class MMKV private constructor(val id: Long) {

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

        val instance = MMKV(0)

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

    fun decodeBool(key: String): Boolean {
        return decodeBool(nativeHandle, key, false)
    }

    fun decodeBool(key: String, defaultValue: Boolean): Boolean {
        return decodeBool(nativeHandle, key, defaultValue)
    }

    fun encode(key: String, value: String): Boolean {
        return encodeString(nativeHandle, key, value)
    }

    fun decodeString(key: String): String? {
        return decodeString(nativeHandle, key, null)
    }

    fun decodeString(key: String, defaultValue: String): String? {
        return decodeString(nativeHandle, key, defaultValue)
    }

    fun encode(key: String, value: Int): Boolean {
        return encodeInt(nativeHandle, key, value)
    }

    fun decodeInt(key: String): Int {
        return decodeInt(nativeHandle, key, 0)
    }

    fun decodeInt(key: String, defaultValue: Int): Int {
        return decodeInt(nativeHandle, key, defaultValue)
    }

    fun encode(key: String, value: Long): Boolean {
        return encodeLong(nativeHandle, key, value)
    }

    fun decodeLong(key: String): Long {
        return decodeLong(nativeHandle, key, 0)
    }

    fun decodeLong(key: String, defaultValue: Long): Long {
        return decodeLong(nativeHandle, key, defaultValue)
    }

    fun encode(key: String, value: Float): Boolean {
        return encodeFloat(nativeHandle, key, value)
    }

    fun decodeFloat(key: String): Float {
        return decodeFloat(nativeHandle, key, 0f)
    }

    fun decodeFloat(key: String, defaultValue: Float): Float {
        return decodeFloat(nativeHandle, key, defaultValue)
    }

    fun encode(key: String, value: Double): Boolean {
        return encodeDouble(nativeHandle, key, value)
    }

    fun decodeDouble(key: String): Double {
        return decodeDouble(nativeHandle, key, 0.0)
    }

    fun decodeDouble(key: String, defaultValue: Double): Double {
        return decodeDouble(nativeHandle, key, defaultValue)
    }

    private external fun initialize(rootDir: String)

    private external fun getMMKVWithID(mmapID: String): Long


    private external fun encodeBool(handle: Long, key: String, value: Boolean): Boolean

    private external fun decodeBool(handle: Long, key: String, defaultValue: Boolean): Boolean

    private external fun encodeString(handle: Long, key: String, value: String): Boolean

    private external fun decodeString(handle: Long, key: String, defaultValue: String?): String?

    private external fun encodeInt(handle: Long, key: String, value: Int): Boolean

    private external fun decodeInt(handle: Long, key: String, defaultValue: Int): Int

    private external fun encodeLong(handle: Long, key: String, value: Long): Boolean

    private external fun decodeLong(handle: Long, key: String, defaultValue: Long): Long

    private external fun encodeFloat(handle: Long, key: String, value: Float): Boolean

    private external fun decodeFloat(handle: Long, key: String, defaultValue: Float): Float

    private external fun encodeDouble(handle: Long, key: String, value: Double): Boolean

    private external fun decodeDouble(handle: Long, key: String, defaultValue: Double): Double

}