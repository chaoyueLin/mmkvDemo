//
// Created by Charles on 2021/12/26.
//

#ifndef CMMKV_CODEDOUTPUTDATA_H
#define CMMKV_CODEDOUTPUTDATA_H

#include <cstdint>
#include <string>
#include "cpplang.hpp"
#include "MMBuffer.h"

/**
 * 存储数据，小端存储,自定义存储的4个字节的int,8个字节的int
 */
class CodedOutputData final {
    uint8_t *m_ptr;//内存指针
    size_t m_size;//内存区域大小
    int32_t m_position;//内存读取位置

public:
    CodedOutputData(void *ptr, size_t len);

    ~CodedOutputData();

    int32_t spaceLeft();

    void seek(size_t addedSize);

    void writeRawByte(uint8_t value);

    void writeRawLittleEndian32(int32_t value);

    void writeRawLittleEndian64(int64_t value);

    void writeRawVarint32(int32_t value);

    void writeRawVarint64(int64_t value);

    void writeRawData(const MMBuffer &data);

    void writeDouble(double value);

    void writeFloat(float value);

    void writeInt64(int64_t value);

    void writeInt32(int32_t value);

    void writeBool(bool value);

    void writeString(const std::string &value);

    void writeData(const MMBuffer &value);
};


#endif //CMMKV_CODEDOUTPUTDATA_H
