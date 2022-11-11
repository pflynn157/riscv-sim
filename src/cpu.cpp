#include <cstring>

#include "cpu.hpp"

void print_instruction(Data *data);

//
// The main CPU loop
//
void CPU::run() {
    // TODO: We need a better stop
    while (pc < (pc+ 50 * 4)) {
        std::cout << "--------------------------------" << std::endl;
        std::cout << name << " [CLOCK]: " << clock_cycles << std::endl;
        std::cout << "--------------------------------" << std::endl;
    
        State *decodeState1 = decodeState;
        State *exeState1 = executeState;
        State *storeState1 = storeState;
        decodeState = nullptr;
        executeState = nullptr;
        storeState = nullptr;

        
        if (exeState1 && exeState1->stall > 0) {
            pc -= 4;
            std::cout << "STALL [PC: " << pc << "]" << std::endl;
            decode(nullptr);
            
            execute(exeState1);
            store(storeState1);
            
            int start = clock_cycles;
            for (int i = 1; i<exeState1->stall; i++) {
                decode(nullptr);
                execute(nullptr);
                store(nullptr);
                ++clock_cycles;
            }
            
        // Otherwise, fetch and decode
        } else {
            fetch();
            decode(decodeState1);
            execute(exeState1);
            store(storeState1);
        }
        
        
        ++clock_cycles;
        
        if (halt) {
            if (decodeState == nullptr && executeState == nullptr && storeState == nullptr) {
                break;
            }
        }
    }
}

//
// The fetch stage
//
void CPU::fetch() {
    //uint32_t instr = ram->getMemory(pc);
    uint32_t instr = icache->getData(clock_cycles, pc, 4);
    
    if (halt || instr == 0x00FFFF) {
        halt = true;
        return;
    }
    
    State *state = new State;
    state->instr = instr;
    decodeState = state;
    
    pc += 4;
    ++i_count;
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
    
    // For add upper immediate
    // TODO: May need to clean up
    if (data->addui) {
        uint32_t reg = getRegister(data->rd);
        reg = data->imm_u << 12;
        //std::cout << "RD: " << data->rd << " = " << reg << std::endl;
        setRegister(data->rd, reg);
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
            halt = false;
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
            if (data->alu_invert) return src1 - src2;
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
    
    float result = 0;
    if (data->alu_invert) result = src1 - src2;
    else result = src1 + src2;
    
    setFloatRegister(data->rd, result);
}

//
// The store stage
//
void CPU::store(State *state) {
    if (!state) return;
    
    if (state->write) {
        //ram->setMemory(state->address, state->write_data);
        dcache->setData(clock_cycles, state->address, state->write_data, 4);
    }
    
    if (state->read) {
        uint32_t data = dcache->getData(clock_cycles, state->address, 4);
        if (state->write_float) {
            float fdata = 0;
            //uint32_t data = ram->getMemory(state->address);
            memcpy(&fdata, &data, sizeof(uint32_t));
            setFloatRegister(state->rd, fdata); 
        } else {
            //uint32_t data = ram->getMemory(state->address);
            setRegister(state->rd, data);
        }
    }
}

