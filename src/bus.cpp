#include <ctime>
#include <cstdlib>

#include "bus.hpp"
#include "memory.hpp"

//
// The constructor
//
Bus::Bus(RAM *ram) {
    srand(time(NULL));
    this->ram = ram;
}

//
// Adds the cache to the cluster
//
void Bus::addCache(Cache *cache) {
    while (true) {
        cache->id = rand() % 1001;
        
        bool found = false;
        for (Cache *c : cache_cluster) {
            if (c->id == cache->id) {
                found = true;
                break;
            }
        }
        
        if (!found) break;
    }
    
    cache_cluster.push_back(cache);
}

//
// General notes:
// I included a very simple program for you- see "program.asm" in the root directory.
// Basically, it loads "20" from x10 to memory, and puts "10" in x10, then loads memory to "x11".
// So you'll know if everything works if x10 has 10 and x11 has 20 after you run the simulator.
//
// There's two files in the root, "test.bin" and "memory.bin". Plug those into the simulator to run:
// src/cpu test.bin memory.bin
//
// Below are some initial thoughts on how to implement this. I haven't test it, but it's roughly what
// I'm thinking. Of course, if you have a better idea on how to do it, by all means go for it!
// 
// You'll be making changes here, to "main.cpp", and a few minor changes to "cpu.hpp" and "cpu.cpp"
// The two in "cpu.hpp/.cpp" are very minor.
//

//
// The two setMemory functions work the same way:
// 1) For actually setting the RAM, you don't have to do anything.
//    Simply call ram->setMemory(address, item). Same with getMemory
// 2) However, we need to make sure that the CPU isn't trying to read
//    or write from the same locations at memory at the same time.
// Here's how you do it:
//   * Create a class variable (in bus.hpp, in the Bus class under private) called
//     "readAddress" and "writeAddress" (or something like that). Set them to 0 initially.
//     In setMemory and getMemory, check to see if either of those variables equal the address
//     you want to write to. If so, create a busy loop that runs until they are not equal to
//    the address. After that, you can call ram->setMemory or whatever.
//
// So roughly, it would be like this:
// if readAddress == address -> while (readAddress == address) {}
// readAddress = address;
// ram->setMemory
// address = 0
//
//
// GetMemory will work the same way
//
void Bus::setMemory(uint16_t address, uint8_t item) {
    while(readAddress == address) { stalls += 1; }

    readAddress = address;
    ram->setMemory(address, item);

    readAddress = -1;    

}

void Bus::setMemory(uint32_t address, uint32_t item) {
    while(readAddress == address) { stalls += 1; }

    readAddress = address;
    ram->setMemory(address, item);

    readAddress = -1;

}

uint32_t Bus::getMemory(uint32_t address) {
    while(writeAddress == address) { stalls += 1; }

    writeAddress = address;
    uint32_t data = ram->getMemory(address);

    writeAddress = -1;

    return data;
}

