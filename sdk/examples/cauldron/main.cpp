/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"
#include "loader.h"
#define CAULDRON_ID 0
using namespace Sifteo;

static const CubeSet cauldronCubeSte(CAULDRON_ID, CAULDRON_ID + 1);
static AssetSlot MainSlot = AssetSlot::allocate()
        .bootstrap(BootstrapGroup);

static AssetSlot AnimationSlot = AssetSlot::allocate();

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];
static MyLoader cauldronLoader(cauldronCubeSte, MainSlot, vid);

static const Float2 ITEM_CENTER = {64, 64};
static const Float2 LEFT_ITEM_CENTER = {32, 64};
static const Float2 RIGHT_ITEM_CENTER = {96, 64};
static const Float2 CENTER_ITEM_CENTER = {64, 64};

Random gRandom;

static Metadata M = Metadata()
    .title("Cauldron Game")
    .package("com.sifteo.sdk.cauldron", "1.1")
    .icon(Icon)
    .cubeRange(0, CUBE_ALLOCATION);

class CauldronGame {
public:
    enum Ingredient {
        INGREDIENT_NONE = 0,

        HONEY,
        FROG_LEGS,
        NIGHTSHADE,
        LAVENDER,

        DRAGONS_BREATH,
        HEART_ORGAN,
        GRIFFON_FEATHER,
        HARPY_BLOOD,
        DREAM_CLOUDS,
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

    enum AnimateItemState {
        ANIMATE_ITEM_NEUTRAL = 0,
        ANIMATE_ITEM_AWAY,
        ANIMATE_ITEM_NEAR,
    };

    struct ItemAnimation {
        AnimateItemState state;
        unsigned frame;
        Float2 offset;
    };

    struct Player {
        unsigned touch;

        Ingredient leftItem;
        Ingredient rightItem;

        Ingredient mixedItem;

        ItemAnimation leftAnimation;
        ItemAnimation rightAnimation;
        ItemAnimation mixedAnimation;
    } players[CUBE_ALLOCATION];

    Ingredient pot_ingredients[CUBE_ALLOCATION] = {};
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

    void loadPotionSprite() {
        CubeID cube(0);
        vid[cube].initMode(BG0_BG1);
        vid[cube].attach(cube);
        vid[0].bg1.setMask(BG1Mask::filled(vec(3,1), vec(10, 14)));
        switch(pot_mixture) {
            case VITALITY :
                vid[0].bg1.image(vec(0,0), PotionVitality, 0);
                break;
            case LOVE :
                vid[0].bg1.image(vec(0,0), PotionLove, 0);
                break;
            case POISONING :
                vid[0].bg1.image(vec(0,0), PotionPoison, 0);
                break;
            case DROWSINESS :
                vid[0].bg1.image(vec(0,0), PotionDrowsiness, 0);
                break;
            case FLIGHT :
                vid[0].bg1.image(vec(0,0), PotionFlight, 0);
                break;
            case HASTE:
            case NEUTRAL:
                vid[0].bg1.image(vec(0,0), PotionNeutral, 0);
                break;
            default :
                vid[0].bg1.eraseMask();
                break;
        }
    }

    void animate(unsigned id, TimeDelta timeStep) {
        ItemAnimation* leftAnimation = &players[id].leftAnimation;
        if ((*leftAnimation).state == ANIMATE_ITEM_AWAY) {
            (*leftAnimation).frame++;
            (*leftAnimation).offset.x -= 64 * (float)timeStep;

            if ((*leftAnimation).offset.x <= -64) {
                // TODO ensure item changed
                (*leftAnimation).state = ANIMATE_ITEM_NEAR;
                drawPlayer(id);
            }
        } else if ((*leftAnimation).state == ANIMATE_ITEM_NEAR) {
            (*leftAnimation).frame++;
            (*leftAnimation).offset.x += 64 * (float)timeStep;

            if ((*leftAnimation).offset.x >= 0) {
                (*leftAnimation).state = ANIMATE_ITEM_NEUTRAL;
                drawPlayer(id);
            }
        } else {
            (*leftAnimation).frame = 0;
            (*leftAnimation).offset.x = 0;
        }

        ItemAnimation* rightAnimation = &players[id].rightAnimation;
        if ((*rightAnimation).state == ANIMATE_ITEM_AWAY) {
            (*rightAnimation).frame++;
            (*rightAnimation).offset.x += 64 * (float)timeStep;

            if ((*rightAnimation).offset.x >= 64) {
                // TODO ensure item changed
                (*rightAnimation).state = ANIMATE_ITEM_NEAR;
                drawPlayer(id);
            }
        } else if ((*rightAnimation).state == ANIMATE_ITEM_NEAR) {
            (*rightAnimation).frame++;
            (*rightAnimation).offset.x -= 64 * (float)timeStep;

            if ((*rightAnimation).offset.x <= 0) {
                (*rightAnimation).state = ANIMATE_ITEM_NEUTRAL;
                drawPlayer(id);
            }
        } else {
            (*rightAnimation).frame = 0;
            (*rightAnimation).offset.x = 0;
        }

        if (players[id].mixedItem) {
            vid[id].sprites[0].move(CENTER_ITEM_CENTER - ITEM_CENTER + players[id].mixedAnimation.offset);
        } else {
            vid[id].sprites[0].move(LEFT_ITEM_CENTER - ITEM_CENTER + players[id].leftAnimation.offset);
            vid[id].sprites[1].move(RIGHT_ITEM_CENTER - ITEM_CENTER + players[id].rightAnimation.offset);
        }
    }

private:
    void onConnect(unsigned id)
    {
        CubeID cube(id);

        bzero(players[id]);
        LOG("Cube %d connected\n", id);

        // init ingredients if player
        if (id > 0) {
            players[id].leftItem = gRandom.randint(HONEY, LAVENDER);
            players[id].rightItem = gRandom.randint(HONEY, LAVENDER);
        }

        vid[id].initMode(id == 0 ? BG0_BG1 : BG0_SPR_BG1);
        vid[id].attach(id);
        motion[id].attach(id);

        // Draw initial state for all sensors
        onAccelChange(cube);
        onTouchOrRelease(cube);

        if (id == 0) {
            // printCauldronDebugIngredients();
        } else {
            vid[id].bg0.image(vec(0,0), FloorBg, 0);
            drawPlayer(id);
        }
    }

    template <unsigned tCapacity>
    String<tCapacity> subString(String<tCapacity> str, unsigned start, unsigned length) {
        String<tCapacity> result;

        int j = 0;
        for (int i = start; i < str.size() && i < start + length; i++) {
            result[j] = str[i];
            LOG("%c", str[i]);
            j++;
        }
        result[j] = '\0';

        return result;
    }

    void drawPlayer(unsigned id) {
        if (players[id].mixedItem) {
            LOG("PLAYER %d MIXED_ITEM: %d\n", id, players[id].mixedItem);
            vid[id].sprites[0].setImage(ingredientToImage(players[id].mixedItem), 0);
            vid[id].sprites[0].move(CENTER_ITEM_CENTER - ITEM_CENTER + players[id].mixedAnimation.offset);
            vid[id].sprites[1].hide();
        } else {
            // LEFT
            {
                LOG("PLAYER %d LEFT: %d\n", id, players[id].leftItem);
                if (players[id].leftItem) {
                    vid[id].sprites[0].setImage(ingredientToImage(players[id].leftItem), 0);
                    vid[id].sprites[0].move(LEFT_ITEM_CENTER - ITEM_CENTER + players[id].leftAnimation.offset);
                } else {
                    vid[id].sprites[0].hide();
                }
            }
            // RIGHT
            {
                LOG("PLAYER %d RIGHT: %d\n", id, players[id].rightItem);
                if (players[id].rightItem) {
                    vid[id].sprites[1].setImage(ingredientToImage(players[id].rightItem), 0);
                    vid[id].sprites[1].move(RIGHT_ITEM_CENTER - ITEM_CENTER + players[id].rightAnimation.offset);
                } else {
                    vid[id].sprites[1].hide();
                }
            }
        }
    }

    const PinnedAssetImage& ingredientToImage(Ingredient ingredient) {
        switch (ingredient) {
            case HONEY:             return Honey;
            case FROG_LEGS:         return FrogLegs;
            case NIGHTSHADE:        return Nightshade;
            case LAVENDER:          return Lavender;

            case DRAGONS_BREATH:    return DragonsBreath;
            case HEART_ORGAN:       return Heart;
            case GRIFFON_FEATHER:   return GriffonFeather;
            case HARPY_BLOOD:       return HarpyBlood;
            case DREAM_CLOUDS:      return DreamClouds;
            case COFFEE_BEANS:      return CoffeeBeans;

            default:                return Heart;
        }
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

    bool isItemPairEqual(Ingredient i0, Ingredient i1, Ingredient j0, Ingredient j1) {
        return (i0 == j0 && i1 == j1) || (i0 == j1 && i1 == j0);
    }

    void maybeCombinePlayerIngredients(unsigned id) {
        Ingredient left = players[id].leftItem;
        Ingredient right = players[id].rightItem;
        Ingredient result = INGREDIENT_NONE;

        if (isItemPairEqual(left, right, FROG_LEGS, NIGHTSHADE)) {
            result = DRAGONS_BREATH;
        }
        else if (isItemPairEqual(left, right, FROG_LEGS, LAVENDER)) {
            result = HEART_ORGAN;
        }
        else if (isItemPairEqual(left, right, HONEY, LAVENDER)) {
            result = GRIFFON_FEATHER;
        }
        else if (isItemPairEqual(left, right, HONEY, NIGHTSHADE)) {
            result = HARPY_BLOOD;
        }
        else if (isItemPairEqual(left, right, LAVENDER, NIGHTSHADE)) {
            result = DREAM_CLOUDS;
        }
        else if (isItemPairEqual(left, right, HONEY, FROG_LEGS)) {
            result = COFFEE_BEANS;
        }

        if (result) {
            players[id].mixedItem = result;
            players[id].leftItem = INGREDIENT_NONE;
            players[id].rightItem = INGREDIENT_NONE;
        }

        drawPlayer(id);
    }

    void clearPotIngredients() {
        for (int i = 0; i < CUBE_ALLOCATION; i++) {
            pot_ingredients[i] = MAX_INGREDIENTS;
            pot_mixture = POTION_NONE;
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
                && potContainsIngredient(COFFEE_BEANS)
            ){
            pot_mixture = VITALITY;
        } else if (
                potContainsIngredient(HEART_ORGAN)
                && potContainsIngredient(DREAM_CLOUDS)
                && potContainsIngredient(COFFEE_BEANS)
                ){
            pot_mixture = LOVE;
        } else if (
                potContainsIngredient(GRIFFON_FEATHER)
                && potContainsIngredient(DRAGONS_BREATH)
                && potContainsIngredient(HARPY_BLOOD)
                ){
            pot_mixture = FLIGHT;
        } else if (
                potContainsIngredient(COFFEE_BEANS)
                && potContainsIngredient(DRAGONS_BREATH)
                && potContainsIngredient(HARPY_BLOOD)
                ){
            pot_mixture = POISONING;
        } else if (
                potContainsIngredient(DREAM_CLOUDS)
                && potContainsIngredient(GRIFFON_FEATHER)
                && potContainsIngredient(HARPY_BLOOD)
                ){
            pot_mixture = DROWSINESS;
        } else if (
                potContainsIngredient(GRIFFON_FEATHER)
                && potContainsIngredient(COFFEE_BEANS)
                && potContainsIngredient(DRAGONS_BREATH)
                ){
            pot_mixture = HASTE;
        } else {
            pot_mixture = NEUTRAL;
        }

        LOG("MIX: %i", pot_mixture);

        clearPotIngredients();
    }
    
    void onTouchOrRelease(unsigned id)
    {
        CubeID cube(id);
        players[id].touch++;
        LOG("Touch event on cube #%d, state=%d\n", id, cube.isTouching());

        if (players[id].touch % 2 == 1) {
            // odd number of "touches" means this is on a press so exit early
            return;
        }
    }

    void onAccelChange(unsigned id)
    {
        CubeID cube(id);
        auto accel = cube.accel();

        unsigned changeFlags = motion[id].update();
        if (changeFlags) {
            // Tilt/shake changed

            LOG("Tilt/shake changed, flags=%08x\n", changeFlags);

            auto tilt = motion[id].tilt;

            if (id == 0) {
                // cauldron
                if (motion[id].shake) {
                    // cauldron shake
                    performMixIngredients();
                } else if (tilt.z == -1) {
                    // empty cauldron
                    clearPotIngredients();
                }
            }

            if (id > 0 && motion[id].shake) {
                maybeCombinePlayerIngredients(id);
            }
        }
    }

    void onNeighborRemove(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Remove: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);
    }

    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Add: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

        if (firstID == 0 || secondID == 0) { // one of the cubes is the cauldron
            unsigned playerID = firstID == 0 ? secondID : firstID;
            unsigned playerSide = firstID == 0 ? secondSide : firstSide;

            if (playerSide == LEFT && players[playerID].leftItem) {
                // pour left item into cauldron
                pot_ingredients[playerID] = players[playerID].leftItem;
                players[playerID].leftItem = INGREDIENT_NONE;
                players[playerID].leftAnimation.state = ANIMATE_ITEM_AWAY;
            } else if (playerSide == RIGHT && players[playerID].rightItem) {
                // pour right item into cauldron
                pot_ingredients[playerID] = players[playerID].rightItem;
                players[playerID].rightItem = INGREDIENT_NONE;
                players[playerID].rightAnimation.state = ANIMATE_ITEM_AWAY;
            } else if (players[playerID].mixedItem) {
                // pour mixed item into cauldron
                pot_ingredients[playerID] = players[playerID].mixedItem;
                players[playerID].mixedItem = INGREDIENT_NONE;
            }
        } else {
            // players initiate a trade

            Ingredient* firstItem;
            ItemAnimation* firstAnim;
            if (firstSide == LEFT && players[firstID].leftItem) {
                firstItem = &players[firstID].leftItem;
                firstAnim = &players[firstID].leftAnimation;
            } else if (firstSide == RIGHT && players[firstID].rightItem) {
                firstItem = &players[firstID].rightItem;
                firstAnim = &players[firstID].rightAnimation;
            }

            Ingredient* secondItem;
            ItemAnimation* secondAnim;
            if (secondSide == LEFT && players[secondID].leftItem) {
                secondItem = &players[secondID].leftItem;
                secondAnim = &players[secondID].leftAnimation;
            } else if (secondSide == RIGHT && players[secondID].rightItem) {
                secondItem = &players[secondID].rightItem;
                secondAnim = &players[secondID].rightAnimation;
            }

            if (firstItem && secondItem && *firstItem && *secondItem) {
                // both players offer an item, do the trade

                (*firstAnim).state = ANIMATE_ITEM_AWAY;
                (*secondAnim).state = ANIMATE_ITEM_AWAY;

                Ingredient tempItem = *firstItem;
                *firstItem = *secondItem;
                *secondItem = tempItem;
            }
        }
    }
};

void main()
{
    static CauldronGame game;
    game.install();

    cauldronLoader.load(Cauldron.assetGroup(), AnimationSlot, CAULDRON_ID);
    vid[CAULDRON_ID].initMode(BG0_BG1);
    vid[CAULDRON_ID].attach(CAULDRON_ID);

    TimeStep ts;
    while (1) {
        unsigned frame = SystemTime::now().cycleFrame(2.0, Cauldron.numFrames());
        vid[CAULDRON_ID].bg0.image(vec(0,0), Cauldron, frame);
        game.loadPotionSprite();

        for (unsigned i = 0; i < arraysize(game.players); i++)
            game.animate(i, ts.delta());

        System::paint();
        ts.next();
    }
}
