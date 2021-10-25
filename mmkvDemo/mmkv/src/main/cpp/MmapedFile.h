//
// Created by Charles on 19/7/22.
//匿名共享内存的封装
//

#ifndef MMKV_MMAPEDFILE_H
#define MMKV_MMAPEDFILE_H

#include <string>
#include <sys/ioctl.h>

#define ASHMEM_NAME_LEN 256
#define ASHMEM_NAME_DEF "/dev/ashmem"

#define __ASHMEMIOC 0x77
#define ASHMEM_SET_NAME _IOW(__ASHMEMIOC, 1, char[ASHMEM_NAME_LEN])
#define ASHMEM_GET_NAME _IOR(__ASHMEMIOC, 2, char[ASHMEM_NAME_LEN])
#define ASHMEM_SET_SIZE _IOW(__ASHMEMIOC, 3, size_t)
#define ASHMEM_GET_SIZE _IO(__ASHMEMIOC, 4)

enum : bool { MMAP_FILE = false, MMAP_ASHMEM = true };

extern const int DEFAULT_MMAP_SIZE;

class MmapedFile {
    std::string m_name;
    int m_fd;
    void *m_segmentPtr;
    size_t m_segmentSize;

    // just forbid it for possibly misuse
    MmapedFile(const MmapedFile &other) = delete;
    MmapedFile &operator=(const MmapedFile &other) = delete;

public:
    MmapedFile(const std::string &path,
               size_t size = static_cast<size_t>(DEFAULT_MMAP_SIZE),
               bool fileType = MMAP_FILE);
    MmapedFile(int ashmemFD);
    ~MmapedFile();

    const bool m_fileType;

    size_t getFileSize() { return m_segmentSize; }

    void *getMemory() { return m_segmentPtr; }

    std::string &getName() { return m_name; }

    int getFd() { return m_fd; }
};

class MMBuffer;

extern bool mkPath(char *path);
extern bool isFileExist(const std::string &nsFilePath);
extern bool removeFile(const std::string &nsFilePath);
extern MMBuffer *readWholeFile(const char *path);
extern bool zeroFillFile(int fd, size_t startPos, size_t size);

#endif //MMKV_MMAPEDFILE_H
