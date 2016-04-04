#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "chip8.hpp"
#include <string.h>
#include <thread>
#include <functional>
#include "portaudio.h"

// game delay
#define C8_CYCLE_DELAY (150)

// take off _X to get opcode output
#define C8_CYCLE_DEBUG_X
#define C8_CYCLE_DEBUG_USLEEP (10 * 1000)

#define DEBUG_PRINT \
    printf("cycle [%03x] %02x %02x", pc, byte1, byte2); \
    printf(", ri: %03x, dt: %02x, sp: %02x", ri, dt, sp); \
    printf(", vx: "); \
    for(int vv = 0; vv < 16; vv++) { \
        printf("%02x ", vx[vv]); \
    } \
    printf("\n");

// for portaudio
#define SAMPLE_RATE (44100)

const unsigned char chip8_font[5 * 16] = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80,
};

chip8::chip8() {
    this->display = new chip8_display();
}

bool chip8::loadRom(char *filename) {
    this->filename = NULL;
    this->memory = (unsigned char *)malloc(0xfff);

    printf("Opening rom: %s... ", filename);
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("fail!\n");
        return false;
    }
    fread(&memory[0x200], 1, 0xfff, fp);
    fclose(fp);

    this->filename = filename;
    printf("ok\n");

    memcpy(&memory[C8_FONT], chip8_font, 16*5);

    return true;
}

void chip8::cycle() {
    unsigned char byte1, byte2;
    unsigned char nib0, nib1, nib2, nib3;
    uint16_t addr;
    uint16_t res;
    uint8_t key;

    byte1 = memory[pc];
    byte2 = memory[pc+1];
    nib0 = (byte1 & 0xf0) >> 4;
    nib1 = (byte1 & 0x0f);
    nib2 = (byte2 & 0xf0) >> 4;
    nib3 = (byte2 & 0x0f);

#ifdef C8_CYCLE_DEBUG
    DEBUG_PRINT
#ifdef C8_CYCLE_DEBUG_USLEEP
    usleep(C8_CYCLE_DEBUG_USLEEP);
#endif
#endif

    pc += 2;
    bool executed = false;

    switch (nib0) {
        case 0x0:
            if (byte1 == 0 && byte2 == 0xE0) {
                // 00E0 CLS
                this->display->clear();
                executed = true;
            } else if (byte1 == 0 && byte2 == 0xEE) {
                // 00EE RET
                pc = stack[sp];
                sp--;
                executed = true;
            } else if (byte1 == 0 && byte2 == 0xFF) {
                // 00FF PRINT
                // defined by me
                DEBUG_PRINT
                executed = true;
            } else {
                // 0nnn SYS addr
                // ignore for now
                executed = true;
            }
            break;
        case 0x1:
            // 1nnn JP addr
            pc = (nib1 << 8) | byte2;
            executed = true;
            break;
            
        case 0x2:
            // 2nnn CALL addr
            sp++;
            stack[sp] = pc;
            pc = (nib1 << 8) | byte2;
            executed = true;
            break;

        case 0x3:
            // 3xkk SE Vx, byte
            if (vx[nib1] == byte2) {
                pc += 2;
            }
            executed = true;
            break;

        case 0x4:
            // 4xkk SNE Vx, byte
            if (vx[nib1] != byte2) {
                pc += 2;
            }
            executed = true;
            break;

        case 0x5:
            // 5xy0 SE Vx, Vy
            if (vx[nib1] == vx[nib2]) {
                pc += 2;
            }
            executed = true;
            break;
            
        case 0x6:
            // 6xkk LD Vx, byte
            vx[nib1] = byte2;
            executed = true;
            break;

        case 0x7:
            // 7xkk ADD Vx, byte
            vx[nib1] += byte2;
            executed = true;
            break;

        case 0x8:
            switch (nib3) {
                case 0x0:
                    // 8xy0 LD Vx, Vy
                    vx[nib1] = vx[nib2];
                    executed = true;
                    break;
                case 0x1:
                    // 8xy1 OR Vx, Vy
                    vx[nib1] |= vx[nib2];
                    executed = true;
                    break;
                case 0x2:
                    // 8xy2 AND Vx, Vy
                    vx[nib1] &= vx[nib2];
                    executed = true;
                    break;
                case 0x3:
                    // 8xy3 XOR Vx, Vy
                    vx[nib1] ^= vx[nib2];
                    executed = true;
                    break;
                case 0x4:
                    // 8xy4 ADD Vx, Vy
                    res = vx[nib1] + vx[nib2];
                    if (res > 255) {
                        vx[VX_FLAG] = 1;
                    }
                    vx[nib1] = (uint8_t)(res & 0xff);
                    executed = true;
                    break;
                case 0x5:
                    // 8xy5 SUB Vx, Vy
                    vx[VX_FLAG] = vx[nib1] > vx[nib2];
                    vx[nib1] -= vx[nib2];
                    executed = true;
                    break;
                case 0x6:
                    // 8xy6 SHR Vx {, Vy}
                    vx[VX_FLAG] = vx[nib1] & 0x1;
                    vx[nib1] >>= 1;
                    executed = true;
                    break;
                case 0x7:
                    // 8xy7 SUBN Vx, Vy
                    vx[VX_FLAG] = vx[nib2] > vx[nib1];
                    vx[nib1] = vx[nib2] - vx[nib1];
                    executed = true;
                    break;
                case 0xE:
                    // 8xyE SHL Vx {, Vy}
                    vx[VX_FLAG] = (vx[nib1] & 0x80) ? 1 : 0;
                    vx[nib1] <<= 1;
                    executed = true;
                    break;
            }
            break;

        case 0x9:
            // 9xy0 SNE Vx, Vy
            if (vx[nib1] != vx[nib2]) {
                pc += 2;
            }
            executed = true;
            break;

        case 0xA:
            // Annn LD I, addr
            ri = (nib1 << 8) | byte2;
            executed = true;
            break;

        case 0xB:
            // Bnnn JP V0, addr
            pc = ((nib1 << 8) | byte2) + vx[0];
            executed = true;
            break;

        case 0xC:
            // Cxkk RND Vx, byte
            // better rand?
            res = rand() % 255;
            vx[nib1] = res & byte2;
            executed = true;
            break;

        case 0xD:
            // Dxyn DRW Vx, Vy, nibble
            vx[VX_FLAG] = this->display->drawSprite(&memory[ri], vx[nib1], vx[nib2], nib3);
            executed = true;
            break;

        case 0xE:
            switch (byte2) {
                case 0x9E:
                    // Ex9E SKP Vx
                    if (display->keyPressed(vx[nib1])) {
                        pc += 2;
                    }
                    executed = true;
                    break;
                case 0xA1:
                    // ExA1 SKNP Vx
                    if (!display->keyPressed(vx[nib1])) {
                        pc += 2;
                    }
                    executed = true;
                    break;
            }
            break;

        case 0xF:
            switch (byte2) {
                case 0x07:
                    // Fx07 LD Vx, DT
                    vx[nib1] = dt;
                    executed = true;
                    break;
                case 0x0A:
                    // Fx0A LD Vx, K
                    key = display->waitForKey();
                    vx[nib1] = key;
                    executed = true;
                    break;
                case 0x15:
                    // Fx15 LD DT, Vx
                    while (dt_lock);
                    dt_lock = 1;
                    dt = vx[nib1];
                    dt_lock = 0;
                    executed = true;
                    break;
                case 0x18:
                    // Fx18 LD ST, Vx
                    while (st_lock);
                    st_lock = 1;
                    st = vx[nib1];
                    st_lock = 0;
                    executed = true;
                    break;
                case 0x1E:
                    // Fx1E ADD I, Vx
                    ri += vx[nib1];
                    executed = true;
                    break;
                case 0x29:
                    // Fx29 LD F, Vx
                    ri = C8_FONT + vx[nib1] * 5;
                    executed = true;
                    break;
                case 0x33:
                    // Fx33 - LD B, Vx
                    memory[ri] = (vx[nib1] / 100) % 10;
                    memory[ri+1] = (vx[nib1] / 10) % 10;
                    memory[ri+2] = vx[nib1] % 10;
                    executed = true;
                    break;
                case 0x55:
                    // Fx55 LD [I], Vx
                    for (int curvx = 0; curvx <= nib1; curvx++) {
                        memory[ri+curvx] = vx[curvx];
                    }
                    executed = true;
                    break;
                case 0x65:
                    // Fx65 LD Vx, [I]
                    for (int curvx = 0; curvx <= nib1; curvx++) {
                        vx[curvx] = memory[ri+curvx];
                    }
                    executed = true;
                    break;
            }
            break;
    }

    if (!executed) {
        printf("Unknown instruction [%x]: %x %x\n", pc-2, byte1, byte2);
        exit(1);
    }
}

void chip8::run() {
    pc = 0x200;
    sp = -1;
    ri = 0;
    dt = 0;
    st = 0;
    this->stack = (uint16_t *)calloc(16, sizeof(uint16_t));
    for (int vv = 0; vv < 16; vv++) {
        vx[vv] = 0;
    }

    dt_lock = 0;
    st_lock = 0;

    this->threads[0] = thread(bind(&chip8::delay_thread, this));
    this->threads[1] = thread(bind(&chip8::sound_thread, this));

    while (1) {
        cycle();
        usleep(C8_CYCLE_DELAY);
    }
}

void chip8::delay_thread() {
    while (!this->display->isClosed()) {
        usleep(1000000 / 60);
        if (dt) {
            while (dt_lock);
            dt_lock = 1;
            dt--;
            dt_lock = 0;
        }
    }
}

typedef struct
{
    float left_phase;
    float right_phase;
}   

paTestData;
/* This routine will be called by the PortAudio engine when audio is needed.
   It may called at interrupt level on some machines so don't do anything
   that could mess up the system like calling malloc() or free().
*/ 
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    /* Cast data passed through stream to our structure. */
    paTestData *data = (paTestData*)userData; 
    float *out = (float*)outputBuffer;
    unsigned int i;
    (void) inputBuffer; /* Prevent unused variable warning. */
    
    for( i=0; i<framesPerBuffer; i++ )
    {
        *out++ = data->left_phase;  /* left */
        *out++ = data->right_phase;  /* right */
        /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
        data->left_phase += 0.01f;
        /* When signal reaches top, drop back down. */
        if( data->left_phase >= 1.0f ) data->left_phase -= 2.0f;
        /* higher pitch so we can distinguish left and right. */
        data->right_phase += 0.03f;
        if( data->right_phase >= 1.0f ) data->right_phase -= 2.0f;
    }
    return 0;
}
void chip8::sound_thread() {
    static paTestData data;

    sound_started = false;

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        printf("sound thread failed to start\n");
        return;
    }

    PaStream *stream;
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                2,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                SAMPLE_RATE,
                                256,        /* frames per buffer, i.e. the number
                                                   of sample frames that PortAudio will
                                                   request from the callback. Many apps
                                                   may want to use
                                                   paFramesPerBufferUnspecified, which
                                                   tells PortAudio to pick the best,
                                                   possibly changing, buffer size.*/
                                patestCallback, /* this is your callback function */
                                &data ); /*This is a pointer that will be passed to
                                                   your callback*/
    if (err != paNoError) {
        printf("sound thread failed to open\n");
        return;
    }

    while (!this->display->isClosed()) {
        usleep(1000000 / 60);
        if (st) {
            if (!sound_started) {
                err = Pa_StartStream(stream);
                if (err != paNoError) {
                    printf("error opening sound stream\n");
                    return;
                }
                sound_started = true;
            }

            while (st_lock);
            st_lock = 1;
            st--;
            st_lock = 0;

            if (st == 0 && sound_started) {
                err = Pa_StopStream(stream);
                if (err != paNoError) {
                    printf("error closing sound stream\n");
                    return;
                }
                sound_started = false;
            }
        }
    }

    if (sound_started) {
        err = Pa_StopStream(stream);
        if (err != paNoError) {
            printf("error closing sound stream\n");
        } else {
            sound_started = false;
        }
    }

    err = Pa_Terminate();
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
    }
}



