#pragma once

#include <cstdint>

struct CacheEntry {
    uint8_t *data;
    uint32_t tag = 0;
    uint32_t time = 0;
    bool valid = false;
};

class DirectCache {
public:
    explicit DirectCache(int size, int line_size, int assoc = 1);
    bool containsAddress(uint32_t address);
    bool setData(uint32_t address, uint8_t data);
private:
    CacheEntry **cache;
    int blocks = 0;
    
    int tag_width = 0;
    int offset_width = 0;
    int set_width = 0;
};

// 2-Way Associative
//
// Offset address = log2(32) = 5 bits
// Total number of lines = 256/32 = 8 Bytes
// Number of sets = 32 / 4 (assoc) = 8
// Set address width = log2(8) = 3
// TAG = 32 - (3) - (5) = 24
/*class AssociativeCache {
public:
    explicit AssociativeCache(int size, int line_size);
    bool containsAddress(uint32_t address);
    bool setData(uint32_t address, uint8_t data);
private:
    CacheEntry **cache;
    int blocks = 0;
};*/

