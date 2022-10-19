#include "cpu.hpp"

void print_instruction(Data *data);

//
// The main CPU loop
//
void CPU::run() {
    // TODO: We need a better stop
    while (pc < 10 * 4) {
        State *decodeState1 = decodeState;
        State *exeState1 = executeState;
        State *storeState1 = storeState;
        decodeState = nullptr;
        executeState = nullptr;
        storeState = nullptr;
        
        // TODO: There are stalls, for now we're assuming not
        fetch();
        decode(decodeState1);
        execute(exeState1);
        store(storeState1);
    }
}

//
// The fetch stage
//
void CPU::fetch() {
    uint8_t *buffer = new uint8_t[4];
    buffer[0] = memory[pc];
    buffer[1] = memory[pc + 1];
    buffer[2] = memory[pc + 2];
    buffer[3] = memory[pc + 3];

    uint32_t instr = (uint32_t)buffer[3] << 24 | (uint32_t)buffer[2] << 16 | (uint32_t)buffer[1] << 8 | (uint32_t)buffer[0];
    
    State *state = new State;
    state->instr = instr;
    decodeState = state;
    
    pc += 4;
}

//
// The execute stage
//
void CPU::execute(State *state) {
    if (!state) return;
    print_instruction(state->decodeData);
    Data *data = state->decodeData;
    
    // Get the ALU inputs
    uint32_t src1 = registers[data->rs1];
    uint32_t src2 = 0;
    if (data->alu_src == 0) {
        src2 = registers[data->rs2];
    } else {
        src2 = data->imm_i;
    }
    
    // Execute
    uint32_t result = 0;
    if (data->branch) {
        result = executeBRU(data, src1, src2);
    } else {
        result = executeALU(data, src1, src2);
    }
}

//
// The ALU
//
uint32_t CPU::executeALU(Data *data, uint32_t src1, uint32_t src2) {
    switch (data->aluop) {
        // Add and Sub
        case 0: {
        
        } break;
        
        // Shift left
        case 1: return src1 << src2;
        
        // SLT
        case 2: {
            if ((int)src1 < (int)src2) return 1;
            return 0;
        } break;
        
        // SLTU (set less than unsigned)
        case 3: {
            if (src1 < src2) return 1;
            return 0;
        } break;
        
        // XOR
        case 4: return src1 ^ src2;
        
        // SRL/SRA (shift right logic/arithmetic)
        // Logic vs arithmetic determined by func7
        case 5: {
            // TODO: We need to make the difference, this may not be right
            return src1 >> src2;
        } break;
        
        // OR
        case 6: return src1 | src2;
        
        // AND
        case 7: return src1 & src2;
    }
    
    return 0;
}

//
// The BLU
// Branch-control unit
//
uint32_t CPU::executeBRU(Data *data, uint32_t src1, uint32_t src2) {
    switch (data->aluop) {
        // BEQ
        case 0: {
            if (src1 == src2) return 1;
            return 0;
        }
        
        // BNE
        case 1: {
            if (src1 != src2) return 1;
            return 0;
        }
        
        // BLT
        case 4: {
            if ((int)src1 < (int)src2) return 1;
            return 0;
        }
        
        // BGE
        case 5: {
            if ((int)src1 >= (int)src2) return 1;
            return 0;
        }
        
        // BLTU
        case 6: {
            if (src1 < src2) return 1;
            return 0;
        }
        
        // BGEU
        case 7: {
            if (src1 >= src2) return 1;
            return 0;
        }
    }
    
    return 0;
}

//
// The store stage
//
void CPU::store(State *state) {

}

