#ifndef __C8ASM_HPP__
#define __C8ASM_HPP__

#include <stdint.h>
#include <vector>
#include <map>
#include "c8asm_instruction.hpp"

using namespace std;

class c8asm {
public: 
    static c8asm &inst();

    c8asm();
    void markLabel(char *label);
    void markLabel(char *label, int8_t offset);
    void addInstruction(uint8_t nib1, uint8_t nib2, uint8_t nib3, uint8_t nib4); 
    void addInstruction(uint8_t nib1, uint8_t nib2, uint8_t byte2);
    void addInstruction(uint8_t byte1, uint8_t byte2);
    void addInstructionWithLabel(uint8_t nibble, char *label);
    uint16_t addrFromLabel(char *label);

    void dumpLabels();
    void dumpBytes(char *filename);
    void dumpReadableBytes();

    uint16_t pc;

private:
    map<string, uint16_t> labels;
    vector<c8asm_instruction *> instructions;
};

#endif

