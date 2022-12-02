#pragma once

#include <cstdint>
#include <vector>

#include "directory.hpp"

class Directory;

struct CacheEntry {
    uint8_t *data;
    uint32_t tag = 0;
    bool valid = false;
};

class Cache {
public:
    explicit Cache(int size, int line_size, int assoc = 1);
    void setDirectory(Directory *dir);
    void print();
    void invalidate(uint32_t address);
    uint32_t getData(uint32_t address, int size);
    std::vector<uint32_t> getLineData(uint32_t address);
    bool setData(uint32_t address, uint32_t data, int size);
    
    int id = 0;
private:
    CacheEntry **cache;
    Directory *dir;
    int blocks = 0;
    int line_size = 0;
    
    int tag_width = 0;
    int offset_width = 0;
    int set_width = 0;
};

