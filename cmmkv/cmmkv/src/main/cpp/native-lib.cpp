#include <jni.h>
#include <string>
#include <cstdint>
#include "MMKVLog.h"
#include "/test/testLock.h"
#include "/test/testMMapFile.h"


using namespace std;


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    MMKVDebug("d=%s", hello.c_str());
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_testLog(JNIEnv *env, jobject thiz) {
    Test::testLog();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_testLock(JNIEnv *env, jobject thiz) {
    Test test;
    test.testLock();
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
        testMMapFile::initializeMMKV(kstr);
        env->ReleaseStringUTFChars(rootDir, kstr);
    }
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_getMMKVWithID(JNIEnv *env, jobject thiz, jstring mmapID) {
    MmapedFile *kv = nullptr;
    string str = jstring2string(env, mmapID);
    kv = testMMapFile::mmkvWithID(str, DEFAULT_MMAP_SIZE);
    return (jlong) kv;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_getMMKVValue(JNIEnv *env, jobject thiz) {
    return testMMapFile::getValue();
}