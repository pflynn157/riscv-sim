#include <iostream>

#include "directory.hpp"
#include "cache.hpp"

int main(int argc, char **argv) {
    Directory *dir = new Directory(256, 32);
    Cache *c1 = new Cache(256, 32);
    Cache *c2 = new Cache(256, 32);
    Cache *c3 = new Cache(256, 32);
    Cache *c4 = new Cache(256, 32);
    c1->id = 1;
    c2->id = 2;
    c3->id = 3;
    c4->id = 4;
    std::cout << "=======================================" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    
    // Test 1
    // Read: Read -> Invalid -> Exclusive
    // Host: Cache 1
    c1->getData(0x804, 4);
    //c3->getData(0x964, 4);
    
    // Read: Read -> Invalid -> Shared
    //c2->getData(0x804, 4);
    //c3->getData(0x804, 4);
    
    //puts("");
    
    //
    // Write -> Invalid -> Modified
    //c1->setData(0x932, 10, 4);
    
    // Write -> Invalid -> Modified
    //c2->setData(0x932, 12, 4);
    
    // Write -> Exclusive -> Modified
    //c3->setData(0x964, 10, 4);
    
    // Write -> Shared -> Modified
    //c3->setData(0x804, 0x0A0A, 4);
    
    // Write -> Modified -> Modified
    //c3->setData(0x804, 0x0E0E, 4);
    
    // Read -> Modified
    //c3->getData(0x804, 4);
    
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

