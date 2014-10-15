#ifndef __C8_DISPLAY_HPP__
#define __C8_DISPLAY_HPP__

#include <stdint.h>
#include "CImg.h"

using namespace cimg_library;
using namespace std;

class chip8_display {
public:
    chip8_display();
    void clear();
    bool keyPressed(uint8_t key);
    uint8_t waitForKey();
    bool drawSprite(unsigned char *addr, uint8_t x, uint8_t y, uint8_t h);
    bool isClosed();

private:
    unsigned char *memory;
    uint16_t w;
    uint16_t h;
    CImg<unsigned char> *cimage;
    CImgDisplay *cdisplay;

    void update();

};

#endif 
