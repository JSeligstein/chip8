#ifndef __CHIP8_HPP__
#define __CHIP8_HPP__

#include <stdint.h>
#include <thread>
#include "chip8_display.hpp"

#define VX_FLAG 15
#define C8_FONT 0x000

using namespace std;

class chip8 {
public:
    char *filename;

    chip8();
    bool loadRom(char *filename);
    void run();

private:
    uint8_t vx[16];  // v0-vf
    uint16_t ri;     // I register
    uint16_t pc;     // program counter
    uint8_t sp;      // stack pointer
    uint8_t dt;      // delay timer
    uint8_t st;      // sound timer

    // lazy locks
    bool dt_lock;
    bool st_lock;
    bool sound_started;

    unsigned char *memory;
    uint16_t *stack;
    chip8_display *display;
    thread threads[2];

    void cycle();
    void delay_thread();
    void sound_thread();

};

#endif 

