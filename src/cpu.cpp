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
    if (state) print_instruction(state->decodeData);
}

//
// The store stage
//
void CPU::store(State *state) {

}

