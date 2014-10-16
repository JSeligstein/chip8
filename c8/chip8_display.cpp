#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "chip8_display.hpp"

/**
 * Keys are in 0-F indexes, but will be laid out in this fashion:
 *
 *  1 2 3 C
 *  4 5 6 D
 *  7 8 9 E
 *  A 0 B F
 *
 */

const unsigned int chip8_display_keymap[16] = {
    cimg::keyB, cimg::key4, cimg::key5, cimg::key6,
    cimg::keyR, cimg::keyT, cimg::keyY, cimg::keyF,
    cimg::keyG, cimg::keyH, cimg::keyV, cimg::keyN,
    cimg::key7, cimg::keyU, cimg::keyJ, cimg::keyM,
};

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
    if (key >= 0 && key <= 15) {
        return this->cdisplay->is_key(chip8_display_keymap[key]);
    }
    return false;
}

uint8_t chip8_display::waitForKey() {
    while (!this->cdisplay->is_closed()) {
        for (int kk = 0; kk < 16; kk++) {
            if (this->cdisplay->is_key(chip8_display_keymap[kk])) {
                return kk;
            }
        }
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

            // if we wrapped around, don't drop down a line
            pos = (cy+y) * w + ((cx+x) % w);

            // if off the screen, forget it!
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

