//
// Created by Charles on 2021/12/26.
//

#ifndef CMMKV_PBUTILITY_H
#define CMMKV_PBUTILITY_H


#include "cpplang.hpp"
#include <cstdint>

template <typename T, typename P>
union Converter {
    static_assert(sizeof(T) == sizeof(P), "size not match");
    T first;
    P second;
};


static inline int64_t Float64ToInt64(double v) {
    Converter<double, int64_t> converter;
    converter.first = v;
    return converter.second;
}

static inline int32_t Float32ToInt32(float v) {
    Converter<float, int32_t> converter;
    converter.first = v;
    return converter.second;
}

static inline double Int64ToFloat64(int64_t v) {
    Converter<double, int64_t> converter;
    converter.second = v;
    return converter.first;
}

static inline float Int32ToFloat32(int32_t v) {
    Converter<float, int32_t> converter;
    converter.second = v;
    return converter.first;
}

static inline uint64_t Int64ToUInt64(int64_t v) {
    Converter<int64_t, uint64_t> converter;
    converter.first = v;
    return converter.second;
}

static inline int64_t UInt64ToInt64(uint64_t v) {
    Converter<int64_t, uint64_t> converter;
    converter.second = v;
    return converter.first;
}

static inline uint32_t Int32ToUInt32(int32_t v) {
    Converter<int32_t, uint32_t> converter;
    converter.first = v;
    return converter.second;
}

static inline int32_t UInt32ToInt32(uint32_t v) {
    Converter<int32_t, uint32_t> converter;
    converter.second = v;
    return converter.first;
}

static inline int32_t logicalRightShift32(int32_t value, int32_t spaces) {
    return UInt32ToInt32((Int32ToUInt32(value) >> spaces));
}

static inline int64_t logicalRightShift64(int64_t value, int32_t spaces) {
    return UInt64ToInt64((Int64ToUInt64(value) >> spaces));
}

constexpr uint32_t littleEdian32Size = 4;

constexpr uint32_t pbFloatSize(float value) {
    return littleEdian32Size;
}

constexpr uint32_t pbFixed32Size(int32_t value) {
    return littleEdian32Size;
}

constexpr uint32_t littleEdian64Size = 8;

constexpr uint32_t pbDoubleSize(double value) {
    return littleEdian64Size;
}

constexpr uint32_t pbBoolSize(bool value) {
    return 1;
}

extern uint32_t pbRawVarint32Size(int32_t value);

extern uint32_t pbInt64Size(int64_t value);

static inline uint32_t pbInt32Size(int32_t value) {
    if (value >= 0) {
        return pbRawVarint32Size(value);
    } else {
        return 10;
    }
}
#endif //CMMKV_PBUTILITY_H
