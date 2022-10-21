#pragma once

#include <string>
#include <cstdint>

class RAM {
public:
    RAM() {}
    
    //
    // Memory controller functions
    //
    void loadMemory(std::string path, int size);
    void loadProgram(std::string path, int start = 0);
    void setMemory(uint16_t address, uint8_t item);
    void setMemory(uint32_t address, uint32_t item);
    uint32_t getMemory(uint32_t address);
    void flushMemory(std::string path);
    
private:
    uint8_t *memory;
    int memory_size = 0;
};

