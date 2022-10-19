#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>

// Note: default to 4096

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cerr << "Error: Please specify the size of the memory." << std::endl;
        return 1;
    }
    
    int size = std::stoi(argv[1]);
    FILE *file = fopen("memory.bin", "wb");
    
    uint8_t element = 0;
    for (int i = 0; i<size; i++) {
        fwrite(&element, sizeof(uint8_t), 1, file);
    }
    
    fclose(file);
    
    return 0;
}

