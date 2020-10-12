//
// Created by Charles on 19/7/21.
//

#ifndef MMKVDEMO_PBENCODEITEM_H
#define MMKVDEMO_PBENCODEITEM_H

#include "MMBuffer.h"
#include <stdint.h>
#include <memory.h>
#include <string>

enum PBEncodeItemType{
    PBEncodeItemType_None,
    PBEncodeItemType_String,
    PBEncodeItemType_Data,
    PBEncodeItemType_Container,
};


struct PBEncodeItem {
    PBEncodeItemType type;
    uint32_t compiledSize;
    uint32_t valueSize;
    union {
        const std::string * strValue;
        const MMBuffer *bufferValue;
    } value;

    PBEncodeItem():type(PBEncodeItemType_None),compiledSize(0),valueSize(0){
        memset(&value,0, sizeof(value));
    }
};


#endif //MMKVDEMO_PBENCODEITEM_H
