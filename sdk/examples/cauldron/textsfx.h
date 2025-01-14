//
// Created by soda on 1/27/24.
//

#ifndef  _TEXTSFX_H
#define  _TEXTSFX_H

#include <sifteo.h>
#include "fontdata.h"
#define MAX_LINE_CHAR 26

using namespace Sifteo;

void fadeOut(Colormap *cm[], const unsigned cmsCount, const int speed, const unsigned hold);

RGB565 makeColor(uint8_t);

void solidBg(VideoBuffer*, unsigned id);
void initLetterbox(VideoBuffer*);

struct TextRenderer {
    FB128Drawable* fb;
    UByte2 position;

//    TextRenderer(FB128Drawable* fb) : fb(fb) {}

    void drawGlyph(char);
    void drawText(const char*);
    void drawCentered(const char*);
    static unsigned measureGlyph(char);
    static unsigned measureText(const char*);
};

void playSfx(const AssetAudio&);

// str will print out character by character, limit of 64 chars
// location is a vector<unsigned> of coords indicating where to place the text
// charRate is the number of chars to print at a time
// if drawCentered is true, it will ignore location.x
void typeText(const char*, TextRenderer*[], const unsigned, Vector2<int>, const AssetAudio&, unsigned textUpdateDelay=2, unsigned charRate=2, bool drawCentered=false);

void typeLines(const String<MAX_LINE_CHAR> lines[], const unsigned numLines, TextRenderer trs[], const unsigned trsCount, Vector2<int> location, const AssetAudio& sfx, unsigned textUpdateDelay, unsigned charRate, bool drawCentered);

#endif
