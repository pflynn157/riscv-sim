#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "cache.hpp"

int main(int argc, char **argv) {
    std::vector<uint32_t> addresses;
    int hits = 0;
    int misses = 0;
    
    // tmp
    //addresses.push_back(0x0010);
    //addresses.push_back(0x0127);
    //addresses.push_back(0x0439);
    //addresses.push_back(0x0010);
    
    // Read the address file
    std::ifstream reader("addresses.txt");
    if (reader.is_open()) {
        std::string line = "";
        while (std::getline(reader, line)) {
            std::istringstream conv(line);
            uint32_t value = 0;
            conv >> std::hex >> value;
            addresses.push_back(value);
        }
    }
    
    // Create the cache and test
    Cache *cache1 = new Cache(256, 32);
    for (int i = 0; i<addresses.size(); i++) {
        if (cache1->setData(addresses.at(i), 1)) {
            ++hits;
        } else {
            ++misses;
        }
    }
    
    std::cout << "Hits: " << hits << std::endl;
    std::cout << "Misses: " << misses << std::endl;

    return 0;
}

