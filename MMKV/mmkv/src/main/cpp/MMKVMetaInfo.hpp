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

#ifndef MMKV_MMKVMETAINFO_H
#define MMKV_MMKVMETAINFO_H

#include <cassert>
#include <cstdint>
#include <cstring>

struct MMKVMetaInfo {
    uint32_t m_crcDigest = 0;//crc校验的数据
    uint32_t m_version = 1;
    uint32_t m_sequence = 0; // full write-back count
    //版本信息写在目标地址
    void write(void *ptr) {
        assert(ptr);
        memcpy(ptr, this, sizeof(MMKVMetaInfo));
    }

    void read(const void *ptr) {
        assert(ptr);
        memcpy(this, ptr, sizeof(MMKVMetaInfo));
    }
};

#endif //MMKV_MMKVMETAINFO_H
