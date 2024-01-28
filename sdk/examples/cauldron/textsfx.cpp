//
// Created by soda on 1/27/24.
//

#include "textsfx.h"

using namespace Sifteo;

void fadeOut(Colormap *cm[], const unsigned cmsCount, const int speed, const unsigned hold)
{
    LOG(("~ FADE ~\n"));


    for (unsigned i = 0; i < hold; i++)
        System::paint();

    for (int i = 255; i >= 0; i -= speed) {
        for (unsigned j = 0; j < cmsCount; j++) {
            (*cm[j])[1] = makeColor(i);
        }
        System::paint();
    }

    LOG("Finished Fade\n");
}

void TextRenderer::drawGlyph(char ch) {
    // Specifics of our font format
    uint8_t index = ch - ' ';
    const uint8_t *data = font_data + (index << 3) + index;
    uint8_t escapement = *(data++);
    const Int2 size = {8, 8};

    fb->bitmap(position, size, data, 1);
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
void typeText(const char *str, TextRenderer trs[], const unsigned trsCount, Vector2<int> location, const AssetAudio& sfx, unsigned textUpdateDelay, unsigned charRate, bool drawCentered) {
    LOG("Typing out '%s'\n", str);
    String<128> tempStr;

    for (unsigned i = 0; i < trsCount; i++) {
        trs[i].position.x = location.x;
        trs[i].position.y = location.y;
    }

    int endIndex = 0; // index of str to print to
    int count = 0;
    int textDrawCount = 0;
    bool strEnd = false;
    while (str[endIndex] != '\0') {
        // reset x position of text for each redraw
        for (unsigned i = 0; i < trsCount; i++) {
            trs[i].position.x = location.x;
        }

        if (count == 0 || count % (textUpdateDelay * 100) == 0) {
            LOG("Writing '%s'\n", tempStr.c_str());

            // iterate endIndex by charRate, but watch for the null terminator
            bool flag = true;
            int loopCount = 0;
            while (flag) {
                endIndex++;
                if (str[endIndex] == '\0')
                    flag = false;
                if (loopCount >= charRate-1)
                    flag = false;
                loopCount++;
            }

            for (int i = 0; i < endIndex; i++) {
                tempStr[i] = str[i];
            }
            tempStr[endIndex] = '\0';

            for (unsigned i = 0; i < trsCount; i++) {
                if (drawCentered) {
                    trs[i].drawCentered(tempStr.c_str());
                    trs[i].position.x = location.x;
                } else trs[i].drawText(tempStr.c_str());
            }
            System::paint();

            if (textDrawCount == 0 || textDrawCount % (charRate) == 0)
                playSfx(sfx);
            textDrawCount++;
        }

        count++;
    }
}

void typeLines(const char **lines, unsigned numLines, TextRenderer trs[], const unsigned trsCount, Vector2<int> location, const AssetAudio& sfx, unsigned textUpdateDelay, unsigned charRate, bool drawCentered) {
    Vector2<int> loc = location;
    for (int i = 0; i < numLines; i++) {
        typeText(lines[i], trs, trsCount, loc, sfx, textUpdateDelay, charRate, drawCentered);
        loc.y += 8;
    }
}

RGB565 makeColor(uint8_t alpha)
{
    // Linear interpolation between foreground and background

    const RGB565 bg = RGB565::fromRGB(0x0);
    const RGB565 fg = RGB565::fromRGB(0xc7c7fc);

    return bg.lerp(fg, alpha);
}

void solidBg(VideoBuffer* myVidBuf, unsigned id) {
    /*
     * Init framebuffer, paint a solid background.
     */

    myVidBuf->initMode(SOLID_MODE);
    myVidBuf->colormap[0].set(0.0f, 0.0f, 0.0f);
    myVidBuf->attach(id);
}

void initLetterbox(VideoBuffer* myVidBuf)
{
    /*
     * Now set up a letterboxed 128x48 mode. This uses windowing to
     * start drawing on scanline 40, and draw a total of 48 scanlines.
     *
     * initMode() will automatically wait for the above rendering to finish.
     */

    myVidBuf->initMode(FB128, 40, 48);
    myVidBuf->colormap[1] = makeColor((unsigned)255);
}

