//
// Created by Charles on 19/7/27.
//

#include "ThreadLock.h"
#include "MMKVLog.h"
/**
 * PTHREAD_MUTEX_RECURSIVE
 * 则该互斥锁会保留锁定计数这一概念。线程首次成功获取互斥锁时，锁定计数会设置为 1。线程每重新锁定该互斥锁一次，锁定计数就增加 1。
 * 线程每解除锁定该互斥锁一次，锁定计数就减小 1。 锁定计数达到 0 时，该互斥锁即可供其他线程获取。
 * 如果某个线程尝试解除锁定的互斥锁不是由该线程锁定或者未锁定，则将返回错误。
 */
ThreadLock::ThreadLock() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_lock,&attr);
    pthread_mutexattr_destroy(&attr);
}

ThreadLock::~ThreadLock() {
    pthread_mutex_destroy(&m_lock);
}

void ThreadLock::lock() {
    auto ret=pthread_mutex_lock(&m_lock);
    if(ret!=0){
        MMKVError("fail to lock %p, ret=%d, errno=%s", &m_lock, ret, strerror(errno));
    }
}

bool ThreadLock::try_lock() {
    auto ret = pthread_mutex_trylock(&m_lock);
    if (ret != 0) {
        MMKVError("fail to try lock %p, ret=%d, errno=%s", &m_lock, ret, strerror(errno));
    }
    return (ret == 0);
}

void ThreadLock::unlock() {
    auto ret = pthread_mutex_unlock(&m_lock);
    if (ret != 0) {
        MMKVError("fail to unlock %p, ret=%d, errno=%s", &m_lock, ret, strerror(errno));
    }
}