//
// Created by Charles on 19/7/7.
//

#ifndef MMKVDEMO_MMKVLOG_H
#define MMKVDEMO_MMKVLOG_H

#include <android/log.h>
#include <cstdint>
#include <cstring>
#include <errno.h>

enum MMKVLogLevel:uint32_t {
    MMKVLogDebug=0,
    MMKVLogInfo=1,
    MMKVLogWarning=2,
    MMKVLogError=3,
};

#define ENABLE_MMKV_LOG

#ifdef ENABLE_MMKV_LOG

extern bool g_isLogRedirecting;
extern MMKVLogLevel g_currentlogLevel;

#define __filename__ (strrchr(__FILE__, '/') + 1)

#define MMKVError(format, ...)                                                                     \
    _MMKVLogWithLevel(MMKVLogError, __filename__, __func__, __LINE__, format, ##__VA_ARGS__)

#define MMKVWarning(format, ...)                                                                   \
    _MMKVLogWithLevel(MMKVLogWarning, __filename__, __func__, __LINE__, format, ##__VA_ARGS__)

#define MMKVInfo(format, ...)                                                                      \
    _MMKVLogWithLevel(MMKVLogInfo, __filename__, __func__, __LINE__, format, ##__VA_ARGS__)

#ifndef NDEBUG
#define MMKVDebug(format, ...)                                                                     \
    _MMKVLogWithLevel(MMKVLogDebug, __filename__, __func__, __LINE__, format, ##__VA_ARGS__)
#else
#define MMKVDebug(format, ...)                                                                     \
    {}
#endif

void _MMKVLogWithLevel(
        MMKVLogLevel level,const  char *file,const char *func,int line,const char *format,...);

#else
#define MMKVError(format, ...)                                                                     \
    {}
#define MMKVWarning(format, ...)                                                                   \
    {}
#define MMKVInfo(format, ...)                                                                      \
    {}
#define MMKVDebug(format, ...)                                                                     \
    {}

#endif //MMKVDEMO_MMKVLOG_H
