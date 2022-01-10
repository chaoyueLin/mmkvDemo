

#ifndef CMMKV_INTERPROCESSLOCK_H
#define CMMKV_INTERPROCESSLOCK_H

#include "cpplang.hpp"
#include <fcntl.h>

/**
 * 自定义锁类型，共享锁与排他锁
 */

enum LockType {
    SharedLockType,
    ExclusiveLockType,
};


/**
 * 文件锁
 */

class FileLock final {
    int m_fd;
    //flock文件锁，用于多进程
    struct flock m_lockInfo;
    //共享锁的被获取的计数
    size_t m_shareLockCount;
    //排斥锁的被获取的计数
    size_t m_exclusiveLockCount;

    bool doLock(LockType lockType, int cmd) noexcept;

    bool isFileLockValid() noexcept {
        return m_fd > 0;
    }

    FileLock(const FileLock &other) = delete;

    FileLock &operator=(const FileLock &other) = delete;


public:
    FileLock(int fd) noexcept;

    bool lock(LockType lockType) noexcept;

    bool try_lock(LockType lockType) noexcept;

    bool unlock(LockType lockType) noexcept;
};

class InterProcessLock final {
    FileLock *m_fileLock;
    LockType m_lockType;

public:
    bool m_enable;

    InterProcessLock(FileLock *fileLock, LockType lockType) noexcept
            : m_fileLock(fileLock), m_lockType(lockType), m_enable(true) {
        assert(m_fileLock);
    }

    InterProcessLock(const InterProcessLock &other) = delete;

    InterProcessLock &operator=(const InterProcessLock &other) = delete;

    void lock() noexcept {
        if (m_enable) {
            m_fileLock->lock(m_lockType);
        }
    }

    bool try_lock() noexcept {
        if (m_enable) {
            return m_fileLock->try_lock(m_lockType);
        }
        return false;
    }

    void unlock() noexcept {
        if (m_enable) {
            m_fileLock->unlock(m_lockType);
        }
    }
};


#endif //CMMKV_INTERPROCESSLOCK_H
