//
// Created by soda on 1/27/24.
//

#ifndef  _TEXTSFX_H
#define  _TEXTSFX_H

#include <sifteo.h>
#include "fontdata.h"

using namespace Sifteo;

void fadeInAndOut(Colormap*);

RGB565 makeColor(uint8_t);

void initDrawing(VideoBuffer*);

struct TextRenderer {
    FB128Drawable &fb;
    UByte2 position;

    TextRenderer(FB128Drawable &fb) : fb(fb) {}

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
void typeText(const char*, TextRenderer, Vector2<int>, const AssetAudio&, unsigned textUpdateDelay=2, unsigned charRate=2, bool drawCentered=false);

void typeLines(const char **lines, unsigned numLines, TextRenderer tr, Vector2<int> location, const AssetAudio& sfx, unsigned textUpdateDelay, unsigned charRate, bool drawCentered);

#endif
