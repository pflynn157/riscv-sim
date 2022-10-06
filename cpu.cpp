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
    uint32_t imm_i = 0;
    uint32_t imm_b = 0;
    uint32_t imm_s = 0;
    uint32_t imm_u = 0;
    uint32_t imm_j = 0;
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
    
    data->opcode =  (uint8_t)(instr & 0b00000000000000000000000001111111);
    data->rd =     (uint8_t)((instr & 0b00000000000000000000111110000000) >> 7);
    data->imm_u = (uint32_t)((instr & 0b11111111111111111111000000000000) >> 12);
    data->func3 =  (uint8_t)((instr & 0b00000000000000000111000000000000) >> 12);
    data->rs1 =    (uint8_t)((instr & 0b00000000000011111000000000000000) >> 15);
    data->imm_i = (uint32_t)((instr & 0b11111111111100000000000000000000) >> 20);
    data->rs2 =    (uint8_t)((instr & 0b00000001111100000000000000000000) >> 20);
    data->func7 =  (uint8_t)((instr & 0b11111110000000000000000000000000) >> 25);
    
    // Branch Imm: imm[4:1][11][10:5][12]
    data->imm_b = ((uint32_t)(instr & 0x80000000) >> 19)
        | ((instr & 0x80) << 4)
        | ((instr >> 20) & 0x7e0)
        | ((instr >> 7) & 0x1e);
        
    // Store Imm: imm[4:0] imm[11:5]
    data->imm_s = data->rd | ((data->func7) >> 20);
    
    // J-Types
    data->imm_j = ((uint32_t)(instr & 0x80000000) >> 11)
        | (instr & 0xff000)              // imm[19:12]
        | ((instr >> 9) & 0x800)         // imm[11]
        | ((instr >> 20) & 0x7fe);       // imm[10:1]
    
    // opcode
    // 6 - 0
    /*data->opcode = (uint8_t)(instr & 0b00000000000000000000000000111111);
    
    // rd
    // 11 - 7
    data->rd = (uint8_t)((instr & 0b00000000000000000000011111000000) >> 7);
    
    // funct3
    // 14 - 12
    data->func3 = (uint8_t)((instr & 0b000000000000000000111000000000000) >> 12);
    
    // rs1
    // 19 - 15
    data->rs1 = (uint8_t)((instr & 0b000000000000011111000000000000000) >> 15);
    
    // rs2
    // 24 - 20
    data->rs2 = (uint8_t)((instr & 0b000000001111100000000000000000000) >> 20);
    
    // func7
    // 31 - 25
    data->func7 = (uint8_t)((instr & 0b1111111000000000000000000000000) >> 25);
    
    // imm
    // 31 - 20
    data->imm = (uint32_t)((instr & 0b1111111111110000000000000000000) >> 20);*/
    
    return data;
}

//
// Handles all ALU operations
//
void alu(Data *data, CPU *cpu, bool isImmediate) {
    uint32_t val1 = cpu->getRegister(data->rs1);
    uint32_t val2 = 0;
    if (isImmediate) val2 = data->imm_i;
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
        //
        // R-Types
        //
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
            instr_str += " x" + to_string(data->rd) + ", x" + to_string(data->rs1) + ", x" + to_string(data->rs2);
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // I-Types
        //
        case 0b0010011: {
            std::cout << "I-Type" << std::endl;
            printf("Func3: %X\n", data->func3);
            std::cout << "Imm: " << data->imm_i << std::endl;
            
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
            else instr_str += to_string(data->imm_i);
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // JALR (I-TYPE)
        //
        case 0b1100111: {
            std::cout << "I-Type (JALR)" << std::endl;
            printf("Func3: %X\n", data->func3);
            std::cout << "Imm: " << data->imm_i << std::endl;
            
            std::string instr_str = "jalr ";
            instr_str += " x" + to_string(data->rd) + ", x" + to_string(data->rs1) + ", ";
            instr_str += to_string(data->imm_i);
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // B-Types
        //
        case 0b1100011: {
            std::cout << "B-Type" << std::endl;
            printf("Func3: %X\n", data->func3);
            std::cout << "Imm: " << (signed int)data->imm_b << std::endl;
            
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
            instr_str += " x" + to_string(data->rs1) + ", x" + to_string(data->rs2) + ", SOMEWHERE";
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // Loads
        //
        case 0b0000011: {
            std::cout << "I-Type (Loads)" << std::endl;
            printf("Func3: %X\n", data->func3);
            std::cout << "Offset: " << data->imm_i << std::endl;
            
            std::string instr_str = "lw x" + to_string(data->rd) + ", ";
            instr_str += to_string(data->imm_i) + "(x";
            instr_str += to_string(data->rs1) + ")";
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // Stores
        // S-Type
        //
        case 0b0100011: {
            uint32_t imm = data->imm_s;
        
            std::cout << "S-Type (Store)" << std::endl;
            printf("Func3: %X\n", data->func3);
            std::cout << "Offset: " << imm << std::endl;
            
            std::string instr_str = "sw x" + to_string(data->rs2) + ", ";
            instr_str += to_string(imm) + "(x";
            instr_str += to_string(data->rs1) + ")";
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // LUI
        // U-Type
        //
        case 0b0110111: {
            std::cout << "U-Type (LUI)" << std::endl;
            std::cout << "Offset: " << data->imm_u << std::endl;
            
            std::string instr_str = "lui x" + to_string(data->rd) + ", ";
            instr_str += to_string(data->imm_u);
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // AUIPC
        // U-Type
        //
        case 0b0010111: {
            std::cout << "U-Type (AUIPC)" << std::endl;
            std::cout << "Offset: " << data->imm_u << std::endl;
            
            std::string instr_str = "auipc x" + to_string(data->rd) + ", ";
            instr_str += to_string(data->imm_u);
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // JAL | J-Type
        //
        case 0b1101111: {
            int32_t imm = (int32_t)data->imm_j;
        
            std::cout << "J-Type (JAL)" << std::endl;
            std::cout << "Offset: " << imm << std::endl;
            
            std::string instr_str = "jal x" + to_string(data->rd) + ", ";
            instr_str += to_string(imm);
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
    
    std::vector<uint32_t> instr_memory;
    
    instr_memory.push_back(0x00a00093);     // addi x1, x0, 10
    instr_memory.push_back(0x01400113);     // addi x2, x0, 20
    instr_memory.push_back(0x00a12193);     // slti x3, x2, 10
    instr_memory.push_back(0x00a13213);     // sltiu x4, x2, 10
    instr_memory.push_back(0x00a14293);     // xori x5, x2, 10
    instr_memory.push_back(0x00a16313);     // ori x6, x2, 10
    instr_memory.push_back(0x00a17393);     // andi x7, x2, 10
    instr_memory.push_back(0x00a11413);     // slli x8, x2, 10
    instr_memory.push_back(0x00a15493);     // srli x9, x2, 10
    instr_memory.push_back(0x40a15513);     // srai x10, x2, 10
    
    instr_memory.push_back(0x002081b3);     // add x3, x1, x2
    instr_memory.push_back(0x40208233);     // sub x4, x1, x2
    instr_memory.push_back(0x002092b3);     // sll x5, x1, x2
    instr_memory.push_back(0x0020d333);     // srl x6, x1, x2
    instr_memory.push_back(0x4020d3b3);     // sra x7, x1, x2
    instr_memory.push_back(0x0020f433);     // and x8, x1, x2
    instr_memory.push_back(0x0020e4b3);     // or x9, x1, x2
    instr_memory.push_back(0x0020c533);     // xor x10, x1, x2
    instr_memory.push_back(0x0020a5b3);     // slt x11, x1, x2
    instr_memory.push_back(0x0020b633);     //sltu x12, x1, x2
    
    instr_memory.push_back(0x00208063);     // beq x1, x2, LOOP
    instr_memory.push_back(0xfe209ee3);     // bne x1, x2, LOOP
    instr_memory.push_back(0xfe20cce3);     // blt x1, x2, LOOP
    instr_memory.push_back(0xfe20dae3);     // bge x1, x2, LOOP
    instr_memory.push_back(0xfe20e8e3);     // bltu x1, x2, LOOP
    instr_memory.push_back(0xfe20f6e3);     // bgeu x1, x2, LOOP
    
    instr_memory.push_back(0x00012083);     // lw x1, 0(x2)
    instr_memory.push_back(0x00a1a103);     // lw x2, 10(x3)
    instr_memory.push_back(0x01422183);     // lw x3, 20(x4)
    instr_memory.push_back(0x00e7a203);     // lw x4, 14(x15)
    instr_memory.push_back(0x00f0a503);     // lw x10, 15(x1)
    
    instr_memory.push_back(0x00ab2023);     // sw x10, 0(x22)
    instr_memory.push_back(0x014f2523);     // sw x20, 10(x30)
    instr_memory.push_back(0x00d22a23);     // sw x13, 20(x4)
    instr_memory.push_back(0x00e7a723);     // sw x14, 14(x15)
    instr_memory.push_back(0x0010a7a3);     // sw x1, 15(x1)
    
    instr_memory.push_back(0x007e40b7);     // lui x1, 2020
    instr_memory.push_back(0x007e5137);     // lui x2, 2021
    instr_memory.push_back(0x007e61b7);     // lui x3, 2022
    
    instr_memory.push_back(0x000bc617);     // auipc x12, 188
    instr_memory.push_back(0x0012bb17);     // auipc x22, 299
    instr_memory.push_back(0x0018f797);     // auipc x15, 399
    
    instr_memory.push_back(0x0080056f);     // jal x10, BOOT1
    instr_memory.push_back(0xffdff5ef);     // jal x11, BOOT2
    
    instr_memory.push_back(0x065500e7);     // jalr x1, x10, 101
    instr_memory.push_back(0x0c960167);     // jalr x2, x12, 201
    
    
    //Data *d1 = decode(instr1);
    //print_instruction(d1);
   // 
    //Data *d2 = decode(instr2);
    //print_instruction(d2);
    for (int i = 0; i<instr_memory.size(); i++) {
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

