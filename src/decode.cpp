#include "cpu.hpp"

//
// The decode stage
//
void CPU::decode(State *state) {
    if (!state) return;
    
    uint32_t instr = state->instr;
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
        
    // Sets all the control signals based on the decoded instruction
    decodeSet(data);
        
    // Create the new state
    State *newState = new State;
    newState->decodeData = data;
    executeState = newState;
}

void CPU::decodeSet(Data *data) {
    switch (data->opcode) {
        //
        // R-Types
        //
        case 0b0110011: {
            //print_signals(0, 0, 0, data->func3, 0, 0, 1, 0, 0);
            //std::cout << "Invert: " << int(data->func7 == 32) << std::endl;
            data->branch = 0;
            data->mem_read = 0;
            data->mem2reg = 0;
            data->aluop = data->func3;
            data->mem_write = 0;
            data->alu_src = 0;
            data->reg_write = 1;
            data->rs1_src = 0;
            data->pc_write = 0;
            
            if (data->func7 == 32) data->alu_invert = 1;
        } break;
        
        //
        // I-Types
        //
        case 0b0010011: {
            //print_signals(0, 0, 0, data->func3, 0, 1, 1, 0, 0);
            data->branch = 0;
            data->mem_read = 0;
            data->mem2reg = 0;
            data->aluop = data->func3;
            data->mem_write = 0;
            data->alu_src = 1;
            data->reg_write = 1;
            data->rs1_src = 0;
            data->pc_write = 0;
        } break;
        
        //
        // JALR (I-TYPE)
        //
        case 0b1100111: {
            //print_signals(1, 0, 0, 000, 0, 1, 1, 1, 1);
            data->branch = 1;
            data->mem_read = 0;
            data->mem2reg = 0;
            data->aluop = 000;
            data->mem_write = 0;
            data->alu_src = 1;
            data->reg_write = 1;
            data->rs1_src = 1;
            data->pc_write = 1;
        } break;
        
        //
        // B-Types
        //
        case 0b1100011: {
            //print_signals(1, 0, 0, 000, 0, 0, 0, 0, 0);
            data->branch = 1;
            data->mem_read = 0;
            data->mem2reg = 0;
            data->aluop = 000;
            data->mem_write = 0;
            data->alu_src = 0;
            data->reg_write = 0;
            data->rs1_src = 0;
            data->pc_write = 0;
        } break;
        
        //
        // Loads
        //
        case 0b0000011: {
            //print_signals(0, 1, 1, 000, 0, 1, 1, 0, 0);
            data->branch = 0;
            data->mem_read = 1;
            data->mem2reg = 1;
            data->aluop = 000;
            data->mem_write = 0;
            data->alu_src = 1;
            data->reg_write = 1;
            data->rs1_src = 0;
            data->pc_write = 0;
        } break;
        
        //
        // Stores
        // S-Type
        //
        case 0b0100011: {
            //print_signals(0, 0, 0, 000, 1, 1, 0, 0, 0);
            data->branch = 0;
            data->mem_read = 0;
            data->mem2reg = 0;
            data->aluop = 000;
            data->mem_write = 1;
            data->alu_src = 1;
            data->reg_write = 0;
            data->rs1_src = 0;
            data->pc_write = 0;
        } break;
        
        //
        // LUI
        // U-Type
        //
        case 0b0110111: {
            //print_signals(0, 0, 0, 000, 0, 1, 1, 0, 0);
            data->branch = 0;
            data->mem_read = 0;
            data->mem2reg = 0;
            data->aluop = 000;
            data->mem_write = 0;
            data->alu_src = 1;
            data->reg_write = 1;
            data->rs1_src = 0;
            data->pc_write = 0;
        } break;
        
        //
        // AUIPC
        // U-Type
        //
        case 0b0010111: {
            //print_signals(0, 0, 0, 000, 0, 1, 1, 1, 0);
            data->branch = 0;
            data->mem_read = 0;
            data->mem2reg = 0;
            data->aluop = 000;
            data->mem_write = 0;
            data->alu_src = 1;
            data->reg_write = 1;
            data->rs1_src = 1;
            data->pc_write = 0;
        } break;
        
        //
        // JAL | J-Type
        //
        case 0b1101111: {
            //print_signals(1, 0, 0, 000, 0, 1, 1, 1, 1);
            data->branch = 1;
            data->mem_read = 0;
            data->mem2reg = 0;
            data->aluop = 000;
            data->mem_write = 0;
            data->alu_src = 1;
            data->reg_write = 1;
            data->rs1_src = 1;
            data->pc_write = 1;
        } break;
        
        default: {}
    }
}

