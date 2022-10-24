#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <thread>

using namespace std;

#include "cpu.hpp"
#include "memory.hpp"

void test1(RAM *ram) {
    uint32_t count = 10;
    uint32_t index = 6;

    uint32_t address1 = 0x0400;
    for (int i = address1; i<(address1 + count * 4); i+=4) {
        ram->setMemory(i, index);
        ++index;
    }
    
    uint32_t address2 = 0x0800;
    index = 1;
    for (int i = address2; i<(address2 + count * 4); i+=4) {
        ram->setMemory(i, index);
        ++index;
    }
}

void run0(std::string input, std::string memory, RAM *ram, int start) {
    CPU *cpu = new CPU;
    cpu->setRAM(ram);
    
    ram->loadProgram(input, start);
    cpu->pc = start;
    ram->flushMemory(memory);
    
    cpu->run();
    
    // Print the registers
    std::cout << "==CPU" << start << "=========================================" << std::endl;
    cpu->debugRegisters();
    ram->flushMemory(memory);
    std::cout << "===============================================" << std::endl;
}

/*void run1(RAM *ram) {
    ram->loadProgram(input2, 0x100);
    ram->flushMemory(memory);
    
    CPU *cpu2 = new CPU;
    cpu2->setRAM(ram);
    cpu2->pc = 0x100;
    cpu2->run();
    
    std::cout << "==CPU1=========================================" << std::endl;
    cpu2->debugRegisters();
    ram->flushMemory(memory);
    std::cout << "===============================================" << std::endl;
}*/

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cerr << "Error: Invalid input syntax!" << std::endl;
        std::cerr << "Expected: cpu [program] [memory file]" << std::endl;
        return 1;
    }
    
    std::string input = argv[1];
    std::string memory = argv[2];
    std::string input2 = "";
    
    if (argc == 4) {
        input2 = memory;
        memory = argv[3];
    }
    
    RAM *ram = new RAM();
    ram->loadMemory(memory, 5400);
    
    // Set some values in memory
    test1(ram);
    
    
    /*run0(input, memory, ram, 0);
    
    
    
    
    //
    // CPU 2
    //
    if (input2 != "") {
        run0(input2, memory, ram, 0x0100);
    }*/
    
    // If we have multiple inputs, then thread
    if (input2 != "") {
        std::thread t1(run0, input, memory, ram, 0);
        std::thread t2(run0, input2, memory, ram, 0x0100);
        
        t1.join();
        t2.join();
    } else {
        run0(input, memory, ram, 0);
    }
    
    // TODO: Print CPI here
    /*std::cout << "=====================================" << std::endl;
    std::cout << "Clock Cycles: " << cpu->clock_cycles << std::endl;
    std::cout << "Instruction Count: " << cpu->i_count << std::endl;
    std::cout << "CPI = Clock Cycles / Instruction Count = " << (float)cpu->clock_cycles / cpu->i_count << std::endl;
    std::cout << "=====================================" << std::endl;*/
    
    return 0;
}

