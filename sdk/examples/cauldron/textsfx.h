//
// Created by soda on 1/27/24.
//

#ifndef  _TEXTSFX_H
#define  _TEXTSFX_H

#include <sifteo.h>
#include "fontdata.h"

using namespace Sifteo;

static void playSfx(const AssetAudio&);

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

// str will print out character by character, limit of 64 chars
// location is a vector<unsigned> of coords indicating where to place the text
// charRate is the number of chars to print at a time
void typeText(TextRenderer, String<128>, Vector2<int>, const AssetAudio&, unsigned textUpdateDelay=2, unsigned charRate=2);

static RGB565 makeColor(uint8_t);

static void fadeInAndOut(Colormap);

void initDrawing(VideoBuffer);

void onRefresh(void*, unsigned cube);

#endif
