#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <math.h>

#include "cache.hpp"

int main(int argc, char **argv) {
    std::vector<uint32_t> addresses;
    int hits = 0;
    int misses = 0;
    
    int size = 512;
    int line_size = 32;
    
    Cache *cache1 = new Cache(size, line_size);
    cache1->print();
    puts("====================================================");
    
    // Test 1
    cache1->setData(1, 0x0010, 20, 4);
    //cache1->print();
    
    cache1->setData(2, 0x0010, 5, 4);
    //cache1->print();
    
    cache1->setData(3, 0x0127, 1, 4);
    
    cache1->setData(4, 0x0439, 2, 4);
    
    cache1->setData(5, 0x0015, 2, 4);
    //cache1->print();
    
    uint32_t data = cache1->getData(6, 0x02799, 4);
    printf("Get: %d\n", data);
    
    data = cache1->getData(2, 0x0439, 4);
    printf("Get: %d\n", data);
    
    cache1->print();
    
    // tmp
    /*addresses.push_back(0x0010);
    addresses.push_back(0x0127);
    addresses.push_back(0x0439);
    addresses.push_back(0x0010);*/
    
    // Read the address file
    /*std::ifstream reader("addresses.txt");
    if (reader.is_open()) {
        std::string line = "";
        while (std::getline(reader, line)) {
            std::istringstream conv(line);
            uint32_t value = 0;
            conv >> std::hex >> value;
            addresses.push_back(value);
        }
    }*/
    
    // Create the cache and test
    /*Cache *cache1 = new Cache(size, line_size);
    for (int i = 0; i<addresses.size(); i++) {
        if (cache1->setData(addresses.at(i), 1)) {
            ++hits;
        } else {
            ++misses;
        }
    }
    
    std::cout << "Hits: " << hits << std::endl;
    std::cout << "Misses: " << misses << std::endl;*/
    
    return 0;
}

