

#ifndef CMMKV_MMKVLOG_H
#define CMMKV_MMKVLOG_H

#include <android/log.h>
#include <cstring>
#include <errno.h>
#include "cpplang.hpp"

#define ENABLE_MMKV_LOG

#ifdef ENABLE_MMKV_LOG

#define APPNAME "MMKV"

#define MMKVError(format, ...) __android_log_print(ANDROID_LOG_ERROR,APPNAME,format,##__VA_ARGS__)
#define MMKVWarning(format, ...)                                                                   \
    __android_log_print(ANDROID_LOG_WARN, APPNAME, format, ##__VA_ARGS__)
#define MMKVInfo(format, ...) __android_log_print(ANDROID_LOG_INFO, APPNAME, format, ##__VA_ARGS__)

#ifndef NDEBUG
#define MMKVDebug(format, ...)                                                                     \
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, format, ##__VA_ARGS__)
#else
#define MMKVDebug(format, ...)                                                                     \
    {}
#endif
#endif
#endif //CMMKV_MMKVLOG_H
