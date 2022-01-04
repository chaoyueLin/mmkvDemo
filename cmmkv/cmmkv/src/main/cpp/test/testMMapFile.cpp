

#include "testMMapFile.h"

static std::string g_rootDir;

void testMMapFile::initializeMMKV(const std::string &rootDir) {
    g_rootDir = rootDir;
    char *path = strdup(g_rootDir.c_str());
    mkPath(path);
    free(path);

    MMKVDebug("cmmkv root dir: %s", g_rootDir.c_str());
}

int *map;

MmapedFile *testMMapFile::mmkvWithID(const std::string &mmapID, int size) {
    auto kv = new MmapedFile(g_rootDir+"/"+mmapID, size, MMAP_FILE);
    if (kv->getMemory()) {
        MMKVDebug("cmmkv name %s", kv->getName().c_str());
        map = (int *) kv->getMemory();

        map[0] = 11;
    }
    return kv;
}

int testMMapFile::getValue() {
    if (map) {
        return map[0];
    }
    return -1;
}
