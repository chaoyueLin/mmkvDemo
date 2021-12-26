//
// Created by Charles on 2021/12/26.
//

#ifndef CMMKV_CODEDINPUTDATA_H
#define CMMKV_CODEDINPUTDATA_H

#include "MMBuffer.h"
#include <cstdint>
#include <string>
#include "cpplang.hpp"

class CodedInputData final {
    uint8_t *m_ptr;
    int32_t m_size;
    int32_t m_position;

    int8_t readRawByte();

    int32_t readRawVarint32();

    int32_t readRawLittleEndian32();

    int64_t readRawLittleEndian64();

public:
    CodedInputData(const void *oData, int32_t length);

    ~CodedInputData();

    bool isAtEnd() { return m_position == m_size; };

    bool readBool();

    double readDouble();

    float readFloat();

    int64_t readInt64();

    int32_t readInt32();

    int32_t readFixed32();

    std::string readString();

    MMBuffer readData();
};


#endif //CMMKV_CODEDINPUTDATA_H
