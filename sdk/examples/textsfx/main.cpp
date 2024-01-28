/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "fontdata.h"
#include "assets.gen.h"
using namespace Sifteo;

static Metadata M = Metadata()
    .title("Time SFX Demo")
    .package("com.sifteo.sdk.textsfx", "0.1")
    .icon(Icon)
    .cubeRange(1);

static VideoBuffer vid;

static void playSfx(const AssetAudio& sfx) {
    static int i=0;
    AudioChannel(i).play(sfx);
    i = 1 - i;
}

struct TextRenderer {
    FB128Drawable &fb;
    UByte2 position;

    TextRenderer(FB128Drawable &fb) : fb(fb) {}

    void drawGlyph(char ch) {
        // Specifics of our font format
        uint8_t index = ch - ' ';
        const uint8_t *data = font_data + (index << 3) + index;
        uint8_t escapement = *(data++);
        const Int2 size = {8, 8};

        fb.bitmap(position, size, data, 1);
        position.x += escapement;
    }

    static unsigned measureGlyph(char ch) {
        uint8_t index = ch - ' ';
        const uint8_t *data = font_data + (index << 3) + index;
        return data[0];
    }

    void drawText(const char *str) {
        LOG("Drawing text: \"%s\" at (%d, %d)\n",
            str, position.x, position.y);

        char c;
        while ((c = *str)) {
            str++;
            drawGlyph(c);
        }
    }

    static unsigned measureText(const char *str) {
        unsigned width = 0;
        char c;
        while ((c = *str)) {
            str++;
            width += measureGlyph(c);
        }
        return width;
    }

    void drawCentered(const char *str) {
        position.x = (LCD_width - measureText(str)) / 2;
        drawText(str);
        position.y += 8;
    }
};

// str will print out character by character, limit of 64 chars
// location is a vector<unsigned> of coords indicating where to place the text
// charRate is the number of chars to print at a time
void typeText(TextRenderer tr, String<128> str, Vector2<int> location, const AssetAudio& sfx, unsigned textUpdateDelay=2, unsigned charRate=2) {
    String<128> tempStr;

    tr.position.x = location.x;
    tr.position.y = location.y;

    int endIndex = 0; // index of str to print to
    int count = 0;
    while (tempStr.size() < str.size()) {
        tr.position.x = location.x;
        if (count % textUpdateDelay == 0) {
            LOG("Writing '%s'\n", tempStr.c_str());
            endIndex += charRate;
            for (int i = 0; i < endIndex; i++) {
                tempStr[i] = str[i];
            }
            tempStr[endIndex] = '\0';
            tr.drawText(tempStr.c_str());
            System::paint();

            playSfx(sfx);
        }
        count++;
    }
}


static RGB565 makeColor(uint8_t alpha)
{
    // Linear interpolation between foreground and background

    const RGB565 bg = RGB565::fromRGB(0x31316f);
    const RGB565 fg = RGB565::fromRGB(0xc7c7fc);

    return bg.lerp(fg, alpha);
}


static void fadeInAndOut(Colormap &cm)
{
    const unsigned speed = 4;
    const unsigned hold = 100;

    LOG(("~ FADE ~\n"));


    for (unsigned i = 0; i < 0x100; i += speed) {
        cm[1] = makeColor(i);
        System::paint();
    }

    for (unsigned i = 0; i < hold; i++)
        System::paint();

    for (unsigned i = 0; i < 0x100; i += speed) {
        cm[1] = makeColor(255 - i);
        System::paint();
    }
}

void initDrawing()
{
    /*
     * Init framebuffer, paint a solid background.
     */

    vid.initMode(SOLID_MODE);
    vid.colormap[0] = makeColor(0);
    vid.attach(0);

    System::paint();

    /*
     * Now set up a letterboxed 128x48 mode. This uses windowing to
     * start drawing on scanline 40, and draw a total of 48 scanlines.
     *
     * initMode() will automatically wait for the above rendering to finish.
     */

    vid.initMode(FB128, 40, 48);
    vid.colormap[0] = makeColor(0);
}

void onRefresh(void*, unsigned cube)
{
    /*
     * This is an event handler for cases where the system needs
     * us to fully repaint a cube. Normally this can happen automatically,
     * but if we're doing any fancy windowing effects (like we do in this
     * example) the system can't do the repaint all on its own.
     */

    LOG("Refresh event on cube %d\n", cube);
    if (cube == 0)
        initDrawing();
}

void main()
{
    LOG("main\n");
    unsigned fg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::BLUE;
    unsigned bg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::BLACK;

    TextRenderer text(vid.fb128);
    Events::cubeRefresh.set(onRefresh);
    initDrawing();

    text.fb.fill(0);
    String<128> myStr;
    myStr << "This is a test!";
    LOG("%s\n", myStr.c_str());
    typeText(text, myStr, vec(1, 2), Beep, 2, 1);
    LOG("Finished typing");

    // We're entirely event-driven. Everything is
    // updated by SensorListener's event callbacks.
    while (1)
        System::paint();
}
