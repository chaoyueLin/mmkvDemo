#include <jni.h>
#include <string>

#include "MMKVLog.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_chaoyue_cmmkv_MMKV_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    MMKVDebug("d=%s", hello.c_str());
    return env->NewStringUTF(hello.c_str());
}