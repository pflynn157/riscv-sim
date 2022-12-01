#include <cmath>
#include <cstdio>
#include <string>

#include "directory.hpp"

// Memory size = 256 words
// Block = 32 words
// Number of blocks: 8

// Block count = 256 / 32
Directory::Directory(int size, int line_size, int assoc) {
    int offset = std::log2(line_size);
    int line_count = size / line_size;
    int sets = line_count / assoc;
    int set_addr_width = log2(sets);
    int tag_width = 32 - set_addr_width - offset;
    
    this->tag_width = tag_width;
    this->offset_width = offset;
    this->set_width = set_addr_width;
    this->line_size = line_size;
    
    // Setup the cache
    blocks = size / line_size;
    directory = new DirEntry*[blocks];
    for (int i = 0; i<blocks; i++) {
        DirEntry *entry = new DirEntry;
        entry->tag = 0;
        entry->lines = 0;
        entry->state = State::Uncached;
        entry->dirty = 0;
        directory[i] = entry;
    }
}

void Directory::print() {
    for (int i = 0; i<blocks; i++) {
        DirEntry *dir = directory[i];
        
        printf("[Lines: ");
        std::string ln = "";
        for (int i = 0; i<8; i++) {
            uint8_t bit = (dir->lines >> i) & 0x01;
            ln += std::to_string(bit);
        }
        printf("%s", ln.c_str());
        
        printf("][Dirty: ");
        std::string dln = "";
        for (int i = 0; i<8; i++) {
            uint8_t bit = (dir->dirty >> i) & 0x01;
            dln += std::to_string(bit);
        }
        printf("%s", dln.c_str());
        
        printf("][Tag: %d][State: ", dir->tag);
        switch (dir->state) {
            case State::Exclusive: printf("E"); break;
            case State::Shared: printf("S"); break;
            case State::Uncached: printf("U"); break;
            default: {}
        }
        printf("]\n");
    }
}

