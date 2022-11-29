#pragma once

#include <vector>

#include "memory.hpp"
#include "cache.hpp"

class Cache;

class Bus {
public:
    explicit Bus(RAM *ram);
    
    void addCache(Cache *cache);
    
    //
    // These are exactly the same on purpose
    //
    void setMemory(uint16_t address, uint8_t item);
    void setMemory(uint32_t address, uint32_t item);
    uint32_t getMemory(uint32_t address);
    
    int stalls = 0;
private:
    RAM *ram;
    std::vector<Cache *> cache_cluster;
    
    uint32_t readAddress = -1;
    uint32_t writeAddress = -1;
};

