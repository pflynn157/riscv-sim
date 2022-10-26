#pragma once

#include "memory.hpp"

class Bus {
public:
    explicit Bus(RAM *ram) {
        this->ram = ram;
    }
    
    //
    // These are exactly the same on purpose
    //
    void setMemory(uint16_t address, uint8_t item);
    void setMemory(uint32_t address, uint32_t item);
    uint32_t getMemory(uint32_t address);
private:
    RAM *ram;
};

