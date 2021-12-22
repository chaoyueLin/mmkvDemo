

#include "ThreadLock.h"
#include "MMKVLog.h"


ThreadLock::ThreadLock() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&m_lock, &attr);

    pthread_mutexattr_destroy(&attr);
}

ThreadLock::~ThreadLock() {
    pthread_mutex_destroy(&m_lock);
}

void ThreadLock::lock() noexcept {
    auto ret = pthread_mutex_lock(&m_lock);
    if (ret != 0) {
        MMKVError("fail to lock %p, ret=%d, errno=%s", &m_lock, ret, strerror(errno));
    }
}

bool ThreadLock::try_lock() noexcept {
    auto ret = pthread_mutex_trylock(&m_lock);
    if (ret != 0) {
        MMKVError("fail to try lock %p, ret=%d, errno=%s", &m_lock, ret, strerror(errno));
    }
    return (ret == 0);
}

void ThreadLock::unlock() noexcept {
    auto ret = pthread_mutex_unlock(&m_lock);
    if (ret != 0) {
        MMKVError("fail to unlock %p, ret=%d, errno=%s", &m_lock, ret, strerror(errno));
    }
}
