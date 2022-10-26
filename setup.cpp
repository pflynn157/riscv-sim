#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstring>

int main(int argc, char **argv) {
    FILE *file;
    file = fopen("memory.bin", "rb");
    
    size_t mem_size = 5500;
    
    // First, read the whole thing to our memory
    uint8_t *memory = new uint8_t[mem_size];
    
    uint32_t index = 0;
    while (!feof(file)) {
        memory[index] = fgetc(file);
        ++index;
    }
    
    // Now, start the verification
    uint32_t array_a = 0x0400;
    uint32_t array_b = 0x0800;
    uint32_t array_c = 0x0C00;
    uint32_t size = 256;
    
    // Load each array
    for (uint32_t i = array_a; i<(array_a + size * 4); i+=4) {
        float num = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100.0));
        memcpy(&memory[i], &num, sizeof(uint32_t));
    }
    
    for (uint32_t i = array_b; i<(array_b + size * 4); i+=4) {
        float num = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100.0));
        memcpy(&memory[i], &num, sizeof(uint32_t));
    }
    
    // Flush it back to the disk
    fclose(file);
    
    file = fopen("memory.bin", "wb");
    for (int i = 0; i<mem_size; i++) {
        fwrite(&memory[i], sizeof(uint8_t), 1, file);
    }
    fclose(file);
    
    // Cleanup and close
    //fclose(file);
    
    return 0;
}

