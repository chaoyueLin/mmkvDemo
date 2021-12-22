

#ifndef CMMKV_SCOPEDLOCK_HPP
#define CMMKV_SCOPEDLOCK_HPP

#include "MMKVLog.h"
#include "cpplang.hpp"

template<typename T>
class ScopedLock final {
    T *m_lock;

    // just forbid it for possibly misuse
    ScopedLock(const ScopedLock<T> &other) = delete;

    ScopedLock &operator=(const ScopedLock<T> &other) = delete;

public:
    ScopedLock(T *oLock) noexcept: m_lock(oLock) {
        assert(m_lock);
        lock();
    }

    ~ScopedLock() noexcept {
        unlock();
        m_lock = nullptr;
    }

    void lock() noexcept {
        if (m_lock) {
            m_lock->lock();
        }
    }

    bool try_lock() noexcept {
        if (m_lock) {
            return m_lock->try_lock();
        }
        return false;
    }

    void unlock() noexcept {
        if (m_lock) {
            m_lock->unlock();
        }
    }
};


//__scopedLock##counter 宏定义,__COUNTER__ 初值是0，每预编译一次其值自己加1
#define SCOPEDLOCK(lock) _SCOPEDLOCK(lock, __COUNTER__)
#define _SCOPEDLOCK(lock, counter) __SCOPEDLOCK(lock, counter)
#define __SCOPEDLOCK(lock, counter) ScopedLock<decltype(lock)> __scopedLock##counter(&lock)

#endif //CMMKV_SCOPEDLOCK_HPP
