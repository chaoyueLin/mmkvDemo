

#include "MiniPBCoder.h"
#include "CodedInputData.h"
#include "CodedOutputData.h"
#include "MMBuffer.h"

#include "PBUtility.h"
#include <string>
#include <sys/stat.h>
#include <vector>

using namespace std;

MiniPBCoder::MiniPBCoder() {
    m_inputBuffer = nullptr;
    m_inputData = nullptr;

    m_outputBuffer = nullptr;
    m_outputData = nullptr;

}

MiniPBCoder::~MiniPBCoder() {
    if (m_inputData) {
        delete m_inputData;
    }
    if (m_outputBuffer) {
        delete m_outputBuffer;
    }
    if (m_outputData) {
        delete m_outputData;
    }
}

MiniPBCoder::MiniPBCoder(const MMBuffer *inputBuffer) : MiniPBCoder() {
    m_inputBuffer = inputBuffer;
    m_inputData =
            new CodedInputData(m_inputBuffer->getPtr(),
                               static_cast<int32_t>(m_inputBuffer->length()));
}


string MiniPBCoder::decodeOneString() {
    return m_inputData->readString();
}

MMBuffer MiniPBCoder::decodeOneBytes() {
    return m_inputData->readData();
}

vector<string> MiniPBCoder::decodeOneSet() {
    vector<string> v;

    auto length = m_inputData->readInt32();

    while (!m_inputData->isAtEnd()) {
        const auto &value = m_inputData->readString();
        v.push_back(move(value));
    }

    return v;
}

unordered_map<string, MMBuffer> MiniPBCoder::decodeOneMap(size_t size) {
    unordered_map<string, MMBuffer> dic;

    if (size == 0) {
        auto length = m_inputData->readInt32();
    }
    while (!m_inputData->isAtEnd()) {
        const auto &key = m_inputData->readString();
        if (key.length() > 0) {
            auto value = m_inputData->readData();
            if (value.length() > 0) {
                dic[key] = move(value);
            } else {
                dic.erase(key);
            }
        }
    }
    return dic;
}

string MiniPBCoder::decodeString(const MMBuffer &oData) {
    MiniPBCoder oCoder(&oData);
    return oCoder.decodeOneString();
}

MMBuffer MiniPBCoder::decodeBytes(const MMBuffer &oData) {
    MiniPBCoder oCoder(&oData);
    return oCoder.decodeOneBytes();
}

unordered_map<string, MMBuffer> MiniPBCoder::decodeMap(const MMBuffer &oData, size_t size) {
    MiniPBCoder oCoder(&oData);
    return oCoder.decodeOneMap(size);
}

vector<string> MiniPBCoder::decodeSet(const MMBuffer &oData) {
    MiniPBCoder oCoder(&oData);
    return oCoder.decodeOneSet();
}


MMBuffer MiniPBCoder::getEncodeData(const string &str) {

    int32_t valueSize = static_cast<int32_t>(str.size());

    uint32_t compiledSize = pbRawVarint32Size(valueSize) + valueSize;

    if (compiledSize > 0) {
        m_outputBuffer = new MMBuffer(compiledSize);
        m_outputData = new CodedOutputData(m_outputBuffer->getPtr(), m_outputBuffer->length());
        m_outputData->writeString(str);
    }

    return move(*m_outputBuffer);
}

MMBuffer MiniPBCoder::getEncodeData(const MMBuffer &buffer) {
    uint32_t compiledSize = pbRawVarint32Size(buffer.length()) + buffer.length();

    if (compiledSize > 0) {
        m_outputBuffer = new MMBuffer(compiledSize);
        m_outputData = new CodedOutputData(m_outputBuffer->getPtr(), m_outputBuffer->length());

        m_outputData->writeData(buffer);
    }

    return move(*m_outputBuffer);
}

MMBuffer MiniPBCoder::getEncodeData(const vector<string> &v) {
    uint32_t compiledSize;
    for (const auto &str : v) {
        if (str.length() <= 0) {
            continue;
        }
        int32_t valueSize = static_cast<int32_t>(str.size());
        compiledSize += pbRawVarint32Size(valueSize) + valueSize;
    }
    compiledSize = pbRawVarint32Size(compiledSize) + compiledSize;
    if (compiledSize > 0) {
        m_outputBuffer = new MMBuffer(compiledSize);
        m_outputData = new CodedOutputData(m_outputBuffer->getPtr(), m_outputBuffer->length());
        m_outputData->writeRawVarint32(compiledSize);
    }

    return move(*m_outputBuffer);
}

MMBuffer MiniPBCoder::getEncodeData(const unordered_map<string, MMBuffer> &map) {
    uint32_t compiledSize;
    for (const auto &itr : map) {
        const auto &key = itr.first;
        const auto &value = itr.second;
        if (key.length() <= 0) {
            continue;
        }
        if (value.length() <= 0) {
            continue;
        }
        int32_t keyIndex = static_cast<int32_t>(key.size());
        compiledSize += pbRawVarint32Size(keyIndex) + keyIndex;

        int32_t valueIndex = static_cast<int32_t>(key.size());
        compiledSize += pbRawVarint32Size(valueIndex) + valueIndex;
    }
    compiledSize = pbRawVarint32Size(compiledSize) + compiledSize;
    if (compiledSize > 0) {
        m_outputBuffer = new MMBuffer(compiledSize);
        m_outputData = new CodedOutputData(m_outputBuffer->getPtr(), m_outputBuffer->length());
        m_outputData->writeRawVarint32(compiledSize);
    }

    return std::move(*m_outputBuffer);
}



