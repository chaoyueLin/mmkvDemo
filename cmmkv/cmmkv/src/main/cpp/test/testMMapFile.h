

#ifndef CMMKV_TESTMMAPFILE_H
#define CMMKV_TESTMMAPFILE_H

#include "../cpplang.hpp"
#include "../MmapedFile.h"
#include "../MMKVLog.h"
class testMMapFile {

public:
    static void initializeMMKV(const std::string &rootDir);

    static MmapedFile *mmkvWithID(const std::string &mmapID,
                            int size = DEFAULT_MMAP_SIZE);
    static int getValue();
};


#endif //CMMKV_TESTMMAPFILE_H
