#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

using namespace std;

vector<uint16_t> labels;

char *label_str(uint16_t addr) {
    char *ret = (char *)malloc(sizeof(char) * 6);
    for (int ll = 0; ll < labels.size(); ll++) {
        if (labels[ll] == addr) {
            sprintf(ret, "LBL%03d", ll);
            return ret;
        }
    }
    sprintf(ret, "      ");
    return ret;
}

void label(unsigned char byte1, unsigned char byte2) {

    unsigned char nib0, nib1, nib2, nib3;
    uint16_t addr;

    nib0 = (byte1 & 0xf0) >> 4;
    nib1 = (byte1 & 0x0f);
    nib2 = (byte2 & 0xf0) >> 4;
    nib3 = (byte2 & 0x0f);

    switch (nib0) {
        case 0x0:
            if (byte1 == 0 && byte2 == 0xE0) {
            } else if (byte1 == 0 && byte2 == 0xEE) {
            } else {
                // 0nnn SYS addr
                addr = (nib1 << 8) | byte2;
                labels.push_back(addr);
                break;
            }
        case 0x1:
            // 1nnn JP addr
            addr = (nib1 << 8) | byte2;
            labels.push_back(addr);
            break;
        case 0x2:
            // 2nnn CALL addr
            addr = (nib1 << 8) | byte2;
            labels.push_back(addr);
            break;
        case 0xA:
            // Annn LD I, addr
            addr = (nib1 << 8) | byte2;
            labels.push_back(addr);
            break;
        case 0xB:
            // Bnnn JP V0, addr
            addr = ((nib1 << 8) | byte2);
            labels.push_back(addr);
            break;
    }
}

char *instr(unsigned char byte1, unsigned char byte2) {
    char *ret = (char *)malloc(sizeof(char) * 30);

    unsigned char nib0, nib1, nib2, nib3;
    uint16_t addr;

    nib0 = (byte1 & 0xf0) >> 4;
    nib1 = (byte1 & 0x0f);
    nib2 = (byte2 & 0xf0) >> 4;
    nib3 = (byte2 & 0x0f);

    switch (nib0) {
        case 0x0:
            if (byte1 == 0 && byte2 == 0xE0) {
                // 00E0 CLS
                sprintf(ret, "CLS");
                break;
            } else if (byte1 == 0 && byte2 == 0xEE) {
                // 00EE RET
                sprintf(ret, "RET");
                break;
            } else {
                // 0nnn SYS addr
                addr = (nib1 << 8) | byte2;
                sprintf(ret, "SYS  %s", label_str(addr));
                break;
            }
        case 0x1:
            // 1nnn JP addr
            addr = (nib1 << 8) | byte2;
            sprintf(ret, "JP   %s", label_str(addr));
            break;
        case 0x2:
            // 2nnn CALL addr
            addr = (nib1 << 8) | byte2;
            sprintf(ret, "CALL %s", label_str(addr));
            break;
        case 0x3:
            // 3xkk SE Vx, byte
            sprintf(ret, "SE   v%1x, %02x", nib1, byte2);
            break;
        case 0x4:
            // 4xkk SNE Vx, byte
            sprintf(ret, "SNE  v%1x, %02x", nib1, byte2);
            break;
        case 0x5:
            // 5xy0 SE Vx, Vy
            sprintf(ret, "SE   v%1x, v%1x", nib1, nib2);
            break;
        case 0x6:
            // 6xkk LD Vx, byte
            sprintf(ret, "LD   v%1x, %02x", nib1, byte2);
            break;
        case 0x7:
            // 7xkk ADD Vx, byte
            sprintf(ret, "ADD  v%1x, %02x", nib1, byte2);
            break;
        case 0x8:
            switch (nib3) {
                case 0x0:
                    // 8xy0 LD Vx, Vy
                    sprintf(ret, "LD   v%1x, v%1x", nib1, nib2);
                    break;
                case 0x1:
                    // 8xy1 OR Vx, Vy
                    sprintf(ret, "OR   v%1x, v%1x", nib1, nib2);
                    break;
                case 0x2:
                    // 8xy2 AND Vx, Vy
                    sprintf(ret, "AND  v%1x, v%1x", nib1, nib2);
                    break;
                case 0x3:
                    sprintf(ret, "XOR  v%1x, v%1x", nib1, nib2);
                    break;
                case 0x4:
                    // 8xy4 ADD Vx, Vy
                    sprintf(ret, "ADC  v%1x, v%1x", nib1, nib2);
                    break;
                case 0x5:
                    // 8xy5 SUB Vx, Vy
                    sprintf(ret, "SUBC v%1x, v%1x", nib1, nib2);
                    break;
                case 0x6:
                    // 8xy6 SHR Vx {, Vy}
                    sprintf(ret, "SHR  v%1x", nib1);
                    break;
                case 0x7:
                    // 8xy7 SUBN Vx, Vy
                    sprintf(ret, "SUBN v%1x, v%1x", nib1, nib2);
                    break;
                case 0xE:
                    // 8xyE SHL Vx {, Vy}
                    sprintf(ret, "SHL  v%1x", nib1);
                    break;
            }
            break;
        case 0x9:
            // 9xy0 SNE Vx, Vy
            sprintf(ret, "SNE  v%1x, v%1x", nib1, nib2);
            break;
        case 0xA:
            // Annn LD I, addr
            addr = (nib1 << 8) | byte2;
            sprintf(ret, "LD   I, %s", label_str(addr));
            break;
        case 0xB:
            // Bnnn JP V0, addr
            addr = ((nib1 << 8) | byte2);
            sprintf(ret, "JP   v0, %s", label_str(addr));
            break;
        case 0xC:
            // Cxkk RND Vx, byte
            sprintf(ret, "RND  v%1x, %02x", nib1, byte2);
            break;
        case 0xD:
            // Dxyn DRW Vx, Vy, nibble
            sprintf(ret, "DRW  v%1x, v%1x, %1x", nib1, nib2, nib3);
            break;
        case 0xE:
            switch (byte2) {
                case 0x9E:
                    // Ex9E SKP Vx
                    sprintf(ret, "SKP  v%1x", nib1);
                    break;
                case 0xA1:
                    // ExA1 SKNP Vx
                    sprintf(ret, "SKNP v%1x", nib1);
                    break;
            }
            break;
        case 0xF:
            switch (byte2) {
                case 0x07:
                    // Fx07 LD Vx, DT
                    sprintf(ret, "LD   v%1x, DT", nib1);
                    break;
                case 0x0A:
                    // Fx0A LD Vx, K
                    sprintf(ret, "LD   v%1x, K", nib1);
                    break;
                case 0x15:
                    // Fx15 LD DT, Vx
                    sprintf(ret, "LD   DT, v%1x", nib1);
                    break;
                case 0x18:
                    // Fx18 LD ST, Vx
                    sprintf(ret, "LD   ST, v%1x", nib1);
                    break;
                case 0x1E:
                    // Fx1E ADD I, Vx
                    sprintf(ret, "ADD  I, v%1x", nib1);
                    break;
                case 0x29:
                    // Fx29 LD F, Vx
                    sprintf(ret, "LD   F, v%1x", nib1);
                    break;
                case 0x33:
                    // Fx33 - LD B, Vx
                    sprintf(ret, "LD   B, v%1x", nib1);
                    break;
                case 0x55:
                    // Fx55 LD [I], Vx
                    sprintf(ret, "LD   [I], v%1x", nib1);
                    break;
                case 0x65:
                    // Fx65 LD Vx, [I]
                    sprintf(ret, "LD   v%1x, [I]", nib1);
                    break;
            }
            break;
    }
    return ret;
}

int main(int argc, char **argv) {

    if (argc == 1) {
        printf("usage: %s <rom>\n", argv[0]);
        return 1;
    }   

    char *rom = argv[1];
    unsigned char *memory = (unsigned char *)malloc(0xfff);

    printf("Opening rom: %s... ", rom);
    FILE *fp = fopen(rom, "rb");
    if (!fp) {
        printf("fail!\n");
        return false;
    }
    size_t rom_size = fread(&memory[0x200], 1, 0xff, fp);
    fclose(fp);

    printf("ok\n\n");

    // find all labels (pass 1)
    for (size_t b = 0; b < rom_size; b += 2) {
        uint16_t addr = b + 0x200;
        uint8_t byte1 = memory[addr];
        uint8_t byte2 = memory[addr+1];
        label(byte1, byte2);
    }

    // print code (pass 2)
    for (size_t b = 0; b < rom_size; b += 2) {
        uint16_t addr = b + 0x200;
        uint8_t byte1 = memory[addr];
        uint8_t byte2 = memory[addr+1];

        printf("%02x %02x   ; [%03x] %6s  %s\n", byte1, byte2, addr, label_str(addr), instr(byte1, byte2));
    }

    return 0;
}

