

#ifndef CMMKV_TESTLOCK_H
#define CMMKV_TESTLOCK_H

#include "../MMKVLog.h"
#include "../cpplang.hpp"
#include "../ScopedLock.hpp"
#include "../ThreadLock.h"
#include "../InterProcessLock.h"

class Test final {

public:
    Test() = default;

    ~Test() = default;

    Test(const Test &) = delete;

    Test &operator=(const Test &other) = delete;

    static void testLog() noexcept;

    void testLock() noexcept;
};

#endif //CMMKV_TESTLOCK_H
