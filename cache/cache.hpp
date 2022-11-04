#pragma once

#include <cstdint>

struct CacheEntry {
    uint8_t *data;
    uint32_t tag = 0;
    uint32_t time = 0;
    bool valid = false;
};

class Cache {
public:
    explicit Cache(int size, int line_size, int assoc = 1);
    bool containsAddress(uint32_t address);
    bool setData(uint32_t address, uint8_t data);
private:
    CacheEntry **cache;
    int blocks = 0;
    
    int tag_width = 0;
    int offset_width = 0;
    int set_width = 0;
};

