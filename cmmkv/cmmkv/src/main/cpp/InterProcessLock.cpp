

#include "InterProcessLock.h"
#include "MMKVLog.h"
#include <unistd.h>

static short LockType2FlockType(LockType lockType) {
    switch (lockType) {
        case SharedLockType:
            //文件读锁
            return F_RDLCK;
        case ExclusiveLockType:
            //文件写锁
            return F_WRLCK;
    }
}

FileLock::FileLock(int fd) noexcept: m_fd(fd), m_shareLockCount(0), m_exclusiveLockCount(0) {
    m_lockInfo.l_type = F_WRLCK;
    m_lockInfo.l_start = 0;
    m_lockInfo.l_whence = SEEK_SET;
    m_lockInfo.l_len = 0;
    m_lockInfo.l_pid = 0;

}

bool FileLock::doLock(LockType lockType, int cmd) noexcept {
    if (!isFileLockValid()) {
        return false;
    }

    bool unLockFirstIfNeeded = false;

    if (lockType == SharedLockType) {
        m_shareLockCount++;
        // don't want shared-lock to break any existing locks
        if (m_shareLockCount > 1 || m_exclusiveLockCount > 0) {
            return true;
        }
    } else {
        m_exclusiveLockCount++;
        // don't want exclusive-lock to break existing exclusive-locks
        if (m_exclusiveLockCount > 1) {
            return true;
        }
        // 排斥锁当有共享锁时需要解锁
        if (m_shareLockCount > 0) {
            unLockFirstIfNeeded = true;
        }
    }
    //获取到文件写锁
    m_lockInfo.l_type = LockType2FlockType(lockType);
    //这里加排他锁前先把共享锁给释放掉
    if (unLockFirstIfNeeded) {
        //try lock 加锁如果成功了，证明没有被加锁过
        auto ret = fcntl(m_fd, F_SETLK, &m_lockInfo);
        if (ret == 0) {
            return true;
        }
        // lets be gentleman: unlock my shared-lock to prevent deadlock
        auto type = m_lockInfo.l_type;
        //先解锁
        m_lockInfo.l_type = F_UNLCK;
        ret = fcntl(m_fd, F_SETLK, &m_lockInfo);
        if (ret != 0) {
            MMKVError("fail to try unlock first fd=%d, ret=%d, error:%s", m_fd, ret,
                      strerror(errno));
        }
        m_lockInfo.l_type = type;
    }
    auto ret = fcntl(m_fd, cmd, &m_lockInfo);
    if (ret != 0) {
        MMKVError("fail to lock fd=%d, ret=%d, error:%s", m_fd, ret, strerror(errno));
        return false;
    } else {
        return true;
    }
}

bool FileLock::lock(LockType lockType) noexcept {
    return doLock(lockType, F_SETLKW);
}

bool FileLock::try_lock(LockType lockType) noexcept {
    return doLock(lockType, F_SETLK);
}

bool FileLock::unlock(LockType lockType) noexcept {
    if (!isFileLockValid()) {
        return false;
    }

    bool unLockToShareLock = false;

    if (lockType == SharedLockType) {
        if (m_shareLockCount == 0) {
            return false;
        }
        m_shareLockCount--;
        // don't want shared-lock to break any existing locks
        if (m_shareLockCount > 0 || m_exclusiveLockCount > 0) {
            return true;
        }
    } else {
        if (m_exclusiveLockCount == 0) {
            return false;
        }
        m_exclusiveLockCount--;
        if (m_exclusiveLockCount > 0) {
            return true;
        }
        // restore shared-lock when all exclusive-locks are done
        if (m_shareLockCount > 0) {
            unLockToShareLock = true;
        }

    }

    m_lockInfo.l_type = static_cast<short>(unLockToShareLock ? F_RDLCK : F_UNLCK);
    auto ret = fcntl(m_fd, F_SETLK, &m_lockInfo);
    if (ret != 0) {
        MMKVError("fail to unlock fd=%d, ret=%d, error:%s", m_fd, ret, strerror(errno));
        return false;
    } else {
        return true;
    }
}