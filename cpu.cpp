#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>

using namespace std;

//
// Holds decoded data
//
struct Data {
    uint8_t opcode = 0;
    uint8_t rd = 0;
    uint8_t rs1 = 0;
    uint8_t rs2 = 0;
    uint8_t func3 = 0;
    uint8_t func7 = 0;
    uint32_t imm = 0;
};

//
// Represents the CPU
//
struct CPU {
    int *registers;
    uint8_t *memory;
    int pc = 0;
    
    //
    // Sets up the CPU
    //
    CPU() {
        registers = new int[32];
        memory = new uint8_t[1024];
        
        // set x0
        registers[0] = 0;
    }
    
    //
    // Sets a register
    //
    void setRegister(int num, int value) {
        if (num == 0) return;
        registers[num] = value;
    }
    
    //
    // Returns a value from the register
    //
    int getRegister(int num) {
        return registers[num];
    }
    
    //
    // Debug function for registers
    //
    void debugRegisters() {
        for (int i = 0; i<32; i++) {
            cout << "x" << i << ": " << registers[i] << " | ";
        }
        cout << endl;
    }
};

Data *decode(uint32_t instr){
    Data *data = new Data;
    
    // opcode
    // 6 - 0
    data->opcode = (uint8_t)(instr & 0b00000000000000000000000000111111);
    
    // rd
    // 11 - 7
    data->rd = (uint8_t)((instr & 0b00000000000000000000011111000000) >> 7);
    
    // funct3
    // 14 - 12
    data->func3 = (uint8_t)((instr & 0b00000000000000000011100000000000) >> 12);
    
    // rs1
    // 19 - 15
    data->rs1 = (uint8_t)((instr & 0b00000000000001111100000000000000) >> 15);
    
    // rs2
    // 24 - 20
    data->rs2 = (uint8_t)((instr & 0b0000000111110000000000000000000) >> 20);
    
    // func7
    // 31 - 25
    data->func7 = (uint8_t)((instr & 0b1111111000000000000000000000000) >> 25);
    
    // imm
    // 31 - 20
    data->imm = (uint32_t)((instr & 0b1111111111110000000000000000000) >> 20);
    
    // imm
    //string imm = "";
    //for (int i = 20; i<=31; i++) imm += instr[i];
    
    // long imm
    //string long_imm = "";
    //for (int i = 12; i<=31; i++) long_imm += instr[i];
    
    // Build the structure
    /*data->opcode = opcode;
    data->func3 = func3;
    data->func7 = func7;
    data->rd = stoi(rd);
    data->rs1 = stoi(rs1);
    data->rs2 = stoi(rs2);
    data->imm = stoul(imm, nullptr, 2);
    data->long_imm = stoul(imm, nullptr, 2);*/
    
    return data;
}

//
// Handles all ALU operations
//
void alu(Data *data, CPU *cpu, bool isImmediate) {
    uint32_t val1 = cpu->getRegister(data->rs1);
    uint32_t val2 = 0;
    if (isImmediate) val2 = data->imm;
    else val2 = cpu->getRegister(data->rs2);

    // Add and sub (sub determined by func7)
    if (data->func3 == 0) {
        int result = val1 + val2;
        cpu->setRegister(data->rd, result);
        
    // Shift left
    } else if (data->func3 == 1) {
    
    // SLT (set less than)
    } else if (data->func3 == 2) {
    
    // SLTU (set less than unsigned)
    } else if (data->func3 == 3) {
    
    // XOR
    } else if (data->func3 == 4) {
    
    // SRL/SRA (shift right logic/arithmetic)
    // Logic vs arithmetic determined by func7
    } else if (data->func3 == 5) {
    
    // OR
    } else if (data->func3 == 6) {
    
    // AND
    } else if (data->func3 == 7) {
    
    }
}

//
// Handles all branching operations
//
void branch(Data *data, CPU *cpu) {
    // BEQ (000)
    if (data->func3 == 0) {
    
    // BNE (001)
    } else if (data->func3 == 1) {
    
    // BLT (100)
    } else if (data->func3 == 4) {
    
    // BGE (101)
    } else if (data->func3 == 5) {
    
    // BLTU (110)
    } else if (data->func3 == 6) {
    
    // BGEU (111)
    } else if (data->func3 == 7) {
    
    }
}

void execute(Data *data, CPU *cpu) {
    
    //
    // First, determine the opcode
    //
    // ALU instruction, R-Type and I-Type
    // R-Type: 0010011 | I-Type: 0110011
    if (data->opcode == 0x13 || data->opcode == 0x33) {
        if (data->opcode == 0x33) {
            alu(data, cpu, false);
        } else {
            alu(data, cpu, true);
        }
    
    // LUI (load upper immediate), AUIPC, JAL
    // OP: 0110111
    } else if (data->opcode == 0x37) {
    
    // All the branch instructions
    // OP: 1100011
    } else if (data->opcode == 0x63) {
        branch(data, cpu);
    
    // All the load instructions
    // LD: 0000011
    } else if (data->opcode == 0x03) {
    
        
    // All the store instructions
    // ST: 0100011
    } else if (data->opcode == 0x23) {
    
    
    // Otherwise, invalid opcode
    } else {
        cout << "Error: Invalid Opcode." << std::endl;
        return;
    }
}

int main() {
    // Instructions
    // ADD x1, X0, X0
    // 0000000 00000 00000 000 00001 0110011
    // LE: 0000 0000 0000 0000 0000 0000 1011 0011
    uint32_t instr1 = 0x000000B3;
    
    // ADDI X2, X0, 10
    // 000000001010 00000 000 00010 0010011
    // LE: 0000 0000 1010 0000 0000 0001 0001 0011
    uint32_t instr2 = 0x00A00113;
    
    Data *d1 = decode(instr1);
    printf("OP: %X | func3: %X | func7: %X |", d1->opcode, d1->func3, d1->func7);
    printf(" rd: %d | rs1: %d | rs2: %d | imm: %d\n", d1->rd, d1->rs1, d1->rs2, d1->imm);
    
    Data *d2 = decode(instr2);
    printf("OP: %X | func3: %X | func7: %X |", d2->opcode, d2->func3, d2->func7);
    printf(" rd: %d | rs1: %d | rs2: %d | imm: %d\n", d2->rd, d2->rs1, d2->rs2, d2->imm);
    
    // Execute and test
    CPU *cpu = new CPU;
    execute(d1, cpu);
    execute(d2, cpu);
    
    cpu->debugRegisters();
    
    return 0;
}

