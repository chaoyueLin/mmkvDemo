//
// Created by Charles on 19/7/21.
//

#ifndef MMKVDEMO_SCOPEDLOCK_H
#define MMKVDEMO_SCOPEDLOCK_H

#include <cassert>
#include "MMKVLog.h"

template <typename T>
class ScopedLock {
    T * m_lock;

    ScopedLock(const ScopedLock<T> &other)= delete;
    ScopedLock &operator =(const ScopedLock<T> &other)= delete;

public:
    ScopedLock(T *oLock):m_lock(oLock){
        assert(m_lock);
        lock();
    }

    ~ScopedLock(){
        unlock();
        m_lock= nullptr;
    }

    void lock(){
        if(m_lock){
            m_lock->lock();
        }
    }

    bool  try_lock(){
        if(m_lock){
            return m_lock->try_lock();
        }
        return false;
    }

    void unlock(){
        if(m_lock){
            m_lock->unlock();
        }
    }
};


//__scopedLock##counter 宏定义,__COUNTER__ 初值是0，每预编译一次其值自己加1
#define SCOPEDLOCK(lock) _SCOPEDLOCK(lock, __COUNTER__)
#define _SCOPEDLOCK(lock, counter) __SCOPEDLOCK(lock, counter)
#define __SCOPEDLOCK(lock, counter) ScopedLock<decltype(lock)> __scopedLock##counter(&lock)


#endif //MMKVDEMO_SCOPEDLOCK_H
