

#ifndef CMMKV_PBENCODEITEM_HPP
#define CMMKV_PBENCODEITEM_HPP


#include "MMBuffer.h"
#include <cstdint>
#include <memory.h>
#include <string>

enum PBEncodeItemType {
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
        const std::string *strValue;
        const MMBuffer *bufferValue;
    } value;

    PBEncodeItem() : type(PBEncodeItemType_None), compiledSize(0), valueSize(0) {
        memset(&value, 0, sizeof(value));
    }
};


#endif //CMMKV_PBENCODEITEM_HPP
