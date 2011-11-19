#pragma once
#include <sifteo.h>

using namespace Sifteo;

#define PORTAL_OPEN	0
#define PORTAL_WALL	1
#define PORTAL_DOOR 2
#define PORTAL_LOCK	3

#define ITEM_BASIC_KEY 1

#define TRIGGER_TYPE_PASSIVE    0
#define TRIGGER_TYPE_ACTIVE     1

typedef void (*trigger_func)(int);

struct TriggerData {
    int room;
    trigger_func callback;
};

struct ItemData {
    int room;
    uint32_t itemId;
};

struct MapData {
    const AssetImage* tileset;
    uint8_t* tiles; // every 64 tiles represents an 8x8 room of 16px tiles
    uint8_t* xportals; // vertical portals between rooms (x,y) and (x+1,y)
    uint8_t* yportals; // horizontal portals between rooms (x,y) and (x,y+1)
    TriggerData* triggers; // null if empty, callback=0-terminated
    ItemData* items; // null if empty, itemId=0-termindated
    uint8_t width;
    uint8_t height;
    
    // Convenience Methods

    inline uint8_t GetPortalX(int x, int y) const {
        // note that the pitch here is one greater than the width because there's
        // an extra wall on the right side of the last tile in each row
        ASSERT(0 <= x && x <= width);
        ASSERT(0 <= y && y < height);
        return xportals[y * (width+1) + x];
    }

    inline uint8_t GetPortalY(int x, int y) const {
        // Like GetPortalX except we're in column-major order
        ASSERT(0 <= x && x < width);
        ASSERT(0 <= y && y <= height);
        return yportals[x * (height+1) + y];
    }
    
    inline void SetPortalX(int x, int y, uint8_t pid) {
        ASSERT(0 <= x && x <= width);
        ASSERT(0 <= y && y < height);
        xportals[y * (width+1) + x] = pid;
    }

    inline void SetPortalY(int x, int y, uint8_t pid) {
        ASSERT(0 <= x && x < width);
        ASSERT(0 <= y && y <= height);
        yportals[x * (height+1) + y] = pid;
    }

    inline uint8_t GetTileId(Vec2 location, int x, int y) const {
        // this value indexes into tileset.frames
        ASSERT(0 <= location.x && location.x < width);
        ASSERT(0 <= location.y && location.y < height);
        ASSERT(0 <= x && x < 8);
        ASSERT(0 <= y && y < 8);
        return tiles[ 64 * (location.y * width + location.x) + y * 8 + x ];
    }

    inline void SetTileId(Vec2 location, int x, int y, uint8_t tileId) {
        ASSERT(0 <= location.x && location.x < width);
        ASSERT(0 <= location.y && location.y < height);
        ASSERT(0 <= x && x < 8);
        ASSERT(0 <= y && y < 8);
        tiles[ 64 * (location.y * width + location.x) + y * 8 + x ] = tileId;
    }

    inline uint8_t GetRoomId(Vec2 location) const {
        ASSERT(0 <= location.x && location.x < width);
        ASSERT(0 <= location.y && location.y < height);
        return location.x + location.y * width;
    }

    inline Vec2 GetLocation(uint8_t roomId) const {
        ASSERT(roomId < width * height);
        return Vec2(
            roomId % width,
            roomId / width
        );
    }

    inline TriggerData* GetTriggerData(Vec2 loc) {
        return triggers + GetRoomId(loc);
    }

    inline ItemData* GetItemData(Vec2 loc) {
        return items + GetRoomId(loc);
    }
};