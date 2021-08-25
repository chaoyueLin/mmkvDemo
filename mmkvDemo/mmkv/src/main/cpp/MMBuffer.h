/**
// Created by Charles on 19/7/8.
//MMBuffer保持缓存的指针和大小
//1.只能实例化，拷贝构造和拷贝复制删除了
**/

#ifndef MMKVDEMO_MMBUFFER_H
#define MMKVDEMO_MMBUFFER_H

#include <cstdint>
#include <cstddef>

enum MMBufferCopyFlag : bool {
    MMBufferCopy = false,
    MMBufferNoCopy = true,
};

class MMBuffer {
private:
    void *ptr;
    size_t size;
    MMBufferCopyFlag isNoCopy;

public:
    void *getPtr() const { return ptr; }

    size_t length() const { return size; }

    MMBuffer(size_t length = 0);

    MMBuffer(void *source, size_t length, MMBufferCopyFlag noCopy = MMBufferCopy);

    MMBuffer(MMBuffer &&other) noexcept;

    MMBuffer &operator=(MMBuffer &&other) noexcept;

    ~MMBuffer();

private:
    // those are expensive, just forbid it for possibly misuse
    MMBuffer(const MMBuffer &other) = delete;

    MMBuffer &operator=(const MMBuffer &other) = delete;
};


#endif //MMKVDEMO_MMBUFFER_H
