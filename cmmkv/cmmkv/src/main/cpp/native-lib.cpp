#include <jni.h>
#include <string>

#include "MMKVLog.h"
#include "/test/testLock.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    MMKVDebug("d=%s", hello.c_str());
    return env->NewStringUTF(hello.c_str());
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_testLog(JNIEnv *env, jobject thiz) {
    Test::testLog();
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_testLock(JNIEnv *env, jobject thiz) {
    Test test;
    test.testLock();
}