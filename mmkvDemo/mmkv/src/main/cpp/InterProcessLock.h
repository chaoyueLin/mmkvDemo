/**
// Created by Charles on 19/7/15.
使用flock文件锁，作为多进程
**/

#ifndef MMKVDEMO_INTERPROCESSLOCK_H
#define MMKVDEMO_INTERPROCESSLOCK_H


#include <cassert>
#include <fcntl.h>

/**
 * 自定义锁类型，共享锁和排斥锁
 */
enum LockType{
    SharedLockType,
    ExclusiveLockType,
};

/**
 * 文件锁
 */
class FileLock{
    int m_fd;
    //flock文件锁，用于多进程
    struct flock m_lockInfo;
    //共享锁的被获取的计数
    size_t  m_shareLockCount;
    //排斥锁的被获取的计数
    size_t  m_exclusiveLockCount;

    bool doLock(LockType lockType,int cmd);

    bool isFileLockValid(){
        return m_fd>0;
    }

    FileLock(const FileLock &other) = delete;
    FileLock &operator=(const FileLock &other) = delete;


public:
    FileLock(int fd);
    bool lock(LockType lockType);
    bool try_lock(LockType lockType);
    bool unlock(LockType lockType);
};

class InterProcessLock{
    FileLock * m_fileLock;
    LockType  m_lockType;

public:
    InterProcessLock(FileLock *fileLock,LockType lockType)
    :m_fileLock(fileLock),m_lockType(lockType),m_enable(true){
        assert(m_fileLock);
    }
    bool  m_enable;

    void lock(){
        if(m_enable){
            m_fileLock->lock(m_lockType);
        }
    }

    bool  try_lock(){
        if(m_enable){
            return m_fileLock->try_lock(m_lockType);
        }
        return false;
    }

    void unlock(){
        if(m_enable){
            m_fileLock->unlock(m_lockType);
        }
    }

};

#endif //MMKVDEMO_INTERPROCESSLOCK_H