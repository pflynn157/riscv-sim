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

void Cache::setDirectory(Directory *dir) {
    this->dir = dir;
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
        
        printf("][T:%X]\n", entry->tag);
    }
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
    printf("[INVALIDATE] Cache %d\n", id);
    
    // Check the cache
    CacheEntry *entry = cache[block];
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
    
    // Check the cache
    CacheEntry *entry = cache[block];
    
    //printf("[GET] Tag: %x | Set: %x | Offset: %x | Valid: %d\n", tag, block, offset, entry->valid);
    
    // Item in cache
    if (entry->valid && entry->tag == tag) {
        printf("[GET %d] Hit\n", id);
        
        uint32_t data = 0;
        memcpy(&data, &entry->data[offset], sizeof(uint8_t)*size);
        return data;
    
    // Item not in cache
    } else {
        entry->valid = true;
        entry->tag = tag;
        
        //
        // Clear the cache line
        //
        for (int i = 0; i<line_size; i+=4) {
            uint32_t data2 = 0;
            memcpy(&entry->data[i], &data2, sizeof(uint32_t));
        }
        
        // Check the directory
        int pos = dir->checkDirectory(address, id);
        if (pos == -1) {
            //
            // This is fake, but pretend we're loading at a particular offset
            //
            uint32_t data = 0x0F0F0F0F;
            memcpy(&entry->data[offset], &data, sizeof(uint32_t));
            
            // Set the directory
            printf("[GET %d][UNCACHED -> EXCLUSIVE]\t Miss -> Fetch from memory\n", id);
            dir->setLine(address, id);
        } else {
            printf("[GET %d][EXCLUSIVE -> SHARED]\t Miss -> Fetch from shared cache @ %d\n", id, (pos + 1));
            dir->setLine(address, id, true);
        }
        
        // Return the fetched data
        return 0;
    }
    
    return 0;
}

//
// Gets and returns an entire cache line
//
std::vector<uint32_t> Cache::getLineData(uint32_t address) {
    std::vector<uint32_t> line;
    
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
    
    // Check the cache
    CacheEntry *entry = cache[block];
    
    // Item in cache
    if (entry->valid && entry->tag == tag) {
        for (int i = 0; i<line_size; i+=4) {
            uint32_t data2 = 0;
            memcpy(&data2, &entry->data[i], sizeof(uint32_t));
            line.push_back(data2);
        }
    }
    
    return line;
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
        printf("[SET %d] Hit\n", id);
        
        // Update the directory
        dir->setLine(address, id, false, true);
    
    // Item not in cache
    } else {
        // Check the other caches
        entry->valid = true;
        entry->tag = tag;
        
        // Check the rest of the bus
        int pos = dir->checkDirectory(address, id);
        if (pos == -1) {
            //
            // Pull from RAM
            //
            printf("[SET %d][UNCACHED -> EXCLUSIVE/MODIFIED]\t Miss -> Pull line from RAM\n", id);
            dir->setLine(address, id, false, true);
            
            for (int i = 0; i<line_size; i+=4) {
                uint32_t data2 = 0x0F0F0F0F;
                memcpy(&entry->data[i], &data2, sizeof(uint32_t));
            }
        } else {
            //
            // Pull from shared cache
            //
            printf("[SET %d][SHARED -> EXCLUSIVE/MODIFIED]\t Miss -> Pull from shared cache @ %d\n", id, (pos+1));
            dir->setLine(address, id, false, true);
            
        }
        
        memcpy(&entry->data[offset], &data, sizeof(uint8_t)*size);
    }
    
    return false;
}


