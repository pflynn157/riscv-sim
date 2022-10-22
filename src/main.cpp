#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include <fstream>
#include <cstring>

using namespace std;

#include "cpu.hpp"
#include "memory.hpp"

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cerr << "Error: Invalid input syntax!" << std::endl;
        std::cerr << "Expected: cpu [program] [memory file]" << std::endl;
        return 1;
    }
    
    std::string input = argv[1];
    std::string memory = argv[2];
    
    RAM *ram = new RAM();
    ram->loadMemory(memory, 4096);
    
    CPU *cpu = new CPU;
    cpu->setRAM(ram);
    
    // For the sake of the program, set some float memory
    // Array A
    uint32_t address = 0x0400;
    float start = 1.1f;
    for (int i = address; i<(address + (256*4)); i+=4) {
        uint32_t mem = 0;
        memcpy(&mem, &start, sizeof(uint32_t));
        ram->setMemory(i, mem);
        
        start += 1.2f;
    }
    
    // Array B
    address = 0x0800;
    start = 2.1f;
    for (int i = address; i<(address + (256*4)); i+=4) {
        uint32_t mem = 0;
        memcpy(&mem, &start, sizeof(uint32_t));
        ram->setMemory(i, mem);
        
        start += 1.0f;
    }
    
    ram->loadProgram(input, 0x0);
    cpu->pc = 0;
    ram->flushMemory(memory);
    
    // Now, run the decoder from the last stage
    cpu->run();
    
    // Print the registers
    cpu->debugRegisters();
    ram->flushMemory(memory);
    
    return 0;
}

