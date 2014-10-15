#include "c8asm_instruction.hpp"
#include "c8asm.hpp"

c8asm_instruction_byte::c8asm_instruction_byte(uint8_t byte1) {
    b1 = byte1;
}

bool c8asm_instruction_byte::hasSecondByte() {
    return false;
}

uint8_t c8asm_instruction_byte::byte1() {
    return b1;
}

uint8_t c8asm_instruction_byte::byte2() {
    printf("called byte2 on a single byte instruction\n");
    return 0;
}

c8asm_instruction_bytes::c8asm_instruction_bytes(uint8_t byte1, uint8_t byte2) { 
    b1 = byte1;
    b2 = byte2;
}

bool c8asm_instruction_bytes::hasSecondByte() {
    return true;
}

uint8_t c8asm_instruction_bytes::byte1() {
    return b1;
}

uint8_t c8asm_instruction_bytes::byte2() {
    return b2;
}

c8asm_instruction_label::c8asm_instruction_label(uint8_t nibble, char *label) {
    this->nibble = nibble;
    this->label = label;
}

bool c8asm_instruction_label::hasSecondByte() {
    return true;
}

uint8_t c8asm_instruction_label::byte1() {
    uint16_t addr = c8asm::inst().addrFromLabel(label);
    return (uint8_t) ((nibble << 4) | ((addr & 0xf00) >> 8));
}

uint8_t c8asm_instruction_label::byte2() {
    uint16_t addr = c8asm::inst().addrFromLabel(label);
    return (uint8_t) (addr & 0xff);
}

