#pragma once

#include <cstdint>

#include "bus.hpp"

struct CacheEntry {
    uint8_t *data;
    uint32_t tag = 0;
    uint32_t time = 0;
    bool valid = false;
};

class Cache {
public:
    explicit Cache(int size, int line_size, int assoc = 1);
    void setBus(Bus *bus) { this->bus = bus; }
    void print();
    uint32_t getData(int clock_cycle, uint32_t address, int size);
    bool setData(int clock_cycle, uint32_t address, uint32_t data, int size);
    
    int ticks = 0;
private:
    CacheEntry **cache;
    Bus *bus;
    int blocks = 0;
    int line_size = 0;
    
    int tag_width = 0;
    int offset_width = 0;
    int set_width = 0;
};

