#ifndef __C8ASM_INSTRUCTION_HPP__
#define __C8ASM_INSTRUCTION_HPP__

#include <stdint.h>

class c8asm_instruction {
public:
    virtual uint8_t byte1() = 0;
    virtual uint8_t byte2() = 0;
};

class c8asm_instruction_bytes : public c8asm_instruction {
public:
    c8asm_instruction_bytes(uint8_t byte1, uint8_t byte2);
    uint8_t byte1();
    uint8_t byte2();
private:
    uint8_t b1, b2; 
};

class c8asm_instruction_label : public c8asm_instruction {
public:
    c8asm_instruction_label(uint8_t nibble, char *label);
    uint8_t byte1();
    uint8_t byte2();
private:
    uint8_t nibble;
    char *label;
};

#endif

