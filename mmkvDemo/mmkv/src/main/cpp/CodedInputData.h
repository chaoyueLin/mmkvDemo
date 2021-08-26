/**
// Created by Charles on 19/7/17.
//读取数据。自定义一个字节的int,4个字节的int,8个字节的int
**/

#ifndef MMKVDEMO_CODEINPUTDATA_H
#define MMKVDEMO_CODEINPUTDATA_H

#include "MMBuffer.h"
#include <cstdint>
#include <string>

class CodedInputData {
    uint8_t *m_ptr;
    int32_t m_size;
    int32_t m_position;

    int8_t readRawByte();
    int32_t readRawVarint32();

    int32_t readRawLittleEndian32();
    int64_t readRawLittleEndian64();

public:
    CodedInputData(const void *oData,int32_t length);

    ~CodedInputData();

    bool isAtEnd(){
        return m_position==m_size;
    };

    bool readBool();

    double readDouble();
    float  readFloat();
    int64_t  readInt64();
    int32_t readInt32();
    int32_t readFixed32();
    std::string readString();
    MMBuffer readData();
};


#endif //MMKVDEMO_CODEINPUTDATA_H
