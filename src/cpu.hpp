#pragma once

#include <cstdint>
#include <iostream>

#include "memory.hpp"
#include "bus.hpp"

using namespace std;

//
// Holds decoded data
// TODO: Eventually merge with struct
//
struct Data {
    uint8_t opcode = 0;
    uint8_t rd = 0;
    uint8_t rs1 = 0;
    uint8_t rs2 = 0;
    uint8_t func3 = 0;
    uint8_t func7 = 0;
    uint32_t imm_i = 0;
    uint32_t imm_b = 0;
    uint32_t imm_s = 0;
    uint32_t imm_u = 0;
    uint32_t imm_j = 0;
    
    // Control lines
    int branch = 0;
    int mem_read = 0;
    int mem2reg = 0;
    int aluop = 0;
    int mem_write = 0;
    int alu_src = 0;
    int reg_write = 0;
    int float_reg_write = 0;
    int fop = 0;
    int rs1_src = 0;
    int pc_write = 0;
    int alu_invert = 0;
    int addui = 0;          // add upper immediate
    
    // The final, decoded immediate
    uint32_t imm = 0;
};

//
// Holds each state of the CPU
//
struct State {
    uint32_t instr = 0;
    Data *decodeData;
    int stall = 0;
    
    // Memory control lines
    bool read = false;
    bool write = false;
    bool write_float = false;
    uint32_t address = 0;
    uint32_t write_data = 0;
    int rd = 0;
};

//
// Represents the CPU
//
struct CPU {
    int *registers;
    float *float_registers;
    uint32_t pc = 0;
    uint32_t clock_cycles = 0;
    uint32_t i_count = 0;
    bool halt = false;
    
    //
    // Sets up the CPU
    //
    CPU() {
        registers = new int[32];
        float_registers = new float[32];
        for (int i = 0; i<32; i++) {
            registers[i] = 0;
            float_registers[i] = 0;
        }
        
        // set x0
        registers[0] = 0;
        float_registers[0] = 0;
    }
    
    //
    // The main run loop of the CPU
    //
    void run();
    
    //
    // Functions for each state of the CPU
    //
    void fetch();
    void decode(State *state);
    void execute(State *state);
    void executeFPU(State *state);
    void store(State *state);
    
    //
    // Set and get the RAM
    //
    void setBus(Bus *ram) {
        this->ram = ram;
    }
    
    //
    // Sets a register
    //
    void setRegister(int num, int value) {
        if (num == 0) return;
        registers[num] = value;
    }
    
    void setFloatRegister(int num, float value) {
        if (num == 0) return;
        float_registers[num] = value;
    }
    
    //
    // Returns a value from the register
    //
    int getRegister(int num) {
        if (num == 0) return 0;
        return registers[num];
    }
    
    float getFloatRegister(int num) {
        if (num == 0) return 0.0f;
        return float_registers[num];
    }
    
    //
    // Debug function for registers
    //
    void debugRegisters() {
        for (int i = 0; i<32; i++) {
            cout << "x" << i << ": " << registers[i] << " | ";
        }
        cout << endl;
        std::cout << "===================================" << std::endl;
        for (int i = 0; i<32; i++) {
            cout << "f" << i << ": " << (float)float_registers[i] << " | ";
        }
        cout << endl;
    }
    
private:
    // This is currently how we set RAM- with a direct connection to memory via the RAM object
    // Remove this and replace with the bus. Then, go through "cpu.cpp" in the writeback stage,
    // and replace the "ram" references with "bus". The functions should be the same.
    //
    Bus *ram;
    
    void decodeSet(Data *data);
    uint32_t executeALU(Data *data, uint32_t src1, uint32_t src2);
    uint32_t executeBRU(Data *data, uint32_t src1, uint32_t src2);
    
    State *decodeState = nullptr;
    State *executeState = nullptr;
    State *storeState = nullptr;
};

