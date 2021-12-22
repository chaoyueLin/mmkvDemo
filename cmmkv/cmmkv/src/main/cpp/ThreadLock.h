

#ifndef CMMKV_THREADLOCK_H
#define CMMKV_THREADLOCK_H

#include "cpplang.hpp"
#include <pthread.h>

class ThreadLock final {
private:
    pthread_mutex_t m_lock;

public:
    ThreadLock();

    ~ThreadLock();

    ThreadLock(const ThreadLock &) = delete;

    ThreadLock &operator=(const pthread_mutex_t &) = delete;

    void lock() noexcept;

    bool try_lock() noexcept;

    void unlock() noexcept;
};


#endif //CMMKV_THREADLOCK_H
