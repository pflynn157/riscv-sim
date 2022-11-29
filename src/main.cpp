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
#include "bus.hpp"

//
// This simply sets some random values in memory
// By this point, ram is loaded into memory
//
void test1(RAM *ram) {
    srand(time(NULL));
    uint32_t count = 10;

    uint32_t address1 = 0x0400;
    for (int i = address1; i<(address1 + count * 4); i+=4) {
        uint32_t num = 10 + (rand() % 11);
        ram->setMemory(i, num);
    }
    
    uint32_t address2 = 0x0800;
    for (int i = address2; i<(address2 + count * 4); i+=4) {
        uint32_t num = rand() % 11;
        ram->setMemory(i, num);
    }
}

//
// This function runs the CPU
// Right now, it connects the RAM directly to the CPU.
// This shouldn't happen. The RAM should be connected to the bus, and the
// bus should be connected to the CPU (so it's pretty much in between)
//
// See the CPU class for information on how to swap out the ram with the bus
//
void run0(std::string input, std::string memory, CPU *cpu, RAM *ram, Bus *bus, int start) {
    cpu->setBus(bus);
    
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
    ram->loadMemory(memory, 5500);
    
    Bus *bus = new Bus(ram);

    // Set some values in memory
    //test1(ram);
    
    // If we have multiple inputs, then thread
    int clock_cycles = 0;
    int i_count = 0;
    int ticks = 0;
    
    if (input2 != "") {
        // NOTE: Leave this commented out
        CPU *cpu0 = new CPU;
        CPU *cpu1 = new CPU;
        
        cpu0->name = "CPU0";
        cpu1->name = "CPU1";
        
        //std::thread t1(run0, input, memory, cpu0, ram, bus, 0);
        //std::thread t2(run0, input2, memory, cpu1, ram, bus, 0x0100);
        //
        //t1.join();
        //t2.join();
        run0(input, memory, cpu0, ram, bus, 0);
        run0(input2, memory, cpu1, ram, bus, 0x0100);
        clock_cycles += cpu0->clock_cycles;
        i_count += cpu0->i_count;
        ticks += cpu0->getTicks();
        
        clock_cycles += cpu1->clock_cycles;
        i_count += cpu1->i_count;
        ticks += cpu1->getTicks();
    } else {
        CPU *cpu = new CPU;
        run0(input, memory, cpu, ram, bus, 0);
        
        clock_cycles = cpu->clock_cycles;
        i_count = cpu->i_count;
        ticks += cpu->getTicks();
    }
    
    // TODO: Print CPI here
    std::cout << "=====================================" << std::endl;
    std::cout << "Clock Cycles: " << clock_cycles << std::endl;
    std::cout << "Simulation Ticks: " << ticks << std::endl;
    std::cout << "Instruction Count: " << i_count << std::endl;
    std::cout << "CPI = Clock Cycles / Instruction Count = " << (float)clock_cycles / i_count << std::endl;
    std::cout << "=====================================" << std::endl;
    
    return 0;
}

