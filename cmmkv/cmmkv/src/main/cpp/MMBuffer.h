//
// Created by Charles on 2021/12/26.
//

#ifndef CMMKV_MMBUFFER_H
#define CMMKV_MMBUFFER_H

#include <cstdint>
#include <cstddef>
#include "cpplang.hpp"


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

    MMBuffer(const MMBuffer &other) = delete;

    MMBuffer &operator=(const MMBuffer &other) = delete;
};


#endif //CMMKV_MMBUFFER_H
