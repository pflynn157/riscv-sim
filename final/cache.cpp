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
        entry->tag = 0;
        cache[i] = entry;
    }
}

void Cache::setBus(Bus *bus) {
    this->bus = bus;
    
    // Adds the cache to the cache cluster on the bus
    bus->addCache(this);
}

void Cache::print() {
    printf("ID: %d\n", id);
    
    for (int i = 0; i<blocks; i++) {
        CacheEntry *entry = cache[i];
        
        printf("[%d][?%d][", i, entry->valid);
        
        for (int j = 0; j<line_size; j++) {
            printf("%X", entry->data[j]);
            if (j + 1 < line_size) printf("|");
        }
        
        printf("][T:%X][", entry->tag);
        switch (entry->state) {
            case CacheState::Invalid: printf("INVALID"); break;
            case CacheState::Exclusive: printf("EXCLUSIVE"); break;
            case CacheState::Shared: printf("SHARED"); break;
            case CacheState::Modified: printf("MODIFIED"); break;
            default: {}
        }
        printf("]\n");
    }
}

//
// Checks to see if the cache has data at a particular location
//
bool Cache::hasAddress(uint32_t address) {
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
    
    //printf("[GET] Tag: %x | Set: %x | Offset: %x\n", tag, block, offset);
    
    // Check the cache
    CacheEntry *entry = cache[block];
    
    // Item in cache
    if (entry->valid && entry->tag == tag) {
        return true;
    }
    
    return false;
}

//
// Invalidates the cache at a particular address
//
void Cache::invalidate(uint32_t address) {
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
    
    //printf("[INVALIDATE] Tag: %x | Set: %x | Offset: %x\n", tag, block, offset);
    
    // Check the cache
    CacheEntry *entry = cache[block];
    entry->state = CacheState::Invalid;
    entry->valid = false;
}

//
// Pulls data from the cache, and if necessary, from the cache
//
uint32_t Cache::getData(uint32_t address, int size) {
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
    
    //printf("[GET] Tag: %x | Set: %x | Offset: %x\n", tag, block, offset);
    
    // Check the cache
    CacheEntry *entry = cache[block];
    
    // Item in cache
    if (entry->valid && entry->tag == tag) {
        puts("[GET] Hit");
        
        uint32_t data = 0;
        memcpy(&data, &entry->data[offset], sizeof(uint8_t)*size);
        return data;
    
    // Item not in cache
    } else {
        puts("[GET] Miss");
        
        // Check the cache cluster
        int pos = bus->checkCacheCluster(address, id);
        if (pos == -1) {
            puts("[GET] Fetch from memory");
        } else {
            puts("[GET] Fetch from shared cache");
        }
        
        // Get data from RAM
        //uint32_t data = bus->getMemory(address);
        uint32_t data = 0xFFFFFFFF;
        for (int i = 0; i<line_size; i+=4) {
            //uint32_t data2 = bus->getMemory(address+i);
            uint32_t data2 = 0xAAAAAAAA;
            memcpy(&data2, &entry->data[i], sizeof(uint32_t));
        }
        
        // Return the fetched data
        return data;
    }
    
    return 0;
}

//
// Sets the data
//
bool Cache::setData(uint32_t address, uint32_t data, int size) {
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
    
    //puts("\n------------------------------------------");
    //printf("[SET] Tag: %x | Set: %x | Offset: %x\n", tag, block, offset);
    
    // Check the cache
    CacheEntry *entry = cache[block];
    
    // Item in cache
    if (entry->valid && entry->tag == tag) {
        puts("[SET] Hit");
        memcpy(&entry->data[offset], &data, sizeof(uint8_t)*size);
    
    // Item not in cache
    } else {
        // Check the other caches
        entry->state = CacheState::Modified;
        
        // Check the rest of the bus
        int pos = bus->checkCacheCluster(address, id);
        if (pos >= 0) {
            // Miss- pull from another cache and invalid their caches
            printf("[SET] Miss- Shared Caches- @ %d\n", pos);
            bus->invalidateCaches(address, id);
        } else {
            // Miss- fetch from main memory
            puts("[SET] Miss- Fetch From Memory");
        }
        
        entry->valid = true;
        entry->tag = tag;
        memcpy(&entry->data[offset], &data, sizeof(uint8_t)*size);
    }
    
    // Set RAM
    //bus->setMemory(address, data);
    
    return false;
}


