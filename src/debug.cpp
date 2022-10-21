#include "cpu.hpp"

void print_signals(Data *data) {
    std::cout << "Branch: " << data->branch << " | MemRead: " << data->mem_read << " | MemToRegister: " << data->mem2reg;
    std::cout << std::endl;
    std::cout << " | Fop: " << data->fop << std::endl;
    std::cout << " | ALUop: " << data->aluop << std::endl;
    std::cout << "MemWrite: " << data->mem_write << " | ALUsrc: " << data->alu_src << " | RegWrite: " << data->reg_write;
    std::cout << " | Float RegWrite: " << data->float_reg_write;
    std::cout << " | RS1src: " << data->rs1_src;
    std::cout << std::endl << "| ALU_Invert: " << data->alu_invert;
    std::cout << std::endl;
    
    // The branch mux
    std::cout << "PC+4: " << int(!data->branch) << " | PC+Imm: " << int(data->branch && !data->pc_write) << " | PC=rd: " << int(data->pc_write) << std::endl;
    
    // Imm mux
    std::cout << "Read Data 2: " << int(!data->alu_src) << " | Read Imm: " << int(data->alu_src) << std::endl;
    
    // Store mux
    std::cout << "Writeback ALU Result: " << int(!data->mem2reg && data->reg_write) << " | Writeback Memory Data: "
                << int(data->mem2reg && data->reg_write) << std::endl;
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
            print_signals(data);
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
            
            uint32_t imm = data->imm;
            if (data->func3 == 1 || data->func3 == 5) {
                imm = data->rs2;
            }
            
            std::cout << "Signed Extended Immediate: " << imm << std::endl;
            std::cout << "Rd: " << to_string(data->rd) << " | Rs1: " << to_string(data->rs1) << std::endl;
            print_signals(data);
            
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
            std::cout << "Imm: " << data->imm << std::endl;
            
            std::cout << "Rd: " << to_string(data->rd) << " | Rs1: " << to_string(data->rs1) << "| Rs2: PC" << std::endl;
            print_signals(data);
            
            std::string instr_str = "jalr ";
            instr_str += " x" + to_string(data->rd) + ", x" + to_string(data->rs1) + ", ";
            instr_str += to_string(data->imm);
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // B-Types
        //
        case 0b1100011: {
            std::cout << "B-Type" << std::endl;
            printf("Branch Opcode: %X\n", data->func3);
            std::cout << "Branch Destination: " << (signed int)data->imm << std::endl;
            
            std::cout << "Rs1: " << to_string(data->rs1) << " | Rs2: " << to_string(data->rs2) << std::endl;
            print_signals(data);
            
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
        case 0b0000111:
        case 0b0000011: {
            std::cout << "I-Type (Load)" << std::endl;
            printf("Length Opcode: %X\n", data->func3);
            std::cout << "Offset: " << data->imm << std::endl;
            
            std::cout << "Rd: " << to_string(data->rd) << " | Rs1: " << to_string(data->rs1) << std::endl;
            print_signals(data);
            
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
            uint32_t imm = data->imm;
        
            std::cout << "S-Type (Store)" << std::endl;
            printf("Length Opcode: %X\n", data->func3);
            std::cout << "Offset: " << imm << std::endl;
            
            std::cout << "Rs1: " << to_string(data->rs1) << " | Rs2: " << to_string(data->rs2) << std::endl;
            print_signals(data);
            
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
            std::cout << "Upper Immediate Value: " << data->imm << std::endl;
            
            std::cout << "Rd: " << to_string(data->rd) << std::endl;
            print_signals(data);
            
            std::string instr_str = "lui x" + to_string(data->rd) + ", ";
            instr_str += to_string(data->imm);
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // AUIPC
        // U-Type
        //
        case 0b0010111: {
            std::cout << "U-Type (AUIPC)" << std::endl;
            std::cout << "Upper Immediate Value: " << data->imm << std::endl;
            
            std::cout << "Rd: " << to_string(data->rd) << " | Rs1: PC" << std::endl;
            print_signals(data);
            
            std::string instr_str = "auipc x" + to_string(data->rd) + ", ";
            instr_str += to_string(data->imm);
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
            print_signals(data);
            
            std::string instr_str = "jal x" + to_string(data->rd) + ", ";
            instr_str += to_string(imm);
            std::cout << instr_str << std::endl;
        } break;
        
        //
        // F-ALU type
        //
        case 0b1010011: {
            std::cout << "FR-Type" << std::endl;
            printf("ALU Opcode: %X\n", data->func3);
            
            std::cout << "Fd: " << to_string(data->rd) << " | Fs1: " << to_string(data->rs1);
            std::cout << " | Fs2: " << to_string(data->rs2) << std::endl;
            print_signals(data);
            
            std::string instr_str = "fadd.s";
            instr_str += " f" + to_string(data->rd) + ", f" + to_string(data->rs1) + ", f" + to_string(data->rs2);
            std::cout << instr_str << std::endl;
        } break;
        
        default: {}
    }
    
    std::cout << "-----------------------------" << std::endl;
}

