#include <cstdio>
#include <cstring>

#include "cpu.hpp"

//
// Loads the memory
//
void CPU::loadMemory(std::string path, int size) {
    FILE *file = fopen(path.c_str(), "rb");
    
    /*int size = 0;
    while (feof(file) == 0) {
        fgetc(file);
        ++size;
    }*/
    
    std::cout << "Memory size: " << size << std::endl;
    
    /*rewind(file);*/
    
    memory = new uint8_t[size+1];
    memory_size = size+1;
    int index = 0;
    while (feof(file) == 0 && index < size) {
        memory[index] = fgetc(file);
        ++index;
    }
    
    fclose(file);
}

//
// Loads a program to the memory and sets the program counter
//
void CPU::loadProgram(std::string path, int start) {
    FILE *file = fopen(path.c_str(), "rb");
    int index = start;
    
    while (!feof(file)) {
        uint8_t data = (uint8_t)fgetc(file);
        memory[index] = data;
        ++index;
    }

    pc = start;
}

//
// Sets the contents of a particular location in memory
//
void CPU::setMemory(uint16_t address, uint8_t item) {
    memory[address] = item;
}

//
// Sets a 32-bit location in memory
//
void CPU::setMemory(uint32_t address, uint32_t item) {
    memcpy(&memory[address], &item, sizeof(uint32_t));
}

//
// Returns a 32-bit location in memory
//
uint32_t CPU::getMemory(uint32_t address) {
    uint32_t item = 0;
    memcpy(&item, &memory[address], sizeof(uint32_t));
    return item;
}

//
// Flushes our memory file back to the disk
//
void CPU::flushMemory(std::string path) {
    FILE *file = fopen(path.c_str(), "wb");
    
    for (int i = 0; i<memory_size; i++) {
        fwrite(&memory[i], sizeof(uint8_t), 1, file);
    }
    
    fclose(file);
}

