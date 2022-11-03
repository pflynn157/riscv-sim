#include <iostream>

#include "cache.hpp"

void check(bool h1, bool h2, bool h3, bool h4) {
    if (h1) puts("[1] Hit");
    else puts("[1] Miss");
    
    if (h2) puts("[2] Hit");
    else puts("[2] Miss");
    
    if (h3) puts("[3] Hit");
    else puts("[3] Miss");
    
    if (h4) puts("[4] Hit");
    else puts("[4] Miss");
}

int main(int argc, char **argv) {
    DirectCache *cache1 = new DirectCache(256, 32);
    bool h1 = cache1->setData(0x010, 10);   // 0000 0000 | 000 | 1 0000
    bool h2 = cache1->setData(0x127, 20);   // 0000 0001 | 001 | 0 0111
    bool h3 = cache1->setData(0x439, 30);   // 0000 0100 | 001 | 1 1000
    bool h4 = cache1->setData(0x010, 20);   // 0000 0000 | 000 | 1 0000
    
    check(h1, h2, h3, h4);
    
    //
    puts("===================================");
    DirectCache *cache2 = new DirectCache(256, 32, 2);
    h1 = cache2->setData(0x010, 10);   // 0000 0000 | 000 | 1 0000
    h2 = cache2->setData(0x127, 20);   // 0000 0001 | 001 | 0 0111
    h3 = cache2->setData(0x439, 30);   // 0000 0100 | 001 | 1 1000
    h4 = cache2->setData(0x010, 20);   // 0000 0000 | 000 | 1 0000
    
    check(h1, h2, h3, h4);
    
    //
    puts("===================================");
    DirectCache *cache4 = new DirectCache(256, 32, 4);
    h1 = cache4->setData(0x010, 10);   // 0000 0000 | 000 | 1 0000
    h2 = cache4->setData(0x127, 20);   // 0000 0001 | 001 | 0 0111
    h3 = cache4->setData(0x439, 30);   // 0000 0100 | 001 | 1 1000
    h4 = cache4->setData(0x010, 20);   // 0000 0000 | 000 | 1 0000
    
    check (h1, h2, h3, h4);

    return 0;
}

