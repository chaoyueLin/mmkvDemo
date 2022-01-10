

#ifndef CMMKV_CMMKV_H
#define CMMKV_CMMKV_H

#include "InterProcessLock.h"
#include "cpplang.hpp"
#include "MmapedFile.h"
#include "ThreadLock.h"
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "MiniPBCoder.h"

class CodedOutputData;

class MMBuffer;

enum MMKVMode : uint32_t {
    MMKV_SINGLE_PROCESS = 0x1,
    MMKV_MULTI_PROCESS = 0x2,
    MMKV_ASHMEM = 0x4,
};

class CMMKV final {
    std::unordered_map<std::string, MMBuffer> m_dic;
    std::string m_mmapID;
    std::string m_path;

    int m_fd;
    char *m_ptr;
    size_t m_size;
    size_t m_actualSize;
    CodedOutputData *m_output;
    MmapedFile *m_ashmemFile;

    bool m_needLoadFromFile;

    MmapedFile m_metaFile;

    ThreadLock m_lock;
    FileLock m_fileLock;
    InterProcessLock m_sharedProcessLock;
    InterProcessLock m_exclusiveProcessLock;

    void loadFromFile();

    void partialLoadFromFile();

    void loadFromAshmem();

    void checkLoadData();

    bool isFileValid();

    void writeAcutalSize(size_t actualSize);

    bool ensureMemorySize(size_t newSize);

    bool fullWriteback();

    const MMBuffer &getDataForKey(const std::string &key);

    bool setDataForKey(MMBuffer &&data, const std::string &key);

    bool removeDataForKey(const std::string &key);

    bool appendDataWithKey(const MMBuffer &data, const std::string &key);

    void checkReSetCryptKey(int fd, int metaFD, std::string *cryptKey);

    // just forbid it for possibly misuse
    CMMKV(const CMMKV &other) = delete;

    CMMKV &operator=(const CMMKV &other) = delete;

public:
    CMMKV(const std::string &mmapID,
          int size = DEFAULT_MMAP_SIZE,
          MMKVMode mode = MMKV_SINGLE_PROCESS);

    CMMKV(const std::string &mmapID,
          int ashmemFD,
          int ashmemMetaFd);

    ~CMMKV();

    static void initializeMMKV(const std::string &rootDir);

    // a generic purpose instance
    static CMMKV *defaultMMKV(MMKVMode mode = MMKV_SINGLE_PROCESS);

    // mmapID: any unique ID (com.tencent.xin.pay, etc)
    // if you want a per-user mmkv, you could merge user-id within mmapID
    static CMMKV *mmkvWithID(const std::string &mmapID,
                             int size = DEFAULT_MMAP_SIZE,
                             MMKVMode mode = MMKV_SINGLE_PROCESS);

    static CMMKV *mmkvWithAshmemFD(const std::string &mmapID,
                                   int fd,
                                   int metaFD);

    static void onExit();

    const std::string &mmapID();

    const bool m_isInterProcess;

    const bool m_isAshmem;

    int ashmemFD() { return m_isAshmem ? m_fd : -1; }

    int ashmemMetaFD() { return m_isAshmem ? m_metaFile.getFd() : -1; }

    bool setStringForKey(const std::string &value, const std::string &key);

    bool setBytesForKey(const MMBuffer &value, const std::string &key);

    bool setBool(bool value, const std::string &key);

    bool setInt32(int32_t value, const std::string &key);

    bool setInt64(int64_t value, const std::string &key);

    bool setFloat(float value, const std::string &key);

    bool setDouble(double value, const std::string &key);

    bool setVectorForKey(const std::vector<std::string> &vector, const std::string &key);

    bool getStringForKey(const std::string &key, std::string &result);

    MMBuffer getBytesForKey(const std::string &key);

    bool getBoolForKey(const std::string &key, bool defaultValue = false);

    int32_t getInt32ForKey(const std::string &key, int32_t defaultValue = 0);

    int64_t getInt64ForKey(const std::string &key, int64_t defaultValue = 0);

    float getFloatForKey(const std::string &key, float defaultValue = 0);

    double getDoubleForKey(const std::string &key, double defaultValue = 0);

    bool getVectorForKey(const std::string &key, std::vector<std::string> &result);

    bool containsKey(const std::string &key);

    size_t count();

    size_t totalSize();

    std::vector<std::string> allKeys();

    void removeValueForKey(const std::string &key);

    void removeValuesForKeys(const std::vector<std::string> &arrKeys);

    void clearAll();

    // call on memory warning
    void clearMemoryState();

    // you don't need to call this, really, I mean it
    // unless you care about out of battery
    void sync();

    static bool isFileValid(const std::string &mmapID);

    void lock() { m_exclusiveProcessLock.lock(); }

    void unlock() { m_exclusiveProcessLock.unlock(); }

    bool try_lock() { return m_exclusiveProcessLock.try_lock(); }
};


#endif //CMMKV_CMMKV_H
