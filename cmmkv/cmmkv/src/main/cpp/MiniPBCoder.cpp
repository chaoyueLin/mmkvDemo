

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
    m_encodeItems = nullptr;
}

MiniPBCoder::~MiniPBCoder() {
    //m_inputBuffer不能回收，返回给用move
//    if (m_inputBuffer) {
//        delete m_inputBuffer;
//    }
    if (m_inputData) {
        delete m_inputData;
    }

    if (m_outputBuffer) {
        delete m_outputBuffer;
    }
    if (m_outputData) {
        delete m_outputData;
    }
    if (m_encodeItems) {
        delete m_encodeItems;
    }
}

MiniPBCoder::MiniPBCoder(const MMBuffer *inputBuffer) : MiniPBCoder() {
    m_inputBuffer = inputBuffer;
    m_inputData =
            new CodedInputData(m_inputBuffer->getPtr(),
                               static_cast<int32_t>(m_inputBuffer->length()));
}


void MiniPBCoder::writeRootObject() {
    for (size_t index = 0, total = m_encodeItems->size(); index < total; index++) {
        PBEncodeItem *encodeItem = &(*m_encodeItems)[index];
        switch (encodeItem->type) {
            case PBEncodeItemType_String: {
                m_outputData->writeString(*(encodeItem->value.strValue));
                break;
            }
            case PBEncodeItemType_Data: {
                m_outputData->writeData(*(encodeItem->value.bufferValue));
                break;
            }
            case PBEncodeItemType_Container: {
                m_outputData->writeRawVarint32(encodeItem->valueSize);
                break;
            }
            case PBEncodeItemType_None: {
                MMKVError("%d", encodeItem->type);
                break;
            }
        }
    }
}

size_t MiniPBCoder::prepareObjectForEncode(const string &str) {
    m_encodeItems->push_back(PBEncodeItem());
    PBEncodeItem *encodeItem = &(m_encodeItems->back());
    size_t index = m_encodeItems->size() - 1;
    {
        encodeItem->type = PBEncodeItemType_String;
        encodeItem->value.strValue = &str;
        encodeItem->valueSize = static_cast<int32_t>(str.size());
    }
    encodeItem->compiledSize = pbRawVarint32Size(encodeItem->valueSize) + encodeItem->valueSize;

    return index;
}

size_t MiniPBCoder::prepareObjectForEncode(const MMBuffer &buffer) {
    m_encodeItems->push_back(PBEncodeItem());
    PBEncodeItem *encodeItem = &(m_encodeItems->back());
    size_t index = m_encodeItems->size() - 1;
    {
        encodeItem->type = PBEncodeItemType_Data;
        encodeItem->value.bufferValue = &buffer;
        encodeItem->valueSize = buffer.length();
    }
    encodeItem->compiledSize = pbRawVarint32Size(encodeItem->valueSize) + encodeItem->valueSize;

    return index;
}

size_t MiniPBCoder::prepareObjectForEncode(const vector<string> &v) {
    m_encodeItems->push_back(PBEncodeItem());
    PBEncodeItem *encodeItem = &(m_encodeItems->back());
    size_t index = m_encodeItems->size() - 1;
    {
        encodeItem->type = PBEncodeItemType_Container;
        encodeItem->value.strValue = nullptr;

        for (const auto &str : v) {
            size_t itemIndex = prepareObjectForEncode(str);
            if (itemIndex < m_encodeItems->size()) {
                (*m_encodeItems)[index].valueSize += (*m_encodeItems)[itemIndex].compiledSize;
            }
        }

        encodeItem = &(*m_encodeItems)[index];
    }
    encodeItem->compiledSize = pbRawVarint32Size(encodeItem->valueSize) + encodeItem->valueSize;

    return index;
}

size_t MiniPBCoder::prepareObjectForEncode(const unordered_map<string, MMBuffer> &map) {
    m_encodeItems->push_back(PBEncodeItem());
    PBEncodeItem *encodeItem = &(m_encodeItems->back());
    size_t index = m_encodeItems->size() - 1;
    {
        encodeItem->type = PBEncodeItemType_Container;
        encodeItem->value.strValue = nullptr;

        for (const auto &itr : map) {
            const auto &key = itr.first;
            const auto &value = itr.second;
            if (key.length() <= 0) {
                continue;
            }

            size_t keyIndex = prepareObjectForEncode(key);
            if (keyIndex < m_encodeItems->size()) {
                size_t valueIndex = prepareObjectForEncode(value);
                if (valueIndex < m_encodeItems->size()) {
                    (*m_encodeItems)[index].valueSize += (*m_encodeItems)[keyIndex].compiledSize;
                    (*m_encodeItems)[index].valueSize += (*m_encodeItems)[valueIndex].compiledSize;
                } else {
                    m_encodeItems->pop_back(); // pop key
                }
            }
        }

        encodeItem = &(*m_encodeItems)[index];
    }
    encodeItem->compiledSize = pbRawVarint32Size(encodeItem->valueSize) + encodeItem->valueSize;

    return index;
}

MMBuffer MiniPBCoder::getEncodeData(const string &str) {
    m_encodeItems = new vector<PBEncodeItem>();
    size_t index = prepareObjectForEncode(str);
    PBEncodeItem *oItem = (index < m_encodeItems->size()) ? &(*m_encodeItems)[index] : nullptr;
    if (oItem && oItem->compiledSize > 0) {
        m_outputBuffer = new MMBuffer(oItem->compiledSize);
        m_outputData = new CodedOutputData(m_outputBuffer->getPtr(), m_outputBuffer->length());

        writeRootObject();
    }

    return move(*m_outputBuffer);
}

MMBuffer MiniPBCoder::getEncodeData(const MMBuffer &buffer) {
    m_encodeItems = new vector<PBEncodeItem>();
    size_t index = prepareObjectForEncode(buffer);
    PBEncodeItem *oItem = (index < m_encodeItems->size()) ? &(*m_encodeItems)[index] : nullptr;
    if (oItem && oItem->compiledSize > 0) {
        m_outputBuffer = new MMBuffer(oItem->compiledSize);
        m_outputData = new CodedOutputData(m_outputBuffer->getPtr(), m_outputBuffer->length());

        writeRootObject();
    }

    return move(*m_outputBuffer);
}

MMBuffer MiniPBCoder::getEncodeData(const vector<string> &v) {
    m_encodeItems = new vector<PBEncodeItem>();
    size_t index = prepareObjectForEncode(v);
    PBEncodeItem *oItem = (index < m_encodeItems->size()) ? &(*m_encodeItems)[index] : nullptr;
    if (oItem && oItem->compiledSize > 0) {
        m_outputBuffer = new MMBuffer(oItem->compiledSize);
        m_outputData = new CodedOutputData(m_outputBuffer->getPtr(), m_outputBuffer->length());

        writeRootObject();
    }

    return move(*m_outputBuffer);
}

MMBuffer MiniPBCoder::getEncodeData(const unordered_map<string, MMBuffer> &map) {
    m_encodeItems = new vector<PBEncodeItem>();
    size_t index = prepareObjectForEncode(map);
    PBEncodeItem *oItem = (index < m_encodeItems->size()) ? &(*m_encodeItems)[index] : nullptr;
    if (oItem && oItem->compiledSize > 0) {
        m_outputBuffer = new MMBuffer(oItem->compiledSize);
        m_outputData = new CodedOutputData(m_outputBuffer->getPtr(), m_outputBuffer->length());

        writeRootObject();
    }

    return std::move(*m_outputBuffer);
}

#pragma mark - decode

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
                //这里是move的，m_inputBuffer不能回收
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



