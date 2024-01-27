/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"

#define INVENTORY_SIZE 3

using namespace Sifteo;

Random gRandom;

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
        HEART_ORGAN = 0,
        DRAGONS_BREATH,
        GARNET,
        ROSE_PETALS,

        LAVENDER,
        HONEY,
        GRIFFON_FEATHER,
        HARPY_BLOOD,
        DREAM_CLOUDS,
        FROG_LEGS,
        NIGHTSHADE,
        COFFEE_BEANS,

        MAX_INGREDIENTS,
    };

    enum Potion {
        POTION_NONE = 0,

        VITALITY,
        LOVE,
        FLIGHT,
        POISONING,
        DROWSINESS,
        HASTE,
        NEUTRAL,
    };

    struct Player {
        unsigned touch;

        Ingredient inventory[INVENTORY_SIZE];
        unsigned selectedInventoryIndex;
    } players[CUBE_ALLOCATION];

    Ingredient pot_ingredients[CUBE_ALLOCATION] = {MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS, MAX_INGREDIENTS};
    Potion pot_mixture;

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

        bzero(players[id]);
        LOG("Cube %d connected\n", id);

        // init ingredients if player
        if (id > 0) {
            for (unsigned i = 0; i < INVENTORY_SIZE; i++) {
                players[id].inventory[i] = gRandom.randint(HEART_ORGAN, ROSE_PETALS);
            }
            players[id].selectedInventoryIndex = 0;
        }

        vid[id].initMode(BG0_ROM);
        vid[id].attach(id);
        motion[id].attach(id);

        // Draw initial state for all sensors
        onAccelChange(cube);
        onTouchOrRelease(cube);
        drawNeighbors(cube);

        drawCauldronDebugIngredients();
    }

    void clearScreen(unsigned id) {
        // CLEAR SCREEN
        UInt2 topLeft = {0,0};
        UInt2 size = {16,16};
        vid[id].bg0rom.fill(topLeft, size, 0);
    }

    void drawCauldronDebugIngredients() {
        clearScreen(0);

        String<128> str;
        str << "CAULDRON\n";
        for (int i = 1; i < CUBE_ALLOCATION; i++) {
            LOG("hi %d", i);
            str << ingredientToString(pot_ingredients[i]) << "\n";
        }
        vid[0].bg0rom.text(vec(1, 2), str);
    }

    String<16> ingredientToString(Ingredient ingredient) {
        String<16> str;
        switch (ingredient) {
            case HEART_ORGAN:       str << "HEART_ORGAN"; break;
            case DRAGONS_BREATH:    str << "DRAGONS_BREATH"; break;
            case GARNET:            str << "GARNET"; break;
            case ROSE_PETALS:       str << "ROSE_PETALS"; break;

            case LAVENDER:          str << "LAVENDER"; break;
            case HONEY:             str << "HONEY"; break;
            case GRIFFON_FEATHER:   str << "GRIFFON_FEATHER"; break;
            case HARPY_BLOOD:       str << "HARPY_BLOOD"; break;
            case DREAM_CLOUDS:      str << "DREAM_CLOUDS"; break;
            case FROG_LEGS:         str << "FROG_LEGS"; break;
            case NIGHTSHADE:        str << "NIGHTSHADE"; break;
            case COFFEE_BEANS:      str << "COFFEE_BEANS"; break;

            default:                str << "NONE"; break;
        }
        return str;
    }

    String<16> potionToString(Potion potion) {
        String<16> str;
        switch (potion) {
            case VITALITY:      str << "VITALITY"; break;
            case LOVE:          str << "LOVE"; break;
            case FLIGHT:        str << "FLIGHT"; break;
            case POISONING:     str << "POISONING"; break;
            case DROWSINESS:    str << "DROWSINESS"; break;
            case HASTE:         str << "HASTE"; break;
            case NEUTRAL:       str << "NEUTRAL"; break;

            default:            str << "ERROR"; break;
        }
        return str;
    }

    void onPlayerClicked(unsigned id) {
        players[id].selectedInventoryIndex++;
        players[id].selectedInventoryIndex = players[id].selectedInventoryIndex % INVENTORY_SIZE;
    }

    void clearPotIngredients() {
        for (int i = 0; i < CUBE_ALLOCATION; i++) {
            pot_ingredients[i] = MAX_INGREDIENTS;
        }
    }

    bool potContainsIngredient(Ingredient ingredient) {
        for (int i = 0; i < CUBE_ALLOCATION; i++) {
            if (pot_ingredients[i] == ingredient) {
                return true;
            }
        }
        return false;
    }

    void performMixIngredients() {
        if (
                potContainsIngredient(HEART_ORGAN)
                && potContainsIngredient(DRAGONS_BREATH)
                && potContainsIngredient(GARNET)
            ){
            pot_mixture = VITALITY;
        } else if (
                potContainsIngredient(HEART_ORGAN)
                && potContainsIngredient(ROSE_PETALS)
                && potContainsIngredient(LAVENDER)
                && potContainsIngredient(HONEY)
                ){
            pot_mixture = LOVE;
        } else if (
                potContainsIngredient(GRIFFON_FEATHER)
                && potContainsIngredient(DRAGONS_BREATH)
                && potContainsIngredient(HARPY_BLOOD)
                && potContainsIngredient(DREAM_CLOUDS)
                ){
            pot_mixture = FLIGHT;
        } else if (
                potContainsIngredient(FROG_LEGS)
                && potContainsIngredient(NIGHTSHADE)
                && potContainsIngredient(HARPY_BLOOD)
                ){
            pot_mixture = POISONING;
        } else if (
                potContainsIngredient(LAVENDER)
                && potContainsIngredient(HONEY)
                && potContainsIngredient(NIGHTSHADE)
                ){
            pot_mixture = DROWSINESS;
        } else if (
                potContainsIngredient(GRIFFON_FEATHER)
                && potContainsIngredient(FROG_LEGS)
                && potContainsIngredient(COFFEE_BEANS)
                ){
            pot_mixture = HASTE;
        } else {
            pot_mixture = NEUTRAL;
        }
        LOG("MIX: %i", pot_mixture);

        clearPotIngredients();

        clearScreen(0);
        String<32> str;
        str << "MIX: " << potionToString(pot_mixture);

        vid[0].bg0rom.text(vec(1,2), str);
    }
    
    void onTouchOrRelease(unsigned id)
    {
        CubeID cube(id);
        players[id].touch++;
        LOG("Touch event on cube #%d, state=%d\n", id, cube.isTouching());

        if (id != 0) {
            // draw ingredient to player cube
            unsigned inventoryIndex = players[id].selectedInventoryIndex;
            String<16> str = ingredientToString(players[id].inventory[inventoryIndex]);
            vid[id].bg0rom.text(vec(1,9), str);
        }

        if (players[id].touch % 2 == 1) {
            // odd number of "touches" means this is on a press so exit early
            return;
        }

        clearScreen(id);

        // even number of "touches" means this is on a release
        if (id != 0) {
            // players
            onPlayerClicked(id);
        }
    }

    void onAccelChange(unsigned id)
    {
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

            if (motion[id].shake && id == 0) {
                // cauldron shake
                performMixIngredients();
            }
        }

        if (cube != 0) {
            vid[cube].bg0rom.text(vec(1, 10), str);
        }
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
            unsigned inventoryIndex = players[secondID].selectedInventoryIndex;
            pot_ingredients[secondID] = players[secondID].inventory[inventoryIndex];
        }
        if (secondID == 0) {
            unsigned inventoryIndex = players[firstID].selectedInventoryIndex;
            pot_ingredients[firstID] = players[firstID].inventory[inventoryIndex];
        }

        drawCauldronDebugIngredients();

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
