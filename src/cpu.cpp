#include <cstring>

#include "cpu.hpp"

void print_instruction(Data *data);

//
// The main CPU loop
//
void CPU::run() {
    // TODO: We need a better stop
    while (pc < 50 * 4) {
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
    /*uint8_t *buffer = new uint8_t[4];
    buffer[0] = memory[pc];
    buffer[1] = memory[pc + 1];
    buffer[2] = memory[pc + 2];
    buffer[3] = memory[pc + 3];

    uint32_t instr = (uint32_t)buffer[3] << 24 | (uint32_t)buffer[2] << 16 | (uint32_t)buffer[1] << 8 | (uint32_t)buffer[0];*/
    uint32_t instr = ram->getMemory(pc);
    
    State *state = new State;
    state->instr = instr;
    decodeState = state;
    
    pc += 4;
}

//
// The execute stage
//
void CPU::execute(State *state) {
    if (state == nullptr) return;
    print_instruction(state->decodeData);
    Data *data = state->decodeData;
    
    State *nextState = new State;
    storeState = nextState;
    
    if (data->fop) {
        executeFPU(state);
        return;
    }
    
    // Get the ALU inputs
    uint32_t src1 = getRegister(data->rs1);
    uint32_t src2 = 0;
    
    if (data->alu_src == 0) {
        src2 = getRegister(data->rs2);
    } else {
        if (data->mem_write) {
            src2 = data->imm;
            data->aluop = 0;
        } else if (data->mem_read) {
            src2 = data->imm;
            data->aluop = 0;
        } else {
            src2 = data->imm;
        }
    }
    
    // Execute
    uint32_t result = 0;
    
    // Branch mux
    if (data->branch) {
        result = executeBRU(data, src1, src2);
        if (result == 1) {
            int32_t imm = (int32_t)data->imm;
            int32_t offset = -12;
            if (imm < 0) offset = -12;
            pc += data->imm + offset;
            printf("NEW_PC: %d\n", pc);
            decodeState = nullptr;
            executeState = nullptr;
            storeState = nullptr;
        }
        
    } else {
        result = executeALU(data, src1, src2);
        
        // mem_write mux
        if (data->mem_write) {
            nextState->write = true;
            nextState->address = result;
            
            if (data->float_reg_write) {
                float fdata = getFloatRegister(data->rs2);
                uint32_t mem = 0;
                memcpy(&mem, &fdata, sizeof(uint32_t));
                nextState->write_data = mem;
            } else {
                nextState->write_data = getRegister(data->rs2);
            }
        } else {
        
            // mem2reg mux
            if (data->mem2reg) {
                nextState->read = true;
                nextState->address = result;
                nextState->rd = data->rd;
                nextState->write_float = (bool)data->float_reg_write;
            } else {
                setRegister(data->rd, result);
            }
        }
    }
}

//
// The ALU
//
uint32_t CPU::executeALU(Data *data, uint32_t src1, uint32_t src2) {
    switch (data->aluop) {
        // Add and Sub
        case 0: {
            return src1 + src2;
        } break;
        
        // Shift left
        case 1: return src1 >> src2;
        
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
            return src1 << src2;
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
// The FPU
//
void CPU::executeFPU(State *state) {
    if (!state) return;
    Data *data = state->decodeData;
    
    float src1 = getFloatRegister(data->rs1);
    float src2 = getFloatRegister(data->rs2);
    float result = src1 + src2;
    
    setFloatRegister(data->rd, result);
}

//
// The store stage
//
void CPU::store(State *state) {
    if (!state) return;
    
    if (state->write) {
        ram->setMemory(state->address, state->write_data);
    }
    
    if (state->read) {
        /*uint8_t data0 = memory[state->address];
        uint8_t data1 = memory[state->address + 1];
        uint8_t data2 = memory[state->address + 2];
        uint8_t data3 = memory[state->address + 3];
        
        uint32_t data = (uint32_t)data3 << 24 | (uint32_t)data2 << 16 | (uint32_t)data1 << 8 | (uint32_t)data0;*/
        
        if (state->write_float) {
            float fdata = 0;
            uint32_t data = ram->getMemory(state->address);
            memcpy(&fdata, &data, sizeof(uint32_t));
            setFloatRegister(state->rd, fdata); 
        } else {
            uint32_t data = ram->getMemory(state->address);
            setRegister(state->rd, data);
        }
    }
}

