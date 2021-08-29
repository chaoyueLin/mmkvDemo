/**
// Created by Charles on 19/7/27.
// MMKV的版本信息
**/

#ifndef MMKVDEMO_MMKVMETAINFO_H
#define MMKVDEMO_MMKVMETAINFO_H

#include <cassert>
#include <cstdint>
#include <cstring>

struct MMKVMetaInfo{
    uint32_t m_crcDigest=0;
    uint32_t m_version=1;
    uint32_t m_sequence=0;

    //版本信息写在目标地址
    void write(void *ptr){
        assert(ptr);
        memcpy(ptr,this, sizeof(MMKVMetaInfo));
    }
    void read(const void *ptr) {
        assert(ptr);
        memcpy(this, ptr, sizeof(MMKVMetaInfo));
    }
};

#endif //MMKVDEMO_MMKVMETAINFO_H
