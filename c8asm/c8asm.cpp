#include "c8asm.hpp"
#include "c8asm_instruction.hpp"
#include <stdlib.h>
#include <map>
#include <string>

c8asm::c8asm() {
    pc = 0;
}

void c8asm::markLabel(char *label) {
    this->markLabel(label, 0);
}

void c8asm::markLabel(char *label, int8_t offset) {
    std::string slabel(label);
    labels[slabel] = pc+offset;
}

void c8asm::dumpLabels() {
    map<string, uint16_t>::iterator iter;
    printf("Labels: \n");
    for (iter = labels.begin(); iter != labels.end(); ++iter) {
        printf("  [%03x] <%s>\n", iter->second, iter->first.c_str());
    }
}

void c8asm::dumpBytes(char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("could not open %s\n", filename);
        return;
    }

    for (int ii = 0; ii < instructions.size(); ++ii) {
        c8asm_instruction *inst = instructions[ii];
        uint8_t b1 = inst->byte1();
        uint8_t b2 = inst->byte2();
        fwrite(&b1, sizeof(uint8_t), 1, fp);
        fwrite(&b2, sizeof(uint8_t), 1, fp);
    }
}

void c8asm::dumpReadableBytes() {
    uint16_t addr = 0;
    for (int ii = 0; ii < instructions.size(); ++ii) {
        c8asm_instruction *inst = instructions[ii];
        printf("%02x %02x ; [%04x]\n", inst->byte1(), inst->byte2(), addr);
        addr += 2;
    }
}

uint16_t c8asm::addrFromLabel(char *label) {
    std::string slabel(label);
    if (labels.find(slabel) == labels.end()) {
        printf("missing label: [%s]\n", label);
        exit(1);
    }

    uint16_t addr = labels[slabel];

    return labels[slabel]+0x200;
}

void c8asm::addInstruction(uint8_t nib1, uint8_t nib2, uint8_t nib3, uint8_t nib4) {
    c8asm_instruction_bytes *ib = new c8asm_instruction_bytes(
        (nib1 << 4) | nib2, (nib3 << 4) | nib4);
    instructions.push_back(ib);
    pc += 2;
}

void c8asm::addInstruction(uint8_t nib1, uint8_t nib2, uint8_t byte2) {
    c8asm_instruction_bytes *ib = new c8asm_instruction_bytes(
        (nib1 << 4) | nib2, byte2);
    instructions.push_back(ib);
    pc += 2;
}

void c8asm::addInstruction(uint8_t byte1, uint8_t byte2) {
    c8asm_instruction_bytes *ib = new c8asm_instruction_bytes(byte1, byte2);
    instructions.push_back(ib);
    pc += 2;
}

void c8asm::addInstructionWithLabel(uint8_t nibble, char *label) {
    c8asm_instruction_label *il = new c8asm_instruction_label(nibble, label);
    instructions.push_back(il);
    pc += 2;
}

c8asm &c8asm::inst() {
    static c8asm instance;
    return instance;
}

