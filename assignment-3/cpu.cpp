#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include <fstream>

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

void print_signals(int branch, int mem_read, int mem2reg, int aluop, int mem_write, int alu_src, int reg_write, int rs1_src, int pc_write) {
    std::cout << "Branch: " << branch << " | MemRead: " << mem_read << " | MemToRegister: " << mem2reg;
    std::cout << " | ALUop: " << aluop << std::endl;
    std::cout << "MemWrite: " << mem_write << " | ALUsrc: " << alu_src << " | RegWrite: " << reg_write;
    std::cout << " | RS1src: " << rs1_src;
    std::cout << std::endl;
    
    // The branch mux
    std::cout << "PC+4: " << int(!branch) << " | PC+Imm: " << int(branch && !pc_write) << " | PC=rd: " << int(pc_write) << std::endl;
    
    // Imm mux
    std::cout << "Read Data 2: " << int(!alu_src) << " | Read Imm: " << int(alu_src) << std::endl;
    
    // Store mux
    std::cout << "Writeback ALU Result: " << int(!mem2reg && reg_write) << " | Writeback Memory Data: " << int(mem2reg && reg_write) << std::endl;
}

void print_instruction(Data *data) {
    
    // First, print instruction type
    switch (data->opcode) {
        //
        // R-Types
        //
        case 0b0110011: {
            std::cout << "R-Type" << std::endl;
            printf("ALU Opcode: %X\n", data->func3);
            
            std::cout << "Rd: " << to_string(data->rd) << " | Rs1: " << to_string(data->rs1);
            std::cout << " | Rs2: " << to_string(data->rs2) << std::endl;
            print_signals(0, 0, 0, data->func3, 0, 0, 1, 0, 0);
            std::cout << "Invert: " << int(data->func7 == 32) << std::endl;
            
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
            printf("ALU Opcode: %X\n", data->func3);
            
            uint32_t imm = data->imm_i;
            if (data->func3 == 1 || data->func3 == 5) {
                imm = data->rs2;
            }
            
            std::cout << "Signed Extended Immediate: " << imm << std::endl;
            std::cout << "Rd: " << to_string(data->rd) << " | Rs1: " << to_string(data->rs1) << std::endl;
            print_signals(0, 0, 0, data->func3, 0, 1, 1, 0, 0);
            
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
            instr_str += to_string(imm);
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // JALR (I-TYPE)
        //
        case 0b1100111: {
            std::cout << "I-Type (JALR)" << std::endl;
            printf("Func3: %X\n", data->func3);
            std::cout << "Imm: " << data->imm_i << std::endl;
            
            std::cout << "Rd: " << to_string(data->rd) << " | Rs1: " << to_string(data->rs1) << "| Rs2: PC" << std::endl;
            print_signals(1, 0, 0, 000, 0, 1, 1, 1, 1);
            
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
            printf("Branch Opcode: %X\n", data->func3);
            std::cout << "Branch Destination: " << (signed int)data->imm_b << std::endl;
            
            std::cout << "Rs1: " << to_string(data->rs1) << " | Rs2: " << to_string(data->rs2) << std::endl;
            print_signals(1, 0, 0, 000, 0, 0, 0, 0, 0);
            
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
            instr_str += " x" + to_string(data->rs1) + ", x" + to_string(data->rs2) + ", ";
            instr_str += to_string((signed int)data->imm_b);
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // Loads
        //
        case 0b0000011: {
            std::cout << "I-Type (Load)" << std::endl;
            printf("Length Opcode: %X\n", data->func3);
            std::cout << "Offset: " << data->imm_i << std::endl;
            
            std::cout << "Rd: " << to_string(data->rd) << " | Rs1: " << to_string(data->rs1) << std::endl;
            print_signals(0, 1, 1, 000, 0, 1, 1, 0, 0);
            
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
            printf("Length Opcode: %X\n", data->func3);
            std::cout << "Offset: " << imm << std::endl;
            
            std::cout << "Rs1: " << to_string(data->rs1) << " | Rs2: " << to_string(data->rs2) << std::endl;
            print_signals(0, 0, 0, 000, 1, 1, 0, 0, 0);
            
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
            std::cout << "Upper Immediate Value: " << data->imm_u << std::endl;
            
            std::cout << "Rd: " << to_string(data->rd) << std::endl;
            print_signals(0, 0, 0, 000, 0, 1, 1, 0, 0);
            
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
            std::cout << "Upper Immediate Value: " << data->imm_u << std::endl;
            
            std::cout << "Rd: " << to_string(data->rd) << " | Rs1: PC" << std::endl;
            print_signals(0, 0, 0, 000, 0, 1, 1, 1, 0);
            
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
            std::cout << "Signed Extended Offset: " << imm << std::endl;
            
            std::cout << "Rd: " << to_string(data->rd) << " | Rs1: PC" << std::endl;
            print_signals(1, 0, 0, 000, 0, 1, 1, 1, 1);
            
            std::string instr_str = "jal x" + to_string(data->rd) + ", ";
            instr_str += to_string(imm);
            std::cout << instr_str << std::endl;
        } break;
        
        default: {}
    }
    
    std::cout << "-----------------------------" << std::endl;
}

// TODO: Print instruction type rather than just instruction
int main(int argc, char **argv) {
    if (argc == 1) {
        std::cerr << "Error: Please specify a binary file!" << std::endl;
        return 1;
    }
    std::string input = argv[1];
    
    std::vector<uint32_t> instr_memory;
    std::ifstream reader(input, std::ifstream::binary);
    if (!reader.is_open()) {
        std::cerr << "Error: Unable to open binary file. Make sure it is assembled." << std::endl;
        return 1;
    }

    while (!reader.eof()) {
        uint8_t *buffer = new uint8_t[4];
        buffer[0] = reader.get();
        buffer[1] = reader.get();
        buffer[2] = reader.get();
        buffer[3] = reader.get();

        uint32_t instr = (uint32_t)buffer[3] << 24 | (uint32_t)buffer[2] << 16 | (uint32_t)buffer[1] << 8 | (uint32_t)buffer[0];
        instr_memory.push_back(instr);
    }
    
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

