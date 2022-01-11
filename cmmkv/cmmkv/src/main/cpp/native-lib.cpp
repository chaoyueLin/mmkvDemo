#include <jni.h>
#include <string>
#include <cstdint>
#include "MMKVLog.h"
#include "CMMKV.h"
#include "MMBuffer.h"


using namespace std;


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    MMKVDebug("d=%s", hello.c_str());
    return env->NewStringUTF(hello.c_str());
}

static jstring string2jstring(JNIEnv *env, const string &str) {
    return env->NewStringUTF(str.c_str());
}

static string jstring2string(JNIEnv *env, jstring str) {
    if (str) {
        const char *kstr = env->GetStringUTFChars(str, nullptr);
        if (kstr) {
            string result(kstr);
            env->ReleaseStringUTFChars(str, kstr);
            return result;
        }
    }
    return "";
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_initialize(JNIEnv *env, jobject thiz, jstring rootDir) {
    if (!rootDir) {
        return;
    }
    const char *kstr = env->GetStringUTFChars(rootDir, nullptr);
    if (kstr) {
        CMMKV::initializeMMKV(kstr);
        env->ReleaseStringUTFChars(rootDir, kstr);
    }
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_getMMKVWithID(JNIEnv *env, jobject thiz, jstring mmapID) {
    CMMKV *kv = nullptr;
    string str = jstring2string(env, mmapID);
    kv = new CMMKV(str, DEFAULT_MMAP_SIZE, MMKV_SINGLE_PROCESS);
    return (jlong) kv;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_encodeBool(JNIEnv *env, jobject thiz, jlong handle,
                                               jstring oKey,
                                               jboolean value) {
    CMMKV *kv = reinterpret_cast<CMMKV *>(handle);
    if (kv && oKey) {
        string key = jstring2string(env, oKey);
        return (jboolean) kv->setBool(value, key);
    }
    return (jboolean) false;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_decodeBool(JNIEnv *env, jobject thiz, jlong handle,
                                               jstring oKey,
                                               jboolean defaultValue) {
    CMMKV *kv = reinterpret_cast<CMMKV *>(handle);
    if (kv && oKey) {
        string key = jstring2string(env, oKey);
        return (jboolean) kv->getBoolForKey(key, defaultValue);
    }
    return defaultValue;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_encodeString(JNIEnv *env, jobject thiz, jlong handle,
                                                 jstring oKey, jstring oValue) {

    CMMKV *kv = reinterpret_cast<CMMKV *>(handle);
    if (kv && oKey && oValue) {
        string key = jstring2string(env, oKey);
        string value = jstring2string(env, oValue);
        return (jboolean) kv->setStringForKey(value, key);
    }
    return (jboolean) false;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_decodeString(JNIEnv *env, jobject thiz, jlong handle,
                                                 jstring oKey, jstring oDefaultValue) {

    CMMKV *kv = reinterpret_cast<CMMKV *>(handle);
    if (kv && oKey) {
        string key = jstring2string(env, oKey);
        string value;
        bool hasValue = kv->getStringForKey(key, value);
        if (hasValue) {
            return string2jstring(env, value);
        }
    }
    return oDefaultValue;
}




