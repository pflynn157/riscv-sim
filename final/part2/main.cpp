#include <iostream>

#include "directory.hpp"
#include "cache.hpp"

int main(int argc, char **argv) {
    Directory *dir = new Directory(256, 32);
    Cache *c1 = new Cache(256, 32);
    Cache *c2 = new Cache(256, 32);
    Cache *c3 = new Cache(256, 32);
    Cache *c4 = new Cache(256, 32);
    c1->setDirectory(dir);
    c2->setDirectory(dir);
    c3->setDirectory(dir);
    c4->setDirectory(dir);
    c1->id = 1;
    c2->id = 2;
    c3->id = 3;
    c4->id = 4;
    std::cout << "=======================================" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    
    // Read: Uncached -> Exclusive
    c1->getData(0x804, 4);
    
    // Read: Uncached -> Exclusive -> Shared
    c1->getData(0x964, 4);
    c2->getData(0x964, 4);
    
    // Read: Shared -> Shared
    c3->getData(0x964, 4);
    
    // Write: Uncached -> Exclusive
    c1->setData(0x432, 10, 4);
    
    // Write: Shared -> Exclusive
    c2->getData(0x348, 4);
    c4->getData(0x348, 4);
    c3->setData(0x348, 10, 4);
    
    // Write: Modified -> Modified
    c3->setData(0x348, 20, 4);
    c4->setData(0x348, 20, 4);
    
    // Read: Modified/Exclusive -> Modified/Exclusive -> Modified/Shared
    c4->setData(0x496, 20, 4);
    c4->getData(0x496, 4);
    c1->getData(0x496, 4);
    
    // Final print
    std::cout << std::endl;
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
    std::cout << "----------------------------" << std::endl;
    dir->print();
    std::cout << std::endl;

    return 0;
}

