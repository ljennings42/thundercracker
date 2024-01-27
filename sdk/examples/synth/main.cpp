/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

static Metadata M = Metadata()
    .title("TinySynth")
    .package("com.sifteo.sdk.synth", "1.0")
    .icon(Icon)
    .cubeRange(1);

static AssetSlot gMainSlot = AssetSlot::allocate();

static AssetLoader loader; // global asset loader (each cube will have symmetric assets)

static const CubeID cube = 0;
static VideoBuffer vid;


static void playSfx(const AssetAudio& sfx) {
    static int i=0;
    AudioChannel(i).play(sfx);
    i = 1 - i;
}

// str will print out character by character
// location is a vector<unsigned> of coords indicating where to place the text
// charRate is the number of chars to print at a time
void typeText(String<128> str, Vector2<int> location, unsigned textUpdateDelay, unsigned charRate = 2) {
    String<128> temp;
    int endIndex = 0; // index of str to print to
    int count = 0;
    while (temp != str) {
        // magic numbers galore
        if (count % textUpdateDelay) {
            playSfx(Beep);

            endIndex += charRate;
            for (int i = 0; i < endIndex; i++) {
                temp[i] = str[i];
            }
        }
        count++;
    }
    vid.bg0rom.text(location, temp);
}

void main()
{
    unsigned fg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::BLUE;
    unsigned bg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::BLACK;

    vid.initMode(BG0_ROM);
    vid.attach(cube);
    vid.bg0rom.erase(bg);
    vid.bg0rom.fill(vec(0,0), vec(3,3), fg);
    String<128> str;
    str << "This is a test";
    typeText(str, vec(1, 2), 1000, 1);

    while (1) {

        //const Int2 center = LCD_center - vec(24,24)/2;
        //vid.bg0rom.setPanning(-(center + accel.xy() * 60.f));
        System::paint();
    }
}
