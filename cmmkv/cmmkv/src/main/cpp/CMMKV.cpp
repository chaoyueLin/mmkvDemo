

#include "CMMKV.h"
#include "CodedInputData.h"
#include "CodedOutputData.h"
#include "InterProcessLock.h"
#include "MMBuffer.h"
#include "MMKVLog.h"
#include "MmapedFile.h"
#include "PBUtility.h"
#include "ScopedLock.hpp"
#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <zlib.h>

using namespace std;


static ThreadLock g_instanceLock;
static std::string g_rootDir;

#define DEFAULT_MMAP_ID "mmkv.default"
//小端4个字节
constexpr uint32_t Fixed32Size = pbFixed32Size(0);

static string mappedKVPathWithID(const string &mmapID, MMKVMode mode);

static string crcPathWithID(const string &mmapID, MMKVMode mode);


void initialize() {

    g_instanceLock = ThreadLock();

    //testAESCrypt();

    MMKVInfo("page size:%d", DEFAULT_MMAP_SIZE);
}
void CMMKV::initializeMMKV(const std::string &rootDir) {
    //线程安全,一个线程初始化一次
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    pthread_once(&once_control, initialize);

    g_rootDir = rootDir;
    char *path = strdup(g_rootDir.c_str());
    mkPath(path);
    free(path);

    MMKVInfo("root dir: %s", g_rootDir.c_str());
}


enum : bool {
    KeepSequence = false,
    IncreaseSequence = true,
};

CMMKV::CMMKV(const std::string &mmapID, int size, MMKVMode mode)
        : m_mmapID(mmapID), m_path(mappedKVPathWithID(mmapID, mode)),
          m_metaFile(crcPathWithID(m_mmapID, mode), DEFAULT_MMAP_SIZE,
                     (mode & MMKV_ASHMEM) ? MMAP_ASHMEM : MMAP_FILE),

          m_fileLock(m_metaFile.getFd()), m_sharedProcessLock(&m_fileLock, SharedLockType),
          m_exclusiveProcessLock(&m_fileLock, ExclusiveLockType),
          m_isInterProcess((mode & MMKV_MULTI_PROCESS) != 0),
          m_isAshmem((mode & MMKV_ASHMEM) != 0) {
    m_fd = -1;
    m_ptr = nullptr;
    m_size = 0;
    m_actualSize = 0;
    m_output = nullptr;
    if (m_isAshmem) {
        //匿名共享内存的id需要获取
        m_ashmemFile = new MmapedFile(m_mmapID, static_cast<size_t>(size), MMAP_ASHMEM);
        m_fd = m_ashmemFile->getFd();
    } else {
        m_ashmemFile = nullptr;
    }

    m_needLoadFromFile = true;

    //线程共享锁与进程排他锁
    m_sharedProcessLock.m_enable = m_isInterProcess;
    m_exclusiveProcessLock.m_enable = m_isInterProcess;

    // sensitive zone
    {
        SCOPEDLOCK(m_sharedProcessLock);
        loadFromFile();
    }


}

CMMKV::CMMKV(const string &mmapID, int ashmemFD, int ashmemMetaFD)
        : m_mmapID(mmapID), m_path(""), m_metaFile(ashmemMetaFD), m_fileLock(m_metaFile.getFd()),
          m_sharedProcessLock(&m_fileLock, SharedLockType),
          m_exclusiveProcessLock(&m_fileLock, ExclusiveLockType), m_isInterProcess(true),
          m_isAshmem(true) {

    // check mmapID with ashmemID
    {
        auto ashmemID = m_metaFile.getName();
        size_t pos = ashmemID.find_last_of('.');
        if (pos != string::npos) {
            ashmemID.erase(pos, string::npos);
        }
        if (mmapID != ashmemID) {
            MMKVWarning("mmapID[%s] != ashmem[%s]", mmapID.c_str(), ashmemID.c_str());
        }
    }
    m_path = string(ASHMEM_NAME_DEF) + "/" + m_mmapID;
    m_fd = ashmemFD;
    m_ptr = nullptr;
    m_size = 0;
    m_actualSize = 0;
    m_output = nullptr;

    if (m_isAshmem) {
        m_ashmemFile = new MmapedFile(m_fd);
    } else {
        m_ashmemFile = nullptr;
    }

    m_needLoadFromFile = true;

    m_sharedProcessLock.m_enable = m_isInterProcess;
    m_exclusiveProcessLock.m_enable = m_isInterProcess;

    // sensitive zone
    {
        SCOPEDLOCK(m_sharedProcessLock);
        loadFromFile();
    }
}

CMMKV::~CMMKV() {
    clearMemoryState();

    if (m_ashmemFile) {
        delete m_ashmemFile;
        m_ashmemFile = nullptr;
    }

}

void CMMKV::clearMemoryState() {
    MMKVInfo("clearMemoryState [%s]", m_mmapID.c_str());
    SCOPEDLOCK(m_lock);
    if (m_needLoadFromFile) {
        return;
    }
    m_needLoadFromFile = true;

    m_dic.clear();


    if (m_output) {
        delete m_output;
    }
    m_output = nullptr;

    if (!m_isAshmem) {
        if (m_ptr && m_ptr != MAP_FAILED) {
            if (munmap(m_ptr, m_size) != 0) {
                MMKVError("fail to munmap [%s], %s", m_mmapID.c_str(), strerror(errno));
            }
        }
        m_ptr = nullptr;

        if (m_fd >= 0) {
            if (close(m_fd) != 0) {
                MMKVError("fail to close [%s], %s", m_mmapID.c_str(), strerror(errno));
            }
        }
        m_fd = -1;
    }
    m_size = 0;
    m_actualSize = 0;
}

/**
 * 通过id生成路径
 * @param mmapID
 * @param mode
 * @return
 */
static string mappedKVPathWithID(const string &mmapID, MMKVMode mode) {
    //匿名共享内存实际就是驱动目录下的一个内存文件地址
    return (mode & MMKV_ASHMEM) == 0 ? g_rootDir + "/" + mmapID
                                     : string(ASHMEM_NAME_DEF) + "/" + mmapID;
}

/**
 * crc文件路径
 * 这个crc文件木用作为文件锁
 * @param mmapID
 * @param mode
 * @return
 */
static string crcPathWithID(const string &mmapID, MMKVMode mode) {
    return (mode & MMKV_ASHMEM) == 0 ? g_rootDir + "/" + mmapID + ".crc" : string(ASHMEM_NAME_DEF) +
                                                                           "/" + mmapID + ".crc";
}

void CMMKV::loadFromFile() {
    if (m_isAshmem) {
        loadFromAshmem();
        return;
    }

    //通过句柄打开文件
    m_fd = open(m_path.c_str(), O_RDWR | O_CREAT, S_IRWXU);
    if (m_fd < 0) {
        MMKVError("fail to open:%s, %s", m_path.c_str(), strerror(errno));
    } else {
        m_size = 0;
        struct stat st = {0};
        //获取文件大小
        if (fstat(m_fd, &st) != -1) {
            m_size = static_cast<size_t>(st.st_size);
        }
        // round up to (n * pagesize)大小重新调整是pagesize的倍数
        if (m_size < DEFAULT_MMAP_SIZE || (m_size % DEFAULT_MMAP_SIZE != 0)) {
            size_t oldSize = m_size;
            m_size = ((m_size / DEFAULT_MMAP_SIZE) + 1) * DEFAULT_MMAP_SIZE;
            //扩容
            if (ftruncate(m_fd, m_size) != 0) {
                MMKVError("fail to truncate [%s] to size %zu, %s", m_mmapID.c_str(), m_size,
                          strerror(errno));
                m_size = static_cast<size_t>(st.st_size);
            }
            //文件从oldSize开始初始化为0
            zeroFillFile(m_fd, oldSize, m_size - oldSize);
        }
        //mmap映射，ftruncate只是给file结构体进行了扩容，但是还没有对对应绑定虚拟内存进行扩容，因此需要解开一次映射后，重新mmap一次。
        m_ptr = (char *) mmap(nullptr, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
        if (m_ptr == MAP_FAILED) {
            MMKVError("fail to mmap [%s], %s", m_mmapID.c_str(), strerror(errno));
        } else {
            //写的时候会更新总大小在第一个Fixed32Size
            //获得大小m_actualSize
            memcpy(&m_actualSize, m_ptr, Fixed32Size);
            MMKVInfo("loading [%s] with %zu size in total, file size is %zu", m_mmapID.c_str(),
                     m_actualSize, m_size);
            bool loaded = false;
            //已有数据
            if (m_actualSize > 0) {
                if (m_actualSize < m_size && m_actualSize + Fixed32Size <= m_size) {

                    //m_ptr后移Fixed32Size
                    MMBuffer inputBuffer(m_ptr + Fixed32Size, m_actualSize, MMBufferNoCopy);
                    //已有数据填充到map
                    m_dic = MiniPBCoder::decodeMap(inputBuffer);
                    //还剩下的可写的字节长度
                    m_output = new CodedOutputData(m_ptr + Fixed32Size + m_actualSize,
                                                   m_size - Fixed32Size - m_actualSize);
                    loaded = true;

                }
            }
            //没有数据的处理
            if (!loaded) {
                SCOPEDLOCK(m_exclusiveProcessLock);
                if (m_actualSize > 0) {
                    writeAcutalSize(0);
                }
                m_output = new CodedOutputData(m_ptr + Fixed32Size, m_size - Fixed32Size);
            }
            MMKVInfo("loaded [%s] with %zu values", m_mmapID.c_str(), m_dic.size());
        }
    }

    if (!isFileValid()) {
        MMKVWarning("[%s] file not valid", m_mmapID.c_str());
    }

    m_needLoadFromFile = false;
}

bool CMMKV::isFileValid() {
    if (m_fd >= 0 && m_size > 0 && m_output && m_ptr && m_ptr != MAP_FAILED) {
        return true;
    }
    return false;
}

void CMMKV::writeAcutalSize(size_t actualSize) {
    assert(m_ptr != 0);
    assert(m_ptr != MAP_FAILED);

    memcpy(m_ptr, &actualSize, Fixed32Size);
    m_actualSize = actualSize;
}

/**
 * 扩容
 * @param newSize
 * @return
 */
bool CMMKV::ensureMemorySize(size_t newSize) {
    if (!isFileValid()) {
        MMKVWarning("[%s] file not valid", m_mmapID.c_str());
        return false;
    }

    if (newSize >= m_output->spaceLeft()) {
        // try a full rewrite to make space
        static const int offset = pbFixed32Size(0);
        MMBuffer data = MiniPBCoder::encodeDataWithObject(m_dic);
        size_t lenNeeded = data.length() + offset + newSize;
        if (m_isAshmem) {
            if (lenNeeded > m_size) {
                MMKVWarning("ashmem %s reach size limit:%zu, consider configure with larger size",
                            m_mmapID.c_str(), m_size);
                return false;
            }
        } else {
            //扩充大小是按照m_dic，(m_dic.size() + 1) / 2，扩充这么大，相当于*1.5
            size_t futureUsage = newSize * std::max<size_t>(8, (m_dic.size() + 1) / 2);
            // 1. no space for a full rewrite, double it
            // 2. or space is not large enough for future usage, double it to avoid frequently full rewrite
            if (lenNeeded >= m_size || (lenNeeded + futureUsage) >= m_size) {
                size_t oldSize = m_size;
                do {
                    m_size *= 2;
                } while (lenNeeded + futureUsage >= m_size);
                MMKVInfo(
                        "extending [%s] file size from %zu to %zu, incoming size:%zu, futrue usage:%zu",
                        m_mmapID.c_str(), oldSize, m_size, newSize, futureUsage);

                // if we can't extend size, rollback to old state
                if (ftruncate(m_fd, m_size) != 0) {
                    MMKVError("fail to truncate [%s] to size %zu, %s", m_mmapID.c_str(), m_size,
                              strerror(errno));
                    m_size = oldSize;
                    return false;
                }
                if (!zeroFillFile(m_fd, oldSize, m_size - oldSize)) {
                    MMKVError("fail to zeroFile [%s] to size %zu, %s", m_mmapID.c_str(), m_size,
                              strerror(errno));
                    m_size = oldSize;
                    return false;
                }

                if (munmap(m_ptr, oldSize) != 0) {
                    MMKVError("fail to munmap [%s], %s", m_mmapID.c_str(), strerror(errno));
                }
                m_ptr = (char *) mmap(m_ptr, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
                if (m_ptr == MAP_FAILED) {
                    MMKVError("fail to mmap [%s], %s", m_mmapID.c_str(), strerror(errno));
                }

                // check if we fail to make more space
                if (!isFileValid()) {
                    MMKVWarning("[%s] file not valid", m_mmapID.c_str());
                    return false;
                }
            }
        }


        writeAcutalSize(data.length());

        delete m_output;
        m_output = new CodedOutputData(m_ptr + offset, m_size - offset);
        m_output->writeRawData(data);
    }
    return true;
}

void CMMKV::loadFromAshmem() {


    if (m_fd < 0 || !m_ashmemFile) {
        MMKVError("ashmem file invalid %s, fd:%d", m_path.c_str(), m_fd);
    } else {
        m_size = m_ashmemFile->getFileSize();
        m_ptr = (char *) m_ashmemFile->getMemory();
        if (m_ptr != MAP_FAILED) {
            //m_ptr偏移Fixed32Size得到真正大小
            memcpy(&m_actualSize, m_ptr, Fixed32Size);
            MMKVInfo("loading [%s] with %zu size in total, file size is %zu", m_mmapID.c_str(),
                     m_actualSize, m_size);
            bool loaded = false;
            if (m_actualSize > 0) {
                if (m_actualSize < m_size && m_actualSize + Fixed32Size <= m_size) {
                    //缓存到MMBuffer
                    MMBuffer inputBuffer(m_ptr + Fixed32Size, m_actualSize, MMBufferNoCopy);
                    m_dic = MiniPBCoder::decodeMap(inputBuffer);
                    m_output = new CodedOutputData(m_ptr + Fixed32Size + m_actualSize,
                                                   m_size - Fixed32Size - m_actualSize);
                    loaded = true;
                }
            }
            if (!loaded) {
                SCOPEDLOCK(m_exclusiveProcessLock);
                if (m_actualSize > 0) {
                    writeAcutalSize(0);
                }
                m_output = new CodedOutputData(m_ptr + Fixed32Size, m_size - Fixed32Size);

            }
            MMKVInfo("loaded [%s] with %zu values", m_mmapID.c_str(), m_dic.size());
        }
    }

    if (!isFileValid()) {
        MMKVWarning("[%s] ashmem not valid", m_mmapID.c_str());
    }

    m_needLoadFromFile = false;
}


/**
* 存储数据
* @param value
* @param key
* @return
*/
bool CMMKV::setStringForKey(const std::string &value, const std::string &key) {
    if (key.empty()) {
        return false;
    }
    //编码压缩内容
    auto data = MiniPBCoder::encodeDataWithObject(value);
    return setDataForKey(std::move(data), key);
}

bool CMMKV::setDataForKey(MMBuffer &&data, const std::string &key) {
    if (data.length() == 0 || key.empty()) {
        return false;
    }
    SCOPEDLOCK(m_lock);
    SCOPEDLOCK(m_exclusiveProcessLock);
    checkLoadData();

    // m_dic[key] = std::move(data);
    auto itr = m_dic.find(key);
    if (itr == m_dic.end()) {
        itr = m_dic.emplace(key, std::move(data)).first;
    } else {
        itr->second = std::move(data);
    }

    return appendDataWithKey(itr->second, key);
}
/**
 * 校验数据
 */
void CMMKV::checkLoadData() {
    //只从本地加载一次
    if (m_needLoadFromFile) {
        SCOPEDLOCK(m_sharedProcessLock);

        m_needLoadFromFile = false;
        loadFromFile();
        return;
    }
    if (!m_isInterProcess) {
        return;
    }
}

/**
 * 保存数据都是拼接在最后的，需要key,key的大小，数据，数据的大小
 * @param data
 * @param key
 * @return
 */
bool CMMKV::appendDataWithKey(const MMBuffer &data, const std::string &key) {
    size_t keyLength = key.length();
    // size needed to encode the key
    size_t size = keyLength + pbRawVarint32Size((int32_t) keyLength);
    // size needed to encode the value
    size += data.length() + pbRawVarint32Size((int32_t) data.length());

    SCOPEDLOCK(m_exclusiveProcessLock);

    bool hasEnoughSize = ensureMemorySize(size);

    if (!hasEnoughSize || !isFileValid()) {
        return false;
    }
    if (m_actualSize == 0) {
        auto allData = MiniPBCoder::encodeDataWithObject(m_dic);
        if (allData.length() > 0) {
            writeAcutalSize(allData.length());
            m_output->writeRawData(allData); // note: don't write size of data
            return true;
        }
        return false;
    } else {
        writeAcutalSize(m_actualSize + size);
        m_output->writeString(key);
        m_output->writeData(data); // note: write size of data

        return true;
    }
}

bool CMMKV::setBytesForKey(const MMBuffer &value, const std::string &key) {
    if (key.empty()) {
        return false;
    }
    auto data = MiniPBCoder::encodeDataWithObject(value);
    return setDataForKey(std::move(data), key);
}

bool CMMKV::setBool(bool value, const std::string &key) {
    if (key.empty()) {
        return false;
    }
    size_t size = pbBoolSize(value);
    MMBuffer data(size);
    CodedOutputData output(data.getPtr(), size);
    output.writeBool(value);

    return setDataForKey(std::move(data), key);
}

bool CMMKV::setInt32(int32_t value, const std::string &key) {
    if (key.empty()) {
        return false;
    }
    size_t size = pbInt32Size(value);
    MMBuffer data(size);
    CodedOutputData output(data.getPtr(), size);
    output.writeInt32(value);

    return setDataForKey(std::move(data), key);
}

bool CMMKV::setInt64(int64_t value, const std::string &key) {
    if (key.empty()) {
        return false;
    }
    size_t size = pbInt64Size(value);
    MMBuffer data(size);
    CodedOutputData output(data.getPtr(), size);
    output.writeInt64(value);

    return setDataForKey(std::move(data), key);
}

bool CMMKV::setFloat(float value, const std::string &key) {
    if (key.empty()) {
        return false;
    }
    size_t size = pbFloatSize(value);
    MMBuffer data(size);
    CodedOutputData output(data.getPtr(), size);
    output.writeFloat(value);

    return setDataForKey(std::move(data), key);
}

bool CMMKV::setDouble(double value, const std::string &key) {
    if (key.empty()) {
        return false;
    }
    size_t size = pbDoubleSize(value);
    MMBuffer data(size);
    CodedOutputData output(data.getPtr(), size);
    output.writeDouble(value);

    return setDataForKey(std::move(data), key);
}
bool CMMKV::setVectorForKey(const std::vector<std::string> &v, const std::string &key) {
    if (key.empty()) {
        return false;
    }
    auto data = MiniPBCoder::encodeDataWithObject(v);
    return setDataForKey(std::move(data), key);
}

bool CMMKV::getStringForKey(const std::string &key, std::string &result) {
    if (key.empty()) {
        return false;
    }
    auto &data = getDataForKey(key);
    if (data.length() > 0) {
        result = MiniPBCoder::decodeString(data);
        return true;
    }
    return false;
}

const MMBuffer &CMMKV::getDataForKey(const std::string &key) {
    SCOPEDLOCK(m_lock);
    checkLoadData();
    auto itr = m_dic.find(key);
    if (itr != m_dic.end()) {
        return itr->second;
    }
    static MMBuffer nan(0);
    return nan;
}

MMBuffer CMMKV::getBytesForKey(const std::string &key) {
    if (key.empty()) {
        return MMBuffer(0);
    }
    auto &data = getDataForKey(key);
    if (data.length() > 0) {
        return MiniPBCoder::decodeBytes(data);
    }
    return MMBuffer(0);
}

bool CMMKV::getBoolForKey(const std::string &key, bool defaultValue) {
    if (key.empty()) {
        return defaultValue;
    }
    auto &data = getDataForKey(key);
    if (data.length() > 0) {
        CodedInputData input(data.getPtr(), data.length());
        return input.readBool();
    }
    return defaultValue;
}

int32_t CMMKV::getInt32ForKey(const std::string &key, int32_t defaultValue) {
    if (key.empty()) {
        return defaultValue;
    }
    auto &data = getDataForKey(key);
    if (data.length() > 0) {
        CodedInputData input(data.getPtr(), data.length());
        return input.readInt32();
    }
    return defaultValue;
}

int64_t CMMKV::getInt64ForKey(const std::string &key, int64_t defaultValue) {
    if (key.empty()) {
        return defaultValue;
    }
    auto &data = getDataForKey(key);
    if (data.length() > 0) {
        CodedInputData input(data.getPtr(), data.length());
        return input.readInt64();
    }
    return defaultValue;
}

float CMMKV::getFloatForKey(const std::string &key, float defaultValue) {
    if (key.empty()) {
        return defaultValue;
    }
    auto &data = getDataForKey(key);
    if (data.length() > 0) {
        CodedInputData input(data.getPtr(), data.length());
        return input.readFloat();
    }
    return defaultValue;
}

double CMMKV::getDoubleForKey(const std::string &key, double defaultValue) {
    if (key.empty()) {
        return defaultValue;
    }
    auto &data = getDataForKey(key);
    if (data.length() > 0) {
        CodedInputData input(data.getPtr(), data.length());
        return input.readDouble();
    }
    return defaultValue;
}

bool CMMKV::getVectorForKey(const std::string &key, std::vector<std::string> &result) {
    if (key.empty()) {
        return false;
    }
    auto &data = getDataForKey(key);
    if (data.length() > 0) {
        result = MiniPBCoder::decodeSet(data);
        return true;
    }
    return false;
}