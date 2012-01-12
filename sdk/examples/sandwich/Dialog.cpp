#include "Dialog.h"

static const uint8_t font_data[] = {
    0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x02,0x00,0x01,0x01,0x01,0x01,0x01,0x00,0x01,0x00,
    0x04,0x00,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,
    0x07,0x00,0x12,0x3f,0x12,0x12,0x3f,0x12,0x00,0x00,
    0x08,0x14,0x7e,0x15,0x15,0x3e,0x54,0x54,0x3f,0x14,
    0x08,0x00,0x42,0x25,0x12,0x08,0x24,0x52,0x21,0x00,
    0x07,0x00,0x0c,0x12,0x12,0x0e,0x29,0x11,0x2e,0x00,
    0x02,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
    0x04,0x00,0x06,0x01,0x01,0x01,0x01,0x01,0x06,0x00,
    0x04,0x00,0x03,0x04,0x04,0x04,0x04,0x04,0x03,0x00,
    0x06,0x04,0x15,0x0e,0x15,0x04,0x00,0x00,0x00,0x00,
    0x06,0x00,0x00,0x00,0x04,0x04,0x1f,0x04,0x04,0x00,
    0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x02,
    0x05,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,
    0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x00,
    0x08,0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x01,0x00,
    0x06,0x00,0x0e,0x11,0x19,0x15,0x13,0x11,0x0e,0x00,
    0x03,0x00,0x03,0x02,0x02,0x02,0x02,0x02,0x02,0x00,
    0x06,0x00,0x0f,0x10,0x1e,0x0e,0x01,0x01,0x1f,0x00,
    0x06,0x00,0x0f,0x10,0x1e,0x0e,0x10,0x10,0x0f,0x00,
    0x06,0x00,0x08,0x0c,0x0a,0x09,0x1f,0x08,0x08,0x00,
    0x06,0x00,0x1f,0x01,0x01,0x0f,0x10,0x10,0x0f,0x00,
    0x06,0x00,0x0e,0x01,0x01,0x0f,0x11,0x11,0x0e,0x00,
    0x06,0x00,0x1f,0x10,0x08,0x08,0x04,0x04,0x02,0x00,
    0x06,0x00,0x0e,0x11,0x11,0x0e,0x11,0x11,0x0e,0x00,
    0x06,0x00,0x0e,0x11,0x11,0x1e,0x10,0x10,0x0e,0x00,
    0x03,0x00,0x00,0x00,0x03,0x03,0x00,0x03,0x03,0x00,
    0x03,0x00,0x00,0x00,0x03,0x03,0x00,0x03,0x03,0x02,
    0x05,0x00,0x08,0x04,0x02,0x01,0x02,0x04,0x08,0x00,
    0x06,0x00,0x00,0x00,0x00,0x1f,0x00,0x1f,0x00,0x00,
    0x05,0x00,0x01,0x02,0x04,0x08,0x04,0x02,0x01,0x00,
    0x06,0x00,0x0e,0x11,0x10,0x08,0x04,0x00,0x04,0x00,
    0x08,0x00,0x3e,0x41,0x5d,0x55,0x7d,0x01,0x3e,0x00,
    0x06,0x00,0x0e,0x11,0x11,0x1f,0x11,0x11,0x11,0x00,
    0x06,0x00,0x0f,0x11,0x11,0x0f,0x11,0x11,0x0f,0x00,
    0x06,0x00,0x0e,0x11,0x01,0x01,0x01,0x11,0x0e,0x00,
    0x06,0x00,0x0f,0x11,0x11,0x11,0x11,0x11,0x0f,0x00,
    0x06,0x00,0x1f,0x01,0x01,0x0f,0x01,0x01,0x1f,0x00,
    0x06,0x00,0x1f,0x01,0x01,0x0f,0x01,0x01,0x01,0x00,
    0x06,0x00,0x0e,0x11,0x01,0x1d,0x11,0x11,0x0e,0x00,
    0x06,0x00,0x11,0x11,0x11,0x1f,0x11,0x11,0x11,0x00,
    0x04,0x00,0x07,0x02,0x02,0x02,0x02,0x02,0x07,0x00,
    0x06,0x00,0x10,0x10,0x10,0x10,0x10,0x11,0x0e,0x00,
    0x06,0x00,0x11,0x09,0x05,0x03,0x05,0x09,0x11,0x00,
    0x06,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x1f,0x00,
    0x07,0x00,0x21,0x33,0x2d,0x21,0x21,0x21,0x21,0x00,
    0x06,0x00,0x11,0x11,0x13,0x15,0x19,0x11,0x11,0x00,
    0x06,0x00,0x0e,0x11,0x11,0x11,0x11,0x11,0x0e,0x00,
    0x06,0x00,0x1f,0x11,0x11,0x11,0x0f,0x01,0x01,0x00,
    0x07,0x00,0x0e,0x11,0x11,0x11,0x11,0x19,0x1e,0x20,
    0x06,0x00,0x1f,0x11,0x11,0x11,0x0f,0x09,0x11,0x00,
    0x06,0x00,0x0e,0x11,0x01,0x0e,0x10,0x11,0x0e,0x00,
    0x06,0x00,0x1f,0x04,0x04,0x04,0x04,0x04,0x04,0x00,
    0x06,0x00,0x11,0x11,0x11,0x11,0x11,0x11,0x0e,0x00,
    0x06,0x00,0x11,0x11,0x11,0x11,0x0a,0x0a,0x04,0x00,
    0x08,0x00,0x41,0x41,0x49,0x49,0x2a,0x2a,0x14,0x00,
    0x06,0x00,0x11,0x11,0x0a,0x04,0x0a,0x11,0x11,0x00,
    0x06,0x00,0x11,0x11,0x11,0x0a,0x04,0x04,0x04,0x00,
    0x06,0x00,0x1f,0x10,0x08,0x04,0x02,0x01,0x1f,0x00,
    0x04,0x00,0x07,0x01,0x01,0x01,0x01,0x01,0x07,0x00,
    0x08,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x00,
    0x04,0x00,0x07,0x04,0x04,0x04,0x04,0x04,0x07,0x00,
    0x04,0x00,0x02,0x05,0x00,0x00,0x00,0x00,0x00,0x00,
    0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x00,
    0x03,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,
    0x06,0x00,0x00,0x00,0x0e,0x10,0x1e,0x11,0x1e,0x00,
    0x06,0x00,0x01,0x01,0x1f,0x11,0x11,0x11,0x0f,0x00,
    0x06,0x00,0x00,0x00,0x0f,0x11,0x01,0x11,0x0e,0x00,
    0x06,0x00,0x10,0x10,0x1f,0x11,0x11,0x11,0x1e,0x00,
    0x06,0x00,0x00,0x00,0x0e,0x11,0x1f,0x01,0x1e,0x00,
    0x05,0x00,0x0c,0x02,0x07,0x02,0x02,0x02,0x02,0x00,
    0x06,0x00,0x00,0x00,0x1f,0x11,0x11,0x11,0x1e,0x1e,
    0x06,0x00,0x01,0x01,0x0f,0x11,0x11,0x11,0x11,0x00,
    0x02,0x00,0x01,0x00,0x01,0x01,0x01,0x01,0x01,0x00,
    0x03,0x00,0x02,0x00,0x02,0x02,0x02,0x02,0x02,0x02,
    0x05,0x00,0x01,0x01,0x09,0x05,0x03,0x05,0x09,0x00,
    0x02,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,
    0x08,0x00,0x00,0x00,0x3f,0x49,0x49,0x49,0x49,0x00,
    0x06,0x00,0x00,0x00,0x0f,0x11,0x11,0x11,0x11,0x00,
    0x06,0x00,0x00,0x00,0x1f,0x11,0x11,0x11,0x0e,0x00,
    0x06,0x00,0x00,0x00,0x1f,0x11,0x11,0x11,0x0f,0x01,
    0x06,0x00,0x00,0x00,0x1f,0x11,0x11,0x11,0x1e,0x10,
    0x05,0x00,0x00,0x00,0x0d,0x03,0x01,0x01,0x01,0x00,
    0x06,0x00,0x00,0x00,0x1e,0x01,0x0e,0x10,0x0f,0x00,
    0x05,0x00,0x02,0x02,0x0f,0x02,0x02,0x02,0x0c,0x00,
    0x06,0x00,0x00,0x00,0x11,0x11,0x11,0x11,0x0e,0x00,
    0x06,0x00,0x00,0x00,0x11,0x11,0x0a,0x0a,0x04,0x00,
    0x08,0x00,0x00,0x08,0x49,0x49,0x49,0x49,0x36,0x00,
    0x06,0x00,0x00,0x00,0x11,0x0a,0x04,0x0a,0x11,0x00,
    0x06,0x00,0x00,0x00,0x11,0x11,0x11,0x11,0x1e,0x1e,
    0x06,0x00,0x00,0x00,0x1f,0x08,0x04,0x02,0x1f,0x00,
};
#define kFontHeight 9

static uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    // Round to the nearest 5/6 bit color. Note that simple
    // bit truncation does NOT produce the best result!
    uint16_t r5 = ((uint16_t)r * 31 + 128) / 255;
    uint16_t g6 = ((uint16_t)g * 63 + 128) / 255;
    uint16_t b5 = ((uint16_t)b * 31 + 128) / 255;
    return (r5 << 11) | (g6 << 5) | b5;
}

static uint16_t color_lerp(uint8_t alpha) {
    // Linear interpolation between foreground and background

    const unsigned bg_r = 0x00;
    const unsigned bg_g = 0x91;
    const unsigned bg_b = 0x55;

    const unsigned fg_r = 0xff;//0xf4;
    const unsigned fg_g = 0xff;//0xd8;
    const unsigned fg_b = 0xff;//0xb7;
    
    const uint8_t invalpha = 0xff - alpha;

    return rgb565( (bg_r * invalpha + fg_r * alpha) / 0xff,
                   (bg_g * invalpha + fg_g * alpha) / 0xff,
                   (bg_b * invalpha + fg_b * alpha) / 0xff );
}


class DialogView {
private:
    Cube* mCube;
    Vec2 mPosition;

public:
    DialogView(Cube *mCube);
    Cube* GetCube() const { return mCube; }
    void Show(const char* msg);
    void DrawGlyph(char ch);
    unsigned MeasureGlyph(char ch);
    void DrawText(const char* msg);
    unsigned MeasureText(const char* msg);
    void Erase();
    void Fade();

};

DialogView::DialogView(Cube* pCube) : mCube(pCube) {
}

void DialogView::Show(const char* str) {
	mPosition.x = (128 - MeasureText(str)) >> 1;
    DrawText(str);
    mPosition.y += kFontHeight;
}

void DialogView::DrawGlyph(char ch) {
    uint8_t index = ch - ' ';
    const uint8_t *data = font_data + (index * kFontHeight) + index;
    uint8_t escapement = *(data++);
    uint16_t dest = (mPosition.y << 4) | (mPosition.x >> 3);
    unsigned shift = mPosition.x & 7;

    for (unsigned i = 0; i < kFontHeight; i++) {
        mCube->vbuf.pokeb(dest, mCube->vbuf.peekb(dest) | (data[i] << shift));
        dest += 16;
    }

    if (shift) {
        dest += -16*kFontHeight + 1;
        shift = 8 - shift;

        for (unsigned i = 0; i < kFontHeight; i++) {
            mCube->vbuf.pokeb(dest, mCube->vbuf.peekb(dest) | (data[i] >> shift));
            dest += 16;
        }
    }

    mPosition.x += escapement;
}

unsigned DialogView::MeasureGlyph(char ch) {
    uint8_t index = ch - ' ';
    const uint8_t *data = font_data + (index * kFontHeight) + index;
    return data[0];
}

void DialogView::DrawText(const char *str) {
    char c;
    while ((c = *str)) {
        str++;
        DrawGlyph(c);
    }
}

unsigned DialogView::MeasureText(const char *str) {
    unsigned width = 0;
    char c;
    while (c = *str) {
        str++;
        width += MeasureGlyph(c);
    }
    return width;
}

void DialogView::Erase() {
    mPosition.y = 4;
    for (unsigned i = 0; i < sizeof mCube->vbuf.sys.vram.fb / 2; i++) {
    	mCube->vbuf.poke(i, 0);
    }
}

void DialogView::Fade() {
    const unsigned speed = 4;
    const unsigned hold = 100;
    for (unsigned i = 0; i < 0x100; i += speed) {
        mCube->vbuf.poke(
            offsetof(_SYSVideoRAM, colormap) / 2 + 1,
            color_lerp(i)
        );
        System::paint();
    }
    for (unsigned i = 0; i < hold; i++) {
        System::paint();
    }
    for (unsigned i = 0; i < 0x100; i += speed) {
        mCube->vbuf.poke(
            offsetof(_SYSVideoRAM, colormap) / 2 + 1,
            color_lerp(0xFF - i)
        );
        System::paint();
    }
}

void DoDialog(DialogData& data) {
    DialogView view(gCubes);

    VidMode_BG0 mode(view.GetCube()->vbuf);
    mode.init();
    mode.BG0_drawAsset(Vec2(0,0), ScreenOff);
    for(unsigned i=0; i<4; ++i) {
        view.GetCube()->vbuf.touch();
        System::paintSync();
    }
    mode.BG0_drawAsset(Vec2(0,0), Dialog);
    System::paintSync();

    //Now set up a letterboxed 128x48 mode
    view.GetCube()->vbuf.poke(offsetof(_SYSVideoRAM, colormap) / 2 + 0, color_lerp(0));
    view.GetCube()->vbuf.poke(offsetof(_SYSVideoRAM, colormap) / 2 + 1, color_lerp(0));
    view.GetCube()->vbuf.poke(0x3fc/2, 0x3000 + 80);
    view.GetCube()->vbuf.pokeb(offsetof(_SYSVideoRAM, mode), _SYS_VM_FB128);
    view.Erase();
    System::paintSync();
    view.Show("Hello, World");
    view.Show("Wheee!");
    view.Fade();
    view.Erase();
    view.Show("Oh boy");
    view.Show("Another!");
    view.Fade();


    for(;;) {
        view.GetCube()->vbuf.touch();
        System::paint();
    }    
}