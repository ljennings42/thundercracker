/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"
#include "loader.h"
#include "textsfx.h"

#define CAULDRON_ID 0
using namespace Sifteo;

static const CubeSet cauldronCubeSte(CAULDRON_ID, CAULDRON_ID + 1);
static AssetSlot MainSlot = AssetSlot::allocate()
        .bootstrap(BootstrapGroup);

static AssetSlot AnimationSlot = AssetSlot::allocate();

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];
static MyLoader cauldronLoader(cauldronCubeSte, MainSlot, vid);

static const Float2 ZERO_VECTOR = {0, 0};

static const Float2 ITEM_CENTER = {64, 64};
static const Float2 LEFT_ITEM_CENTER = {32, 64};
static const Float2 RIGHT_ITEM_CENTER = {96, 64};
static const Float2 CENTER_ITEM_CENTER = {64, 64};
static const unsigned PLAYER_TOTAL = 6;

Random gRandom;

static Metadata M = Metadata()
    .title("Cauldron Game")
    .package("com.sifteo.sdk.cauldron", "1.1")
    .icon(Icon)
    .cubeRange(0, CUBE_ALLOCATION);

static double lerp(double start, double end, double t) {
    return (1 - t) * start + t * end;
}

template <typename T>
static Vector2<T> lerp(Vector2<T> start, Vector2<T> end, double t) {
    Vector2<T> result;
    result.x = lerp(start.x, end.x, t);
    result.y = lerp(start.y, end.y, t);
    return  result;
}

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
        LAUGHTER,
        NEUTRAL,
    };

    enum AnimateItemState {
        ANIMATE_ITEM_NEUTRAL = 0,
        ANIMATE_ITEM_AWAY,
        ANIMATE_ITEM_NEAR,
    };

    struct ItemAnimation {
        AnimateItemState state;
        float time;
        Float2 offset;
        unsigned animateDirection;
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

    Array<Ingredient, CUBE_ALLOCATION*3> potIngredients;

    ItemAnimation potItemAnimations[4] = {}; // one for each side
    Ingredient potAnimatingItems[4] = {};

    Potion potMixture;
    Potion prompt = LOVE;
    bool textMode = false;
    bool isIntroTextDone = false;

    void install()
    {
        Events::cubeRefresh.set(&CauldronGame::onRefresh, this);
        Events::neighborAdd.set(&CauldronGame::onNeighborAdd, this);
        Events::neighborRemove.set(&CauldronGame::onNeighborRemove, this);
        Events::cubeAccelChange.set(&CauldronGame::onAccelChange, this);
        Events::cubeTouch.set(&CauldronGame::onTouchOrRelease, this);
        Events::cubeConnect.set(&CauldronGame::onConnect, this);

        // Handle already-connected cubes
        for (CubeID cube : CubeSet::connected())
            onConnect(cube);
    }

    void drawCauldronDebugIngredients() {
        // clearScreen(0);
        vid[CAULDRON_ID].initMode(BG0_ROM);
        String<128> str;
        if (potMixture == 0) {
            str << "CAULDRON\n";
            for (int i = 1; i < potIngredients.count(); i++) {
                str << ingredientToString(potIngredients[i]) << "\n";
            }
            vid[CAULDRON_ID].bg0rom.text(vec(1, 2), str);
        }
        else {
            str << "POTION: " << potionToString(potMixture);
            vid[CAULDRON_ID].bg0rom.text(vec(1, 2), str);
        }

    }

    Potion _prevPotMixture = (Potion)-1;
    void loadPotionSprite() {
        CubeID cube(CAULDRON_ID);

        if (_prevPotMixture != potMixture) {
            _prevPotMixture = potMixture;
            // pot mixture changed, reset the mask!

            vid[cube].bg1.eraseMask();
            if (potMixture) {
                // potion shape
                BG1Mask mask;
                mask.fill(vec(6, 1), vec(3, 4)); // top
                mask.fill(vec(3, 5), vec(10, 9)); // base
                mask.fill(vec(0, 14), vec(16, 2)); // bottom text
                vid[cube].bg1.setMask(mask);
            } else {
                // ui top right
                BG1Mask mask;
                mask.fill(vec(14, 0), vec(2, 6));
                vid[cube].bg1.setMask(mask);
            }
        }

        switch(potMixture) {
            case VITALITY :
                vid[cube].bg1.image(vec(0,0), PotionVitality, 0);
                break;
            case LOVE :
                vid[cube].bg1.image(vec(0,0), PotionLove, 0);
                break;
            case POISONING :
                vid[cube].bg1.image(vec(0,0), PotionPoison, 0);
                break;
            case DROWSINESS :
                vid[cube].bg1.image(vec(0,0), PotionSleep, 0);
                break;
            case FLIGHT :
                vid[cube].bg1.image(vec(0,0), PotionFlight, 0);
                break;
            case HASTE:
                vid[cube].bg1.image(vec(0,0), PotionHaste, 0);
            case LAUGHTER:
                vid[cube].bg1.image(vec(0,0), PotionLaughter, 0);
            case NEUTRAL:
                vid[cube].bg1.image(vec(0,0), PotionNeutral, 0);
                break;
            default:
                // draw cauldron fill ui if no potion
                AssetImage progressImages[] = {
                        CauldronUIBar00,
                        CauldronUIBar30,
                        CauldronUIBar70,
                        CauldronUIBar100,
                };
                AssetImage barImage;

                if (potIngredients.count() > 3) {
                    barImage = progressImages[3];
                } else {
                    barImage = progressImages[potIngredients.count()];
                }

                vid[CAULDRON_ID].bg1.image(vec(0, 0), barImage, 0);
                break;
        }
    }

    void animateItem(unsigned id, ItemAnimation* animation, int distance, TimeDelta timeDelta) {
        Float2 targetPos = {0, 0};
        switch (animation->animateDirection) {
            case LEFT:
                targetPos.x = -distance;
                break;
            case RIGHT:
                targetPos.x = distance;
                break;
            case TOP:
                targetPos.y = -distance;
                break;
            case BOTTOM:
                targetPos.y = distance;
                break;
        }

        animation->time += timeDelta;

        if (animation->state == ANIMATE_ITEM_AWAY) {
            animation->offset = lerp(ZERO_VECTOR, targetPos, animation->time);

            if (animation->time >= 1) {
                animation->state = ANIMATE_ITEM_NEAR;
                animation->time = 0;
                drawSprites(id); // render player (will update item sprite)
            }
        } else if (animation->state == ANIMATE_ITEM_NEAR) {
            animation->offset = lerp(targetPos, ZERO_VECTOR, animation->time);

            if (animation->time >= 1) {
                animation->state = ANIMATE_ITEM_NEUTRAL;
                animation->time = 0;
                drawSprites(id); // render player (will update item sprite)
            }
        } else {
            animation->time = 0;
            animation->offset = ZERO_VECTOR;
            drawSprites(id);
        }
    }

    void animatePlayerItems(unsigned id, TimeDelta timeDelta) {
        animateItem(id, &players[id].leftAnimation, 64, timeDelta);
        animateItem(id, &players[id].rightAnimation, 64, timeDelta);
        animateItem(id, &players[id].mixedAnimation, 128, timeDelta);

        if (players[id].mixedItem || players[id].mixedAnimation.state != ANIMATE_ITEM_NEUTRAL) {
            vid[id].sprites[0].move(CENTER_ITEM_CENTER - ITEM_CENTER + players[id].mixedAnimation.offset);
        } else {
            vid[id].sprites[0].move(LEFT_ITEM_CENTER - ITEM_CENTER + players[id].leftAnimation.offset);
            vid[id].sprites[1].move(RIGHT_ITEM_CENTER - ITEM_CENTER + players[id].rightAnimation.offset);
        }
    }

    void animateCauldronItems(TimeDelta timeDelta) {
        for (unsigned i = 0; i < arraysize(potItemAnimations); i++) {
            animateItem(CAULDRON_ID, &potItemAnimations[i], 128, timeDelta*0.5);
            vid[CAULDRON_ID].sprites[i].move(CENTER_ITEM_CENTER - ITEM_CENTER + potItemAnimations[i].offset);
        }
    }

    void forceDraw(unsigned id) {
        if (id == CAULDRON_ID) {
            _prevPotMixture = (Potion)-1;
        }
        if (id > CAULDRON_ID) {
            vid[id].bg0.image(vec(0,0), FloorBg, 0);
            drawSprites(id);
        }
    }

    void showText(const char* lines[], unsigned numLines, bool fade, int charRate=1, unsigned hold=100) {
        TextRenderer trs[CUBE_ALLOCATION];
        Colormap *cms[CUBE_ALLOCATION];

        for (unsigned i = 0; i < CUBE_ALLOCATION; i++) {
            CubeID cube(i);
            vid[cube].initMode(BG0);
            vid[cube].attach(cube);
            trs[i].fb = &vid[i].fb128;
            cms[i] = &vid[i].colormap;
            (*cms[i])[1] = makeColor(255); // reset to white
            solidBg(&vid[i], i);
        }
        System::paint();
        for (unsigned i = 0; i < CUBE_ALLOCATION; i++) {
            initLetterbox(&vid[i]);
            trs[i].fb->fill(0);
        }
        typeLines(lines, numLines, trs, CUBE_ALLOCATION, vec(0, 2), Beep, 10, charRate, true);
        LOG("Finished typing");

        if (fade) {
            fadeOut(cms, CUBE_ALLOCATION, 4, hold);

            // return video modes back to normal
            for (unsigned i = 0; i < CUBE_ALLOCATION; i++) {
                vid[i].initMode(BG0_SPR_BG1);
                vid[i].attach(i);
                forceDraw(i);
                textMode = false;
            }
        }
        else {
            for (unsigned i=0; i < hold; i++) System::paint(); // pause to read text without fade
            textMode = true;
        }
    }

private:
    void onConnect(unsigned id)
    {
        CubeID cube(id);

        bzero(players[id]);
        LOG("Cube %d connected\n", id);

        // init ingredients if player
        if (id > CAULDRON_ID) {
            players[id].leftItem = gRandom.randint(HONEY, LAVENDER);
            players[id].rightItem = gRandom.randint(HONEY, LAVENDER);
        }

        players[id].leftAnimation.animateDirection = LEFT;
        players[id].rightAnimation.animateDirection = RIGHT;

        motion[id].attach(id);

        // Draw initial state for all sensors
        onAccelChange(cube);
        onTouchOrRelease(cube);
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

    void drawSprites(unsigned id) {
        if (id == CAULDRON_ID) {
            for (unsigned i = 0; i < 4; i++) {
                if (potItemAnimations[i].state != ANIMATE_ITEM_NEUTRAL) {
                    vid[CAULDRON_ID].sprites[i].setImage(ingredientToImage(potAnimatingItems[i]), 0);
                } else {
                    vid[CAULDRON_ID].sprites[i].hide();
                }
                vid[CAULDRON_ID].sprites[i].move(CENTER_ITEM_CENTER - ITEM_CENTER + potItemAnimations[i].offset);
            }
            return;
        }

        if (players[id].mixedItem || players[id].mixedAnimation.state != ANIMATE_ITEM_NEUTRAL) {
            LOG("PLAYER %d MIXED_ITEM: %d\n", id, players[id].mixedItem);
            if (players[id].mixedItem) {
                vid[id].sprites[0].setImage(ingredientToImage(players[id].mixedItem), 0);
                vid[id].sprites[0].move(CENTER_ITEM_CENTER - ITEM_CENTER + players[id].mixedAnimation.offset);
            } else {
                vid[id].sprites[0].hide();
            }
            vid[id].sprites[1].hide();
        } else {
            // LEFT
            {
                // LOG("PLAYER %d LEFT: %d\n", id, players[id].leftItem);
                if (players[id].leftItem) {
                    vid[id].sprites[0].setImage(ingredientToImage(players[id].leftItem), 0);
                    vid[id].sprites[0].move(LEFT_ITEM_CENTER - ITEM_CENTER + players[id].leftAnimation.offset);
                } else {
                    vid[id].sprites[0].hide();
                }
            }
            // RIGHT
            {
                // LOG("PLAYER %d RIGHT: %d\n", id, players[id].rightItem);
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
            case LAUGHTER:      str << "LAUGHTER"; break;
            case NEUTRAL:       str << "NEUTRAL"; break;

            default:            str << "ERROR"; break;
        }
        return str;
    }

    String<16> ingredientToString(Ingredient ingredient) {
        String<16> str;
        switch (ingredient) {
            case HONEY:             str << "HONEY\0"; break;
            case FROG_LEGS:         str << "FROG_LEGS\0"; break;
            case NIGHTSHADE:        str << "NIGHTSHADE\0"; break;
            case LAVENDER:          str << "LAVENDER\0"; break;

            case DRAGONS_BREATH:    str << "DRAGONS_BREATH\0"; break;
            case HEART_ORGAN:       str << "HEART_ORGAN\0"; break;
            case GRIFFON_FEATHER:   str << "GRIFFON_FEATHER\0"; break;
            case HARPY_BLOOD:       str << "HARPY_BLOOD\0"; break;
            case DREAM_CLOUDS:      str << "DREAM_CLOUDS\0"; break;
            case COFFEE_BEANS:      str << "COFFEE_BEANS\0"; break;

            default:                str << "NONE\0"; break;
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

        drawSprites(id);
    }


    void clearPotIngredients() {
        potIngredients.clear();
        potMixture = POTION_NONE;
    }

    bool potContainsIngredient(Ingredient ingredient) {
        for (int i = 0; i < potIngredients.count(); i++) {
            if (potIngredients[i] == ingredient) {
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
            potMixture = VITALITY;
        } else if (
                potContainsIngredient(HEART_ORGAN)
                && potContainsIngredient(DREAM_CLOUDS)
                && potContainsIngredient(COFFEE_BEANS)
                ){
            potMixture = LOVE;
        } else if (
                potContainsIngredient(GRIFFON_FEATHER)
                && potContainsIngredient(DRAGONS_BREATH)
                && potContainsIngredient(HARPY_BLOOD)
                ){
            potMixture = FLIGHT;
        } else if (
                potContainsIngredient(COFFEE_BEANS)
                && potContainsIngredient(DRAGONS_BREATH)
                && potContainsIngredient(HARPY_BLOOD)
                ){
            potMixture = POISONING;
        } else if (
                potContainsIngredient(DREAM_CLOUDS)
                && potContainsIngredient(GRIFFON_FEATHER)
                && potContainsIngredient(HARPY_BLOOD)
                ){
            potMixture = DROWSINESS;
        } else if (
                potContainsIngredient(GRIFFON_FEATHER)
                && potContainsIngredient(COFFEE_BEANS)
                && potContainsIngredient(DRAGONS_BREATH)
                ){
            potMixture = HASTE;
        } else if (
                potContainsIngredient(GRIFFON_FEATHER)
                && potContainsIngredient(COFFEE_BEANS)
                && potContainsIngredient(HONEY)
                ){
            potMixture = LAUGHTER;
        } else {
            potMixture = NEUTRAL;
        }

        LOG("MIX: %i", potMixture);
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

        if (textMode) {
            // return video modes back to normal
            for (unsigned i = 0; i < CUBE_ALLOCATION; i++) {
                vid[i].initMode(BG0_SPR_BG1);
                vid[i].attach(i);
                forceDraw(i);
            }
            textMode = false;
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

            if (id == CAULDRON_ID) {
                if (motion[id].shake) {
                    // cauldron shake
                    performMixIngredients();
                } else if (tilt.z == -1) {
                    // empty cauldron and check how player did
                    clearPotIngredients();
                    prompt = gRandom.randint(VITALITY, LAUGHTER);
                    const char *successLines[] = {
                            "EXCELENT!. I CRAVE A ",
                            potionToString(prompt),
                            "POTION",
                            "please tap to continue..."
                    };

                    const char *failLines[] = {
                            "FOOL!",
                            "THATS NOT WHAT I WANTED.",
                            "BETTER MAKE ME A ",
                            potionToString(prompt),
                            "POTION. Tap to continue"
                    };

                    if (prompt == potMixture) {
                        showText(successLines, 4, false);
                    } else {
                        showText(failLines, 5, false);
                    }
                    resetPlayerItems();
                }
            }

            if (id > 0 && motion[id].shake) {
                maybeCombinePlayerIngredients(id);
            }
        }
    }

    void onRefresh(unsigned cube)
    {
        /*
         * This is an event handler for cases where the system needs
         * us to fully repaint a cube. Normally this can happen automatically,
         * but if we're doing any fancy windowing effects (like we do in this
         * example) the system can't do the repaint all on its own.
         */

        if (!isIntroTextDone) {
            LOG("Refresh event on cube %d\n", cube);
            solidBg(&vid[cube], cube);
            System::paint();
            initLetterbox(&vid[cube]);
        }
    }

    void onNeighborRemove(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Remove: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);
    }

    bool isConnectedNeighorhood() {
        unsigned connections = 0;
        // to-do: read in PLAYER_TOTAL from command line arg
        for (int id = 1; id < PLAYER_TOTAL; id++) {
            CubeID cube(id);
            Neighborhood nb(cube);
            int total = 0;
            if (nb.hasNeighborAt(TOP)) total++;
            if (nb.hasNeighborAt(LEFT)) total++;
            if (nb.hasNeighborAt(BOTTOM)) total++;
            if (nb.hasNeighborAt(RIGHT)) total++;
            // LOG("Connections for Player: %d is %d\n", id, total);
            connections += total;
            // LOG("Total Connections: %d\n", connections);
            if (total == 0) return false;
        }
        return connections >= PLAYER_TOTAL;
    }

    void resetPlayerItems() {
        for (int id = 1; id < PLAYER_TOTAL; id++) {
            players[id].leftItem = gRandom.randint(HONEY, LAVENDER);
            players[id].rightItem = gRandom.randint(HONEY, LAVENDER);
            players[id].mixedItem = INGREDIENT_NONE;
            drawSprites(id);
            // LOG("Clearing igredients for Player: %d\n", id);
        }
    }


    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Add: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

        if (firstID == CAULDRON_ID || secondID == CAULDRON_ID) { // one of the cubes is the cauldron
            unsigned playerID = firstID == CAULDRON_ID ? secondID : firstID;
            unsigned playerSide = firstID == CAULDRON_ID ? secondSide : firstSide;
            unsigned cauldronSide = firstID == CAULDRON_ID ? firstSide : secondSide;
            bool shouldAnimateItemOnCauldron = false;

            if (playerSide == LEFT && players[playerID].leftItem) {
                // pour left item into cauldron
                potIngredients.append(players[playerID].leftItem);
                players[playerID].leftItem = INGREDIENT_NONE;
                players[playerID].leftAnimation.state = ANIMATE_ITEM_AWAY;
                shouldAnimateItemOnCauldron = true;
            } else if (playerSide == RIGHT && players[playerID].rightItem) {
                // pour right item into cauldron
                potIngredients.append(players[playerID].rightItem);
                players[playerID].rightItem = INGREDIENT_NONE;
                players[playerID].rightAnimation.state = ANIMATE_ITEM_AWAY;
                shouldAnimateItemOnCauldron = true;
            } else if (players[playerID].mixedItem) {
                // pour mixed item into cauldron
                potIngredients.append(players[playerID].mixedItem);
                players[playerID].mixedItem = INGREDIENT_NONE;
                players[playerID].mixedAnimation.state = ANIMATE_ITEM_AWAY;
                players[playerID].mixedAnimation.animateDirection = playerSide;
                shouldAnimateItemOnCauldron = true;
            }
            if (shouldAnimateItemOnCauldron) {
                potAnimatingItems[cauldronSide] = potIngredients[potIngredients.count()-1];
                potItemAnimations[cauldronSide].state = ANIMATE_ITEM_NEAR;
                potItemAnimations[cauldronSide].animateDirection = cauldronSide;
                vid[CAULDRON_ID].sprites[playerID].setImage(ingredientToImage(potAnimatingItems[cauldronSide]), 0);
            }
        }
        else if (isConnectedNeighorhood()) {
            // Players want new items
            LOG("NEIGHBORHOOD IS FULLY CONNECTED\n");
            resetPlayerItems();
        }
        else {
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

const char *introLines[] = {
        "A patron enters your bar.",
        "He wants a love potion!",
        "But it's your first day",
        "on the job..."
};
const char *tutLines1[] = {
        "Trade ingredients by",
        "bumping cubes!",
        "Shake to mix ingredients!"
};
const char *tutLines2[] = {
        "Put 3 mixed items in ",
        "the cauldron, shake and",
        "see what you create!"
};
const char *tutLines3[] = {
        "Place all cubes next",
        "to each other to get",
        "fresh new items!"
};

void main()
{
    static CauldronGame game;
    game.install();

    // Toggle this for debug or graphic mode for the cauldron cube
    bool debug = false;
    bool showTextIntro = true;

    if (!debug) {
        cauldronLoader.load(Cauldron.assetGroup(), AnimationSlot, CUBE_ALLOCATION);
        if (showTextIntro)
            game.showText(introLines, 4, true, 1);
            game.showText(tutLines1, 3, false, 3, 150);
            game.showText(tutLines2, 3, false, 3, 150);
            game.showText(tutLines3, 3, true, 3);
        game.isIntroTextDone = true;
    }

    for (unsigned i = 0; i < CUBE_ALLOCATION; i++) {
        vid[i].initMode(BG0_SPR_BG1);
        vid[i].attach(i);
        motion[i].attach(i);
        game.forceDraw(i);
    }

    //AudioTracker::play(Music)

    TimeStep ts;
    while (1) {
        if (debug) {
            game.drawCauldronDebugIngredients();
        }
        else {
            if (!game.textMode) {
                unsigned frame = SystemTime::now().cycleFrame(2.0, Cauldron.numFrames());
                vid[CAULDRON_ID].bg0.image(vec(0, 0), Cauldron, frame);
                game.animateCauldronItems(ts.delta());
                game.loadPotionSprite();
            }
        }

        for (unsigned i = 0; i < arraysize(game.potItemAnimations); i++)
            game.animatePlayerItems(i, ts.delta());


        System::paint();
        ts.next();
    }

}
