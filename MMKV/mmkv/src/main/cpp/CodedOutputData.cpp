/*
 * Tencent is pleased to support the open source community by making
 * MMKV available.
 *
 * Copyright (C) 2018 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "CodedOutputData.h"
#include "MMKVLog.h"
#include "PBUtility.h"
#include <cassert>
using namespace std;

CodedOutputData::CodedOutputData(void *ptr, size_t len)
        : m_ptr((uint8_t *) ptr), m_size(len), m_position(0) {
    assert(m_ptr);
}

CodedOutputData::~CodedOutputData() {
    m_ptr = nullptr;
    m_position = 0;
}

void CodedOutputData::writeDouble(double value) {
    this->writeRawLittleEndian64(Float64ToInt64(value));
}

void CodedOutputData::writeFloat(float value) {
    this->writeRawLittleEndian32(Float32ToInt32(value));
}

void CodedOutputData::writeInt64(int64_t value) {
    this->writeRawVarint64(value);
}

void CodedOutputData::writeInt32(int32_t value) {
    if (value >= 0) {
        this->writeRawVarint32(value);
    } else {
        this->writeRawVarint64(value);
    }
}

void CodedOutputData::writeBool(bool value) {
    this->writeRawByte(static_cast<uint8_t>(value ? 1 : 0));
}


void CodedOutputData::writeString(const string &value) {
    //先写了大小
    size_t numberOfBytes = value.size();
    //writeRawVarint32这个方法position已经增加
    this->writeRawVarint32((int32_t) numberOfBytes);
    //再保存值
    memcpy(m_ptr + m_position, ((uint8_t *) value.data()), numberOfBytes);
    m_position += numberOfBytes;
}

void CodedOutputData::writeData(const MMBuffer &value) {
    //先写了大小
    this->writeRawVarint32((int32_t) value.length());
    //再保存值
    this->writeRawData(value);
}

int32_t CodedOutputData::spaceLeft() {
    return int32_t(m_size - m_position);
}

void CodedOutputData::seek(size_t addedSize) {
    m_position += addedSize;

    if (m_position > m_size) {
        MMKVError("OutOfSpace");
    }
}

void CodedOutputData::writeRawByte(uint8_t value) {
    if (m_position == m_size) {
        MMKVError("m_position: %d, m_size: %zd", m_position, m_size);
        return;
    }

    m_ptr[m_position++] = value;
}

void CodedOutputData::writeRawData(const MMBuffer &data) {
    size_t numberOfBytes = data.length();
    memcpy(m_ptr + m_position, data.getPtr(), numberOfBytes);
    m_position += numberOfBytes;
}

/**
 * 写输入，用0x80带入执行就很好理解
 * @param value
 */
void CodedOutputData::writeRawVarint32(int32_t value) {
    while (true) {
        //判断是否小于等于0x7f
        if ((value & ~0x7f) == 0) {
            //类型转换用的uint8_t，不是int8_t
            this->writeRawByte(static_cast<uint8_t>(value));
            return;
        } else {
            //获取低7位
            this->writeRawByte(static_cast<uint8_t>((value & 0x7F) | 0x80));
            //再逻辑右移动7位
            value = logicalRightShift32(value, 7);
        }
    }
}

void CodedOutputData::writeRawVarint64(int64_t value) {
    while (true) {
        if ((value & ~0x7f) == 0) {
            this->writeRawByte(static_cast<uint8_t>(value));
            return;
        } else {
            this->writeRawByte(static_cast<uint8_t>((value & 0x7f) | 0x80));
            value = logicalRightShift64(value, 7);
        }
    }
}

/**
 * 转成小端写入，低字节写在低地址
 * @param value
 */
void CodedOutputData::writeRawLittleEndian32(int32_t value) {
    this->writeRawByte(static_cast<uint8_t>((value) &0xff));
    this->writeRawByte(static_cast<uint8_t>((value >> 8) & 0xff));
    this->writeRawByte(static_cast<uint8_t>((value >> 16) & 0xff));
    this->writeRawByte(static_cast<uint8_t>((value >> 24) & 0xff));
}

void CodedOutputData::writeRawLittleEndian64(int64_t value) {
    this->writeRawByte(static_cast<uint8_t>((value) &0xff));
    this->writeRawByte(static_cast<uint8_t>((value >> 8) & 0xff));
    this->writeRawByte(static_cast<uint8_t>((value >> 16) & 0xff));
    this->writeRawByte(static_cast<uint8_t>((value >> 24) & 0xff));
    this->writeRawByte(static_cast<uint8_t>((value >> 32) & 0xff));
    this->writeRawByte(static_cast<uint8_t>((value >> 40) & 0xff));
    this->writeRawByte(static_cast<uint8_t>((value >> 48) & 0xff));
    this->writeRawByte(static_cast<uint8_t>((value >> 56) & 0xff));
}

