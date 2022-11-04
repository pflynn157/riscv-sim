#include <iostream>
#include <cmath>

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
    
    std::cout << "Offset: " << offset << std::endl;
    std::cout << "Line Count: " << line_count << std::endl;
    std::cout << "Sets: " << sets << std::endl;
    std::cout << "Set Addr Width: " << set_addr_width << std::endl;
    std::cout << "Tag Width: " << tag_width << std::endl;
    std::cout << "---------------------------------" << std::endl;
    
    this->tag_width = tag_width;
    this->offset_width = offset;
    this->set_width = set_addr_width;
    
    // Setup the cache
    blocks = size / line_size;
    cache = new CacheEntry*[blocks];
    for (int i = 0; i<blocks; i++) {
        CacheEntry *entry = new CacheEntry;
        entry->data = new uint8_t[32];
        entry->valid = false;
        entry->time = 0;
        entry->tag = 0;
        cache[i] = entry;
    }
}

bool Cache::containsAddress(uint32_t address) {
    
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
    
    //printf("Address: %x | Tag Width: %x | Set Width: %x | Offset Width: %x\n", address, tag_width, set_width, offset_width);
    //printf("Tag: %x | Set: %x | Offset: %x\n\n", tag, set, offset);
    
    CacheEntry *entry = cache[block];
    if (entry == nullptr) return false;
    if (entry->valid == false) return false;
    if (entry->tag == tag) return true;
    
    return false;
}

//
// To set data:
// 1) Calculate address
// 2) Determine if its in the cache
// 3) If not, set
// 4) If so, find the one with the lowest time
// 5) Increment our fake time variable
//
// Returns true if hit, returns false if miss
//
bool Cache::setData(uint32_t address, uint8_t data) {
    if (containsAddress(address)) {
        // TODO: Data would be set
        return true;
    }

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
    
    // Set the cache
    delete cache[block];
    CacheEntry *entry = new CacheEntry;
    entry->valid = true;
    entry->tag = tag;
    cache[block] = entry;
    
    return false;
}

