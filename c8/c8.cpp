#include <stdio.h>
#include "chip8.hpp"

int main(int argc, char **argv) {

    if (argc == 1) {
        printf("usage: %s <rom>\n", argv[0]);
        return 1;
    }

    char *rom = argv[1];
    chip8 *c8 = new chip8();
    c8->loadRom(rom);
    c8->run();
    
    return 0;
}



