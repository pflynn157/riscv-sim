#include <iostream>
#include <cmath>
#include <cstring>

#include "cache.hpp"

// Memory size = 256 words
// Block = 32 words
// Number of blocks: 8

// Block count = 256 / 32
Cache::Cache(int size, int line_size, int assoc) {
    int offset = std::log2(line_size);
    int line_count = size / line_size;
    int sets = line_count / assoc;
    int set_addr_width = log2(sets);
    int tag_width = 32 - set_addr_width - offset;
    
    this->tag_width = tag_width;
    this->offset_width = offset;
    this->set_width = set_addr_width;
    this->line_size = line_size;
    
    std::cout << "Offset Width: " << this->offset_width << std::endl;
    std::cout << "Set Width: " << this->set_width << std::endl;
    std::cout << "Tag Width: " << this->tag_width << std::endl;
    std::cout << "---------------------------------" << std::endl;
    
    // Setup the cache
    blocks = size / line_size;
    cache = new CacheEntry*[blocks];
    for (int i = 0; i<blocks; i++) {
        CacheEntry *entry = new CacheEntry;
        entry->data = new uint8_t[line_size];
        entry->valid = false;
        entry->time = 0;
        entry->tag = 0;
        cache[i] = entry;
    }
}

void Cache::print() {
    for (int i = 0; i<blocks; i++) {
        CacheEntry *entry = cache[i];
        
        printf("[%d][?%d][", i, entry->valid);
        
        for (int j = 0; j<line_size; j++) {
            printf("%X", entry->data[j]);
            if (j + 1 < line_size) printf("|");
        }
        
        printf("][T:%X]\n", entry->tag);
    }
}

//
// Pulls data from the cache, and if necessary, from the cache
//
uint32_t Cache::getData(int clock_cycle, uint32_t address, int size) {
    // Generate the masks
    int offset_mask = 0;
    for (int i = 0; i<offset_width; i++) offset_mask |= (1 << i);
    
    int set_mask = 0;
    for (int i = 0; i<set_width; i++) set_mask |= (1 << i);
    
    int tag_mask = 0;
    for (int i = 0; i<tag_width; i++) tag_mask |= (1 << i);
    
    int offset = (address) & offset_mask;
    int block = (address >> offset_width) & set_mask;
    int tag = (address >> (set_width + offset_width)) & tag_mask;
    
    printf("[GET] Tag: %x | Set: %x | Offset: %x\n", tag, block, offset);
    
    // Check the cache
    CacheEntry *entry = cache[block];
    entry->time = clock_cycle;
    
    // Item in cache
    if (entry->valid && entry->tag == tag) {
        puts("[GET] Hit");
        uint32_t data = 0;
        memcpy(&data, &entry->data[offset], sizeof(uint8_t)*size);
        return data;
    
    // Item not in cache
    } else {
        puts("[GET] Miss");
        
        // Get data from RAM
        uint32_t data = bus->getMemory(address);
        for (int i = 0; i<line_size; i+=4) {
            uint32_t data = bus->getMemory(address+i);
            memcpy(&data, &entry->data[i], sizeof(uint32_t));
        }
        
        // Set it in the cache
        //setData(clock_cycle, address, data, size);
        
        // Return the fetched data
        return data;
    }
    
    return 0;
}

//
// Sets the data
//
bool Cache::setData(int clock_cycle, uint32_t address, uint32_t data, int size) {
    // Generate the masks
    int offset_mask = 0;
    for (int i = 0; i<offset_width; i++) offset_mask |= (1 << i);
    
    int set_mask = 0;
    for (int i = 0; i<set_width; i++) set_mask |= (1 << i);
    
    int tag_mask = 0;
    for (int i = 0; i<tag_width; i++) tag_mask |= (1 << i);
    
    int offset = (address) & offset_mask;
    int block = (address >> offset_width) & set_mask;
    int tag = (address >> (set_width + offset_width)) & tag_mask;
    
    puts("\n------------------------------------------");
    printf("[SET] Tag: %x | Set: %x | Offset: %x\n", tag, block, offset);
    
    // Check the cache
    CacheEntry *entry = cache[block];
    entry->time = clock_cycle;
    
    // Item in cache
    if (entry->valid && entry->tag == tag) {
        puts("[SET] Hit");
        memcpy(&entry->data[offset], &data, sizeof(uint8_t)*size);
    
    // Item not in cache
    } else {
        puts("[SET] Miss");
        entry->valid = true;
        entry->tag = tag;
        memcpy(&entry->data[offset], &data, sizeof(uint8_t)*size);
    }
    
    // Set RAM
    bus->setMemory(address, data);
    
    return false;
}


