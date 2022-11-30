#include <iostream>

#include "bus.hpp"
#include "cache.hpp"

int main(int argc, char **argv) {
    Bus *bus = new Bus;
    Cache *c1 = new Cache(256, 32);
    Cache *c2 = new Cache(256, 32);
    Cache *c3 = new Cache(256, 32);
    Cache *c4 = new Cache(256, 32);
    c1->setBus(bus);
    c2->setBus(bus);
    c3->setBus(bus);
    c4->setBus(bus);

    return 0;
}

