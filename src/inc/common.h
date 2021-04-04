#pragma once

#include <stdint.h>

union address_t {
    struct {
        uint32_t    low;
        uint32_t    high;
    };
    uint64_t        val;
    void            *ptr;

    address_t(void* ptr) : ptr(ptr) {}
    address_t(uint64_t val) : val(val) {}
    address_t(uint32_t low, uint32_t high) : low(low), high(high) {}
};

struct Allocation {
    size_t size;
    size_t align;
    uint64_t address;
};

// platform/util/utils.h IsPowerOfTwo, don't check input 0
template <typename T> bool IsPow2(T a) {
    return a && ((a & (a - 1)) == 0);
}

template <typename T> uint32_t Log2(T a) {
    return ((a < 2)? 0 : 1 + Log2(a >> 1));
}

/*
template <typename T> T Align(T a, T b) {
    return (a + b - 1) / b * b;
}

template <typename T> T AlignDown(T a, T b) {
    return a / b * b;
}
*/

inline uint32_t High(uint64_t value) {
    return (value & 0xFFFFFFFF00000000) >> 32;
}

inline uint32_t Low(uint64_t value) {
    return (value & 0x00000000);
}

inline uint64_t PackHighLow(uint32_t hi, uint32_t lo) {
    uint64_t ret = hi;
    ret = (ret << 32 ) | lo;
    return ret;
}

