

#include "testLock.h"
#include <thread>
#include <list>

using namespace std;
#define TEST_MAX 1000000

ThreadLock lock_test;

list<int> test_list;

void Test::testLog() noexcept {
    MMKVDebug("mmkv", "log");
}

void in_list() {
    for (int i = 0; i < TEST_MAX; ++i) {
        SCOPEDLOCK(lock_test);
        MMKVDebug("插入数据%d", i);
        test_list.push_back(i);
    }
}

void out_list(){
    for (int i = 0; i < TEST_MAX; ++i) {
        SCOPEDLOCK(lock_test);
        if(!test_list.empty()){
            int temp=test_list.front();
            test_list.pop_front();
            MMKVDebug("取出数据%d", temp);
        }
    }
}


void Test::testLock() noexcept {
    thread in_thread(in_list);
    thread out_thread(out_list);
    in_thread.join();
    out_thread.join();
    MMKVDebug("done");
}




