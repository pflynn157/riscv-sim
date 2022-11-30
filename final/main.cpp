#include <iostream>

#include "bus.hpp"
#include "cache.hpp"

int main(int argc, char **argv) {
    Bus *bus = new Bus;
    Cache *c1 = new Cache(256, 32);
    Cache *c2 = new Cache(256, 32);
    Cache *c3 = new Cache(256, 32);
    Cache *c4 = new Cache(256, 32);
    c1->setBus(bus);
    c2->setBus(bus);
    c3->setBus(bus);
    c4->setBus(bus);
    std::cout << "=======================================" << std::endl;
    
    // Test 1
    // Read: Read -> Invalid -> Exclusive
    // Host: Cache 1
    
    
    // Final print
    std::cout << "=C1=========================" << std::endl;
    c1->print();
    std::cout << std::endl;
    std::cout << "=C2=========================" << std::endl;
    c2->print();
    std::cout << std::endl;
    std::cout << "=C3=========================" << std::endl;
    c3->print();
    std::cout << std::endl;
    std::cout << "=C4=========================" << std::endl;
    c4->print();
    std::cout << std::endl;

    return 0;
}

