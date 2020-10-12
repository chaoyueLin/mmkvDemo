//
// Created by Charles on 19/7/27.
//

#ifndef MMKVDEMO_NATIVE_LIB_H
#define MMKVDEMO_NATIVE_LIB_H

#include <string>
enum MMKVRecoverStrategic:int {
    OnErrorDiscard=0;
    OnErrorRecover,
};

MMKVRecoverStrategic onMMKVCRCCheckFail(const std::string &mmapID);

MMKVRecoverStrategic onMMKVFileLengthError(const std::string &mmapID);

void mmkvLog(int level,
    const std::string &file,
    int line,
    const std::string &function,
    const std::string &message);

#endif //MMKVDEMO_NATIVE_LIB_H
