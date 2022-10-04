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
    data->func3 = (uint8_t)((instr & 0b000000000000000000111000000000000) >> 12);
    
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
        uint32_t result = val1 + val2;
        cpu->setRegister(data->rd, result);
        
    // Shift left
    } else if (data->func3 == 1) {
        uint32_t result = val1 >> val2;
        cpu->setRegister(data->rd, result);
    
    // SLT (set less than)
    } else if (data->func3 == 2) {
        uint32_t result = val1 + val2;
        cpu->setRegister(data->rd, result);
    
    // SLTU (set less than unsigned)
    } else if (data->func3 == 3) {
        uint32_t result = val1 + val2;
        cpu->setRegister(data->rd, result);
    
    // XOR
    } else if (data->func3 == 4) {
        uint32_t result = val1 ^ val2;
        cpu->setRegister(data->rd, result);
    
    // SRL/SRA (shift right logic/arithmetic)
    // Logic vs arithmetic determined by func7
    } else if (data->func3 == 5) {
        uint32_t result = val1 >> val2;
        cpu->setRegister(data->rd, result);
    
    // OR
    } else if (data->func3 == 6) {
        uint32_t result = val1 | val2;
        cpu->setRegister(data->rd, result);
    
    // AND
    } else if (data->func3 == 7) {
        uint32_t result = val1 & val2;
        cpu->setRegister(data->rd, result);
    
    }
}

//
// Handles all branching operations
//
void branch(Data *data, CPU *cpu) {
    uint32_t val1 = cpu->getRegister(data->rs1);
    uint32_t val2 = cpu->getRegister(data->rs2);
    
    // BEQ (000)
    if (data->func3 == 0) {
        if (val1 == val2) {
            // Update PC
        }
    
    // BNE (001)
    } else if (data->func3 == 1) {
        if (val1 != val2) {
            // Update PC
        }
    
    // BLT (100)
    } else if (data->func3 == 4) {
        if (val1 < val2) {
            // Update PC
        }
    
    // BGE (101)
    } else if (data->func3 == 5) {
        if (val1 >= val2) {
            // Update PC
        }
    
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

void print_instruction(Data *data) {
    
    // First, print instruction type
    switch (data->opcode) {
        // R-Types
        case 0b0110011: {
            std::cout << "R-Type" << std::endl;
            printf("Func3: %X | Func7: %X\n", data->func3, data->func7);
            
            std::string instr_str = "";
            switch (data->func3) {
                case 0: {
                    if (data->func7 == 32) instr_str = "sub";
                    else instr_str = "add";
                } break;
                case 1: instr_str = "sll"; break;
                case 2: instr_str = "slt"; break;
                case 3: instr_str = "sltu"; break;
                case 4: instr_str = "xor"; break;
                case 5: {
                    if (data->func7 == 32) instr_str = "sra";
                    else instr_str = "srl";
                } break;
                case 6: instr_str = "or"; break;
                case 7: instr_str = "and"; break;
                
                default: {}
            }
            instr_str += " x" + to_string(data->rs1) + ", x" + to_string(data->rs2) + ", x" + to_string(data->rs2);
            std::cout << instr_str << std::endl;
        } break;
        
        // I-Types
        case 0b0010011: {
            std::cout << "I-Type" << std::endl;
            printf("Func3: %X\n", data->func3);
            std::cout << "Imm: " << data->imm << std::endl;
            
            std::string instr_str = "";
            switch (data->func3) {
                case 0: instr_str = "addi"; break;
                case 1: instr_str = "slli"; break;
                case 2: instr_str = "slti"; break;
                case 3: instr_str = "sltui"; break;
                case 4: instr_str = "xori"; break;
                case 5: {
                    if (data->func7 == 32) instr_str = "srai";
                    else instr_str = "srli";
                } break;
                case 6: instr_str = "ori"; break;
                case 7: instr_str = "andi"; break;
                
                default: {}
            }
            instr_str += " x" + to_string(data->rd) + ", x" + to_string(data->rs1) + ", ";
            if (data->func3 == 1 || data->func3 == 5) instr_str += to_string(data->rs2);
            else instr_str += to_string(data->imm);
            std::cout << instr_str << std::endl;
        } break;
        
        // B-Types
        case 0x23:
        case 0x2F: {
            std::cout << "B-Type" << std::endl;
            printf("Func3: %X\n", data->func3);
            std::cout << "Imm: " << data->imm << std::endl;
            
            std::string instr_str = "";
            switch (data->func3) {
                case 0: instr_str = "beq"; break;
                case 1: instr_str = "bne"; break;
                case 4: instr_str = "blt"; break;
                case 5: instr_str = "bge"; break;
                case 6: instr_str = "bltu"; break;
                case 7: instr_str = "bgeu"; break;
                
                default: {}
            }
            instr_str += " x" + to_string(data->rd) + ", x" + to_string(data->rs1) + ", SOMEWHERE";
            std::cout << instr_str << std::endl;
        } break;
        
        default: {}
    }
    
    std::cout << "-----------------------------" << std::endl;
}

// TODO: Print instruction type rather than just instruction
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
    
    int instr_count = 26;
    uint32_t instr_memory[] = {
        0x00a00093,
        0x01400113,
        0x00a12193,
        0x00a13213,
        0x00a14293,
        0x00a16313,
        0x00a17393,
        0x00a11413,
        0x00a15493,
        0x40a15513,
        0x002081b3,
        0x40208233,
        0x002092b3,
        0x0020d333,
        0x4020d3b3,
        0x0020f433,
        0x0020e4b3,
        0x0020c533,
        0x0020a5b3,
        0x0020b633,
        0x00209463,
        0xfadff06f,
        0x00208463,
        0xfa5ff06f,
        0x0020d463,
        0xf9dff06f,
        0x0020c463,
        0xf95ff06f,
        0x0020f463,
        0xf8dff06f,
        0x0020e463,
        0xf85ff06f
    };
    
    //Data *d1 = decode(instr1);
    //print_instruction(d1);
   // 
    //Data *d2 = decode(instr2);
    //print_instruction(d2);
    for (int i = 0; i<instr_count; i++) {
        Data *d = decode(instr_memory[i]);
        print_instruction(d);
    }
    
    // Execute and test
    //CPU *cpu = new CPU;
    //execute(d1, cpu);
    //execute(d2, cpu);
    
    //cpu->debugRegisters();
    
    return 0;
}

