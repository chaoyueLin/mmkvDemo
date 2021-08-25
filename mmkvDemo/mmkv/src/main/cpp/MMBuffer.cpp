/**
// Created by Charles on 19/7/8.
// 分配缓存
**/

#include "MMBuffer.h"
#include <cstdlib>
#include <cstring>
#include <utility>

MMBuffer::MMBuffer(size_t length):ptr(nullptr),size(length),isNoCopy(MMBufferCopy){
    if(size>0){
        ptr=malloc(size);
    }
}

/**
 * 把原来的缓存数据拷贝到新的对象
 * @param source
 * @param length
 * @param noCopy
 */
MMBuffer::MMBuffer(void *source,size_t length,MMBufferCopyFlag noCopy)
        :ptr(source),size(length),isNoCopy(noCopy){
    if(isNoCopy==MMBufferCopy){
        ptr=malloc(size);

        memcpy(ptr,source,size);
    }
}

MMBuffer::MMBuffer(MMBuffer &&other) noexcept
:ptr(other.ptr),size(other.size),isNoCopy(other.isNoCopy){

    other.ptr= nullptr;
    other.size=0;
    other.isNoCopy=MMBufferCopy;
}

MMBuffer &MMBuffer::operator=(MMBuffer &&other) noexcept {
    std::swap(ptr,other.ptr);
    std::swap(size,other.size);
    std::swap(isNoCopy,other.isNoCopy);
    return *this;
}

MMBuffer::~MMBuffer() {
    if(isNoCopy==MMBufferCopy && ptr){
        free(ptr);
    }
    ptr= nullptr;
}