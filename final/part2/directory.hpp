#pragma once

#include <cstdint>

enum class State {
    Exclusive,
    Shared,
    Uncached
};

struct DirEntry {
    uint32_t tag = 0;                   // The location
    uint8_t lines;                      // The CPU lines
    State state = State::Uncached;      // The state
    uint8_t dirty;                      // The CPU line that contains the dirty bit
};

class Directory {
public:
    explicit Directory(int size, int line_size, int assoc = 1);
    void print();
    void setLine(uint32_t address, uint8_t pos, bool shared = false, bool write = false);
    int checkDirectory(uint32_t address, uint8_t pos);
private:
    DirEntry **directory;
    int blocks = 0;
    int line_size = 0;
    
    int tag_width = 0;
    int offset_width = 0;
    int set_width = 0;
};

