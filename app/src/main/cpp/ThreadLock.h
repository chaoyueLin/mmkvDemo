//
// Created by Charles on 19/7/27.
//

#ifndef MMKVDEMO_THREADLOCK_H
#define MMKVDEMO_THREADLOCK_H

#include <pthread.h>
class ThreadLock {
private:
    pthread_mutex_t m_lock;

public:
    ThreadLock();
    ~ThreadLock();
    void lock();
    bool try_lock();
    void unlock();
};


#endif //MMKVDEMO_THREADLOCK_H
