//
// Created by soda on 1/27/24.
//

#include "textsfx.h"

using namespace Sifteo;

void fadeOut(Colormap *cm, const unsigned speed, const unsigned hold)
{
    LOG(("~ FADE ~\n"));


    for (unsigned i = 0; i < hold; i++)
        System::paint();

    for (unsigned i = 0; i < 0x100; i += speed) {
        (*cm)[1] = makeColor(255 - i);
        System::paint();
    }

    for (unsigned i = 0; i < hold/3; i++)
        System::paint();
}

void TextRenderer::drawGlyph(char ch) {
    // Specifics of our font format
    uint8_t index = ch - ' ';
    const uint8_t *data = font_data + (index << 3) + index;
    uint8_t escapement = *(data++);
    const Int2 size = {8, 8};

    fb.bitmap(position, size, data, 1);
    position.x += escapement;
}

unsigned TextRenderer::measureGlyph(char ch) {
    uint8_t index = ch - ' ';
    const uint8_t *data = font_data + (index << 3) + index;
    return data[0];
}

void TextRenderer::drawText(const char *str) {
    LOG("Drawing text: \"%s\" at (%d, %d)\n",
        str, position.x, position.y);

    char c;
    while ((c = *str)) {
        str++;
        drawGlyph(c);
    }
}

unsigned TextRenderer::measureText(const char *str) {
    unsigned width = 0;
    char c;
    while ((c = *str)) {
        str++;
        width += measureGlyph(c);
    }
    return width;
}

void TextRenderer::drawCentered(const char *str) {
    position.x = (LCD_width - measureText(str)) / 2;
    drawText(str);
}

void playSfx(const AssetAudio& sfx) {
    static int i=0;
    AudioChannel(i).play(sfx);
    i = 1 - i;
}


// str will print out character by character, limit of 64 chars
// location is a vector<unsigned> of coords indicating where to place the text
// charRate is the number of chars to print at a time
void typeText(const char *str, TextRenderer tr, Vector2<int> location, const AssetAudio& sfx, unsigned textUpdateDelay, unsigned charRate, bool drawCentered) {
    LOG("Typing out '%s'\n", str);
    String<128> tempStr;

    tr.position.x = location.x;
    tr.position.y = location.y;

    int endIndex = 0; // index of str to print to
    int count = 0;
    while (str[endIndex] != '\0') {
        tr.position.x = location.x;
        if (count == 0 || count % (textUpdateDelay*1000) == 0) {
            LOG("Writing '%s'\n", tempStr.c_str());
            endIndex += charRate;
            for (int i = 0; i < endIndex; i++) {
                tempStr[i] = str[i];
            }
            tempStr[endIndex] = '\0';
            if (drawCentered) {
                tr.drawCentered(tempStr.c_str());
                tr.position.x = location.x;
            }
            else tr.drawText(tempStr.c_str());
            System::paint();

            playSfx(sfx);
        }
        count++;
    }
}

void typeLines(const char **lines, unsigned numLines, TextRenderer tr, Vector2<int> location, const AssetAudio& sfx, unsigned textUpdateDelay, unsigned charRate, bool drawCentered) {
    Vector2<int> loc = location;
    for (int i = 0; i < numLines; i++) {
        typeText(lines[i], tr, loc, sfx, textUpdateDelay, charRate, drawCentered);
        loc.y += 8;
    }
}

RGB565 makeColor(uint8_t alpha)
{
    // Linear interpolation between foreground and background

    const RGB565 bg = RGB565::fromRGB(0x31316f);
    const RGB565 fg = RGB565::fromRGB(0xc7c7fc);

    return bg.lerp(fg, alpha);
}

void initDrawing(VideoBuffer* myVidBuf)
{
    /*
     * Init framebuffer, paint a solid background.
     */

    myVidBuf->initMode(SOLID_MODE);
    myVidBuf->colormap[0].set(0.0f, 0.0f, 0.0f);
    myVidBuf->attach(0);

    System::paint();

    /*
     * Now set up a letterboxed 128x48 mode. This uses windowing to
     * start drawing on scanline 40, and draw a total of 48 scanlines.
     *
     * initMode() will automatically wait for the above rendering to finish.
     */

    myVidBuf->initMode(FB128, 40, 48);
    myVidBuf->colormap[1] = makeColor((unsigned)255);
}
