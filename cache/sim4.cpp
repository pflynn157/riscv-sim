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
    
    int size = 256;
    int line_size = 32;
    int assoc = 4;
    int lookUpT = ceil(log2(size/line_size))*assoc;
    int accessT = 0;
    
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
    Cache *cache1 = new Cache(size, line_size, assoc);
    for (int i = 0; i<addresses.size(); i++) {
        if (cache1->setData(addresses.at(i), 1)) {
            ++hits;
            accessT += 2;
        } else {
            ++misses;
        }
    }
    
    double hitRate = (double)hits/((double)hits+(double)misses)*100;
    double missRate = (double)misses/((double)hits+(double)misses)*100;
    int cost = misses*(100+lookUpT);
    std::cout << "Hits: " << hits << std::endl;
    std::cout << "Misses: " << misses << std::endl;
    std::cout << "Hit Rate = " << hitRate << "%" << std::endl;
    std::cout << "Miss Rate = " << missRate << "%" << std::endl;
    std::cout << "Cost: " << cost << std::endl;
    std::cout << "Access Time: " << accessT + cost << std::endl;
    std::cout << "Ticks: " << cache1->ticks << std::endl;

    return 0;
}

