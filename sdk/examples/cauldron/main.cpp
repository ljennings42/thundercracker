/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

static Metadata M = Metadata()
    .title("Cauldron Game")
    .package("com.sifteo.sdk.cauldron", "1.1")
    .icon(Icon)
    .cubeRange(0, CUBE_ALLOCATION);

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];

class CauldronGame {
public:
    enum Ingredient {
        WATER = 0,
        FIRE,
        EARTH,
        AIR,

        MAX_INGREDIENTS,
    };

    enum Mixture {
        MIX_NONE = 0,

        STEAM, // WATER + FIRE
        LAVA,  // EARTH + FIRE
        DUST,  // EARTH + AIR
        MIST,  // WATER + AIR
        SMOKE, // FIRE + AIR
        MUD,   // WATER + EARTH
        ASH,   // EARTH + FIRE + AIR
        LIFE,  // AIR + EARTH + FIRE + WATER
    };

    struct Player {
        unsigned touch;

        Ingredient ingredient;
    } players[CUBE_ALLOCATION];

    Ingredient pot_ingredients[CUBE_ALLOCATION] = {MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS};
    Mixture pot_mixture;

    void install()
    {
        Events::neighborAdd.set(&CauldronGame::onNeighborAdd, this);
        Events::neighborRemove.set(&CauldronGame::onNeighborRemove, this);
        Events::cubeAccelChange.set(&CauldronGame::onAccelChange, this);
        Events::cubeTouch.set(&CauldronGame::onTouchOrRelease, this);
        Events::cubeConnect.set(&CauldronGame::onConnect, this);

        // Handle already-connected cubes
        for (CubeID cube : CubeSet::connected())
            onConnect(cube);
    }

private:
    void onConnect(unsigned id)
    {
        CubeID cube(id);
        uint64_t hwid = cube.hwID();

        bzero(players[id]);
        LOG("Cube %d connected\n", id);

        vid[id].initMode(BG0_ROM);
        vid[id].attach(id);
        motion[id].attach(id);

        // Draw the cube's identity
        String<128> str;
        if (id == 0) {
            str << "CAULDRON\n";
        } else {
            str << "I am cube #" << cube << "\n";
        }
        vid[cube].bg0rom.text(vec(1,2), str);

        // Draw initial state for all sensors
        onAccelChange(cube);
        onTouchOrRelease(cube);
        drawNeighbors(cube);
    }

    String<8> ingredientToString(Ingredient ingredient) {
        String<8> str;
        switch (ingredient) {
            case FIRE:
                str << "FIRE   ";
                break;
            case WATER:
                str << "WATER   ";
                break;
            case EARTH:
                str << "EARTH   ";
                break;
            case AIR:
                str << "AIR   ";
                break;
            default:
                str << "UNK   ";
                break;
        }
        return str;
    }

    void onPlayerClicked(unsigned id) {
        players[id].ingredient = (Ingredient) ((players[id].ingredient + 1) % MAX_INGREDIENTS);
    }

    bool containsIngredientPair(Ingredient i0, Ingredient i1, Ingredient target0, Ingredient target1) {
        return (i0 == target0 && i1 == target1) || (i0 == target1 && i1 == target0);
    }

    void performMixIngredients() {
        // TODO: better logic that doesn't use hard coded indices

        String<32> str;
        str << "MIX: ";

        if(containsIngredientPair(pot_ingredients[1], pot_ingredients[2], WATER, FIRE)) {
            pot_mixture = STEAM;
            str << "STEAM";
        }
        if(containsIngredientPair(pot_ingredients[1], pot_ingredients[2], EARTH, FIRE)) {
            pot_mixture = LAVA;
            str << "LAVA ";
        }
        if(containsIngredientPair(pot_ingredients[1], pot_ingredients[2], EARTH, AIR)) {
            pot_mixture = DUST;
            str << "DUST ";
        }
        if(containsIngredientPair(pot_ingredients[1], pot_ingredients[2], WATER, AIR)) {
            pot_mixture = MIST;
            str << "MIST ";
        }
        if(containsIngredientPair(pot_ingredients[1], pot_ingredients[2], FIRE, AIR)) {
            pot_mixture = SMOKE;
            str << "SMOKE ";
        }
        if(containsIngredientPair(pot_ingredients[1], pot_ingredients[2], WATER, EARTH)) {
            pot_mixture = MUD;
            str << "MUD  ";
        }
        LOG("MIX: %i", pot_mixture);

//        ASH,   // EARTH + FIRE + AIR
//        LIFE,  // AIR + EARTH + FIRE + WATER

        vid[0].bg0rom.text(vec(1,2), str);
    }
    
    void onTouchOrRelease(unsigned id)
    {
        CubeID cube(id);
        players[id].touch++;
        LOG("Touch event on cube #%d, state=%d\n", id, cube.isTouching());

        if (id != 0) {
            String<8> str = ingredientToString(players[id].ingredient);
            vid[id].bg0rom.text(vec(1,9), str);
        }

        if (players[id].touch % 2 == 1) {
            // odd number of "touches" means this is on a press so exit early
            return;
        }

        // even number of "touches" means this is on a release
        if (id == 0) {
            // cauldron
            performMixIngredients();
        } else {
            // players
            onPlayerClicked(id);
        }
    }

    void onAccelChange(unsigned id)
    {
        if (id == 0) {
            // early exit for cauldron
            return;
        }

        CubeID cube(id);
        auto accel = cube.accel();

        String<64> str;
        str << "acc: "
            << Fixed(accel.x, 3)
            << Fixed(accel.y, 3)
            << Fixed(accel.z, 3) << "\n";

        unsigned changeFlags = motion[id].update();
        if (changeFlags) {
            // Tilt/shake changed

            LOG("Tilt/shake changed, flags=%08x\n", changeFlags);

            auto tilt = motion[id].tilt;
            str << "tilt:"
                << Fixed(tilt.x, 3)
                << Fixed(tilt.y, 3)
                << Fixed(tilt.z, 3) << "\n";

            str << "shake: " << motion[id].shake;
        }

        vid[cube].bg0rom.text(vec(1,10), str);
    }

    void onNeighborRemove(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Remove: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

        if (firstID > 0) {
            drawNeighbors(firstID);
        }
        if (secondID > 0) {
            drawNeighbors(secondID);
        }
    }

    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Add: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

        if (firstID == 0) {
            pot_ingredients[secondID] = players[secondID].ingredient;
        }
        if (secondID == 0) {
            pot_ingredients[firstID] = players[firstID].ingredient;
        }

        String<128> str;
        str << "CAULDRON\n";
        for (int i = 1; i < CUBE_ALLOCATION; i++) {
            str << ingredientToString(pot_ingredients[i]) << "\n";
        }
        vid[0].bg0rom.text(vec(1,2), str);

        if (firstID > 0) {
            drawNeighbors(firstID);
        }
        if (secondID > 0) {
            drawNeighbors(secondID);
        }
    }

    void drawNeighbors(CubeID cube)
    {
        Neighborhood nb(cube);

        String<64> str;
        str << "nb "
            << Hex(nb.neighborAt(TOP), 2) << " "
            << Hex(nb.neighborAt(LEFT), 2) << " "
            << Hex(nb.neighborAt(BOTTOM), 2) << " "
            << Hex(nb.neighborAt(RIGHT), 2) << "\n";

        BG0ROMDrawable &draw = vid[cube].bg0rom;
        draw.text(vec(1,6), str);

        drawSideIndicator(draw, nb, vec( 1,  0), vec(14,  1), TOP);
        drawSideIndicator(draw, nb, vec( 0,  1), vec( 1, 14), LEFT);
        drawSideIndicator(draw, nb, vec( 1, 15), vec(14,  1), BOTTOM);
        drawSideIndicator(draw, nb, vec(15,  1), vec( 1, 14), RIGHT);
    }

    static void drawSideIndicator(BG0ROMDrawable &draw, Neighborhood &nb,
        Int2 topLeft, Int2 size, Side s)
    {
        unsigned nbColor = draw.ORANGE;
        draw.fill(topLeft, size,
            nbColor | (nb.hasNeighborAt(s) ? draw.SOLID_FG : draw.SOLID_BG));
    }
};


void main()
{
    static CauldronGame game;

    game.install();

    // We're entirely event-driven. Everything is
    // updated by SensorListener's event callbacks.
    while (1)
        System::paint();
}
