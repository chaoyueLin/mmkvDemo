//
// Created by Charles on 19/7/7.
//

#include "MMKVLog.h"
#include "native-lib.h"
#include <string>

#ifdef  ENABLE_MMKV_LOG

using namespace std;

#define  APPNAME "MMKV"

bool g_isLogReddirecting=false;

#ifndef NDEBUG
MMKVLogLevel g_currentLogLevel = MMKVLogDebug;
#else
MMKVLogLevel g_currentLogLevel = MMKVLogInfo;
#endif

//android的log
static android_LogPriority MMKVLogLevelDesc(MMKVLogLevel level){
    switch (level){
        case MMKVLogDebug:
            return ANDROID_LOG_DEBUG;
        case MMKVLogInfo:
            return ANDROID_LOG_INFO;
        case MMKVLogWarning:
            return ANDROID_LOG_WARN;
        case MMKVLogError:
            return ANDROID_LOG_ERROR;
        default:
            return ANDROID_LOG_UNKNOWN;

    }
}

void _MMKVLogWithLevel(
        MMKVLogLevel level,const char *file,const char *func,int line,const char *format,...){
    if (level>=g_currentLogLevel){
        string message;
        char buffer[16];
        //可变参数
        va_list args;
        va_start(args,format);
        auto length = std::vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        if(length<0){
            message={};
        }else if(length<sizeof(buffer)){
            message=string(buffer,length);
        }else {
            message.resize(length,'\0');
            va_start(args,format);
            std::vsnprintf(message.data(), length + 1, format, args);
            va_end(args);
        }

        if (g_isLogRedirecting) {
            mmkvLog((int) level, file, line, func, message);
        } else {
            __android_log_print(MMKVLogLevelDesc(level), APPNAME, "<%s:%d::%s> %s", file, line,
                                func, message.c_str());
        }
    }

}

#endif //ENABLE_MMKV_LOG