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
    string opcode;
    int rd, rs1, rs2;
    string func3;
    string func7;
    unsigned int imm;
    unsigned int long_imm;
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

Data *decode(string instr){
    Data *data = new Data;
    
    // opcode
    string opcode = "";
    for (int i = 0; i<=6; i++) opcode += instr[i];
    
    // rd
    string rd = "";
    for (int i = 7; i<=11; i++) rd += instr[i];
    
    // funct3
    string func3 = "";
    for (int i = 12; i<=14; i++) func3 += instr[i];
    
    // rs1
    string rs1 = "";
    for (int i = 15; i<=19; i++) rs1 += instr[i];
    
    // rs2
    string rs2 = "";
    for (int i = 20; i<=24; i++) rs2 += instr[i];
    
    // func7
    string func7 = "";
    for (int i = 25; i<=31; i++) func7 += instr[i];
    
    // imm
    string imm = "";
    for (int i = 20; i<=31; i++) imm += instr[i];
    
    // long imm
    string long_imm = "";
    for (int i = 12; i<=31; i++) long_imm += instr[i];
    
    // Build the structure
    data->opcode = opcode;
    data->func3 = func3;
    data->func7 = func7;
    data->rd = stoi(rd);
    data->rs1 = stoi(rs1);
    data->rs2 = stoi(rs2);
    data->imm = stoul(imm, nullptr, 2);
    data->long_imm = stoul(imm, nullptr, 2);
    
    return data;
}

//
// Handles all ALU operations
//
void alu(Data *data, CPU *cpu, bool isImmediate) {
    int val1 = cpu->getRegister(data->rs1);
    int val2 = 0;
    if (isImmediate) val2 = data->imm;
    else val2 = cpu->getRegister(data->rs2);

    // Add and sub (sub determined by func7)
    if (data->func3 == "000") {
        int result = val1 + val2;
        cpu->setRegister(data->rd, result);
        
    // Shift left
    } else if (data->func3 == "001") {
    
    // SLT (set less than)
    } else if (data->func3 == "010") {
    
    // SLTU (set less than unsigned)
    } else if (data->func3 == "011") {
    
    // XOR
    } else if (data->func3 == "100") {
    
    // SRL/SRA (shift right logic/arithmetic)
    // Logic vs arithmetic determined by func7
    } else if (data->func3 == "101") {
    
    // OR
    } else if (data->func3 == "110") {
    
    // AND
    } else if (data->func3 == "111") {
    
    }
}

//
// Handles all branching operations
//
void branch(Data *data, CPU *cpu) {
    // BEQ
    if (data->func3 == "000") {
    
    // BNE
    } else if (data->func3 == "001") {
    
    // BLT
    } else if (data->func3 == "100") {
    
    // BGE
    } else if (data->func3 == "101") {
    
    // BLTU
    } else if (data->func3 == "110") {
    
    // BGEU
    } else if (data->func3 == "111") {
    
    }
}

void execute(Data *data, CPU *cpu) {
    
    //
    // First, determine the opcode
    //
    // ALU instruction, R-Type and I-Type
    if (data->opcode == "0010011" || data->opcode == "0110011") {
        if (data->opcode == "0110011") {
            alu(data, cpu, false);
        } else {
            alu(data, cpu, true);
        }
    
    // LUI (load upper immediate), AUIPC, JAL
    } else if (data->opcode == "0110111") {
    
    // All the branch instructions
    } else if (data->opcode == "1100011") {
        branch(data, cpu);
    
    // All the load instructions
    } else if (data->opcode == "0000011") {
    
        
    // All the store instructions
    } else if (data->opcode == "0100011") {
    
    
    // Otherwise, invalid opcode
    } else {
        cout << "Error: Invalid Opcode." << std::endl;
        return;
    }
}

int main() {
    string instr1 = "";
    string instr2 = "";
    /*instr1 += "0000000";
    instr1 += "00000";
    instr1 += "00000";
    instr1 += "000";
    instr1 += "00001";
    instr1 += "0110011";*/    // ADD X1, X0, X0
    
    // ADD X1, X0, X0
    instr1 += "0110011";
    instr1 += "00001";
    instr1 += "000";
    instr1 += "00000";
    instr1 += "00000";
    instr1 += "0000000";
    
    instr2 += "0010011";         // ADDI X2, X0, 10
    instr2 += "00010";
    instr2 += "000";
    instr2 += "00000";
    instr2 += "000000001010";
    
    Data *d1 = decode(instr1);
    std::cout << "OP: " << d1->opcode << " | func3: " << d1->func3 << " | func7: " << d1->func7;
    std::cout << " | rd: " << d1->rd << " | rs1: " << d1->rs1 << " | rs2: " << d1->rs2 << " | imm: " << d1->imm << std::endl;
    
    Data *d2 = decode(instr2);
    std::cout << "OP: " << d2->opcode << " | func3: " << d2->func3 << " | func7: " << d2->func7;
    std::cout << " | rd: " << d2->rd << " | rs1: " << d2->rs1 << " | rs2: " << d2->rs2 << " | imm: " << d2->imm << std::endl;
    
    // Execute and test
    CPU *cpu = new CPU;
    execute(d1, cpu);
    execute(d2, cpu);
    
    cpu->debugRegisters();
    
    return 0;
}

