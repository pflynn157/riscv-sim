#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstring>

int main(int argc, char **argv) {
    FILE *file;
    file = fopen("memory.bin", "rb");
    
    // First, read the whole thing to our memory
    uint8_t *memory = new uint8_t[5500];
    
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
    
    // Create the arrays
    float *a = new float[size];
    float *b = new float[size];
    float *c = new float[size];
    
    // Load each array
    index = 0;
    for (uint32_t i = array_a; i<(array_a + size * 4); i+=4) {
        float num = 0;
        memcpy(&num, &memory[i], sizeof(uint32_t));
        a[index] = num;
        ++index;
    }
    
    index = 0;
    for (uint32_t i = array_b; i<(array_b + size * 4); i+=4) {
        float num = 0;
        memcpy(&num, &memory[i], sizeof(uint32_t));
        b[index] = num;
        ++index;
    }
    
    index = 0;
    for (uint32_t i = array_c; i<(array_c + size * 4); i+=4) {
        float num = 0;
        memcpy(&num, &memory[i], sizeof(uint32_t));
        c[index] = num;
        ++index;
    }
    
    // Now, verify
    for (int i = 0; i<size; i++) {
        std::cout << "A: " << a[i] << " B: " << b[i] << " = C: " << c[i] << " ";
        std::cout << "(Expected: " << a[i] + b[i] << ")";
        
        if ((a[i] + b[i]) != c[i]) std::cout << " \t\t[FAIL]" << std::endl;
        else std::cout << " \t\t[PASS]" << std::endl;
    }
    
    // Cleanup and close
    /*delete[] a;
    delete[] b;
    delete[] c;*/
    //fclose(file);
    
    return 0;
}

