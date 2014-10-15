#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "chip8_display.hpp"

chip8_display::chip8_display() {
    this->w = 64;
    this->h = 32;
    this->memory = (unsigned char *)calloc(w*h, sizeof(unsigned char));
    
    this->cimage = new CImg<unsigned char>(w, h, 1, 1, 0);
    this->cdisplay = new CImgDisplay(*this->cimage, "Chip8");
    this->cdisplay->resize(w * 8, h * 8);
    this->cdisplay->show();
    this->clear();
}

void chip8_display::update() {
    cimg_forXY(*cimage, x, y) {
        (*cimage)(x, y) = this->memory[y*w+x] ? 255 : 0;
    }
    this->cdisplay->resize(false);
    this->cdisplay->display(*cimage);
}

void chip8_display::clear() {
    memset(this->memory, 0, w*h);
    this->update();
}

bool chip8_display::keyPressed(uint8_t key) {
    switch (key) {
        case 0x0: return this->cdisplay->is_key0();
        case 0x1: return this->cdisplay->is_key1();
        case 0x2: return this->cdisplay->is_key2();
        case 0x3: return this->cdisplay->is_key3();
        case 0x4: return this->cdisplay->is_key4();
        case 0x5: return this->cdisplay->is_key5();
        case 0x6: return this->cdisplay->is_key6();
        case 0x7: return this->cdisplay->is_key7();
        case 0x8: return this->cdisplay->is_key8();
        case 0x9: return this->cdisplay->is_key9();
        case 0xA: return this->cdisplay->is_keyA();
        case 0xB: return this->cdisplay->is_keyB();
        case 0xC: return this->cdisplay->is_keyC();
        case 0xD: return this->cdisplay->is_keyD();
        case 0xE: return this->cdisplay->is_keyE();
        case 0xF: return this->cdisplay->is_keyF();
    }
    
    return false;
}

uint8_t chip8_display::waitForKey() {
    while (!this->cdisplay->is_closed()) {
        if (this->cdisplay->is_key0()) return 0x0;
        if (this->cdisplay->is_key1()) return 0x1;
        if (this->cdisplay->is_key2()) return 0x2;
        if (this->cdisplay->is_key3()) return 0x3;
        if (this->cdisplay->is_key4()) return 0x4;
        if (this->cdisplay->is_key5()) return 0x5;
        if (this->cdisplay->is_key6()) return 0x6;
        if (this->cdisplay->is_key7()) return 0x7;
        if (this->cdisplay->is_key8()) return 0x8;
        if (this->cdisplay->is_key9()) return 0x9;
        if (this->cdisplay->is_keyA()) return 0xA;
        if (this->cdisplay->is_keyB()) return 0xB;
        if (this->cdisplay->is_keyC()) return 0xC;
        if (this->cdisplay->is_keyD()) return 0xD;
        if (this->cdisplay->is_keyE()) return 0xE;
        if (this->cdisplay->is_keyF()) return 0xF;
        this->cdisplay->wait();
    }
    return 0;
}

bool chip8_display::drawSprite(unsigned char *addr, uint8_t x, uint8_t y, uint8_t n) {
    uint16_t pos;
    uint8_t bit;
    bool ret = false;

    for (int cy = 0; cy < n; cy++) {
        for (int cx = 0; cx < 8; cx++) {
            pos = (cy+y) * w + (cx+x);
            if (pos >= (w*h)) {
                continue;
            }

            bit = (addr[cy] & (1 << (7-cx))) ? 1 : 0;
            if (this->memory[pos] == 1 && bit == 1) {
                ret = true;
            }
            this->memory[pos] ^= bit;
        }
    }
    this->update();
    return ret;
}

bool chip8_display::isClosed() {
    return this->cdisplay->is_closed();
}

