#ifndef ITEM_HPP
#define ITEM_HPP

#include <cstdlib>

enum ItemType {
    NONE = 0,
    GROWTH = 1,
    POISON = 2
};

struct ItemData {
    int x;
    int y;
    ItemType type;
};

class ItemManager {
public:
    ItemData currentItem = {-1, -1, NONE};

    void generateItem(int mapWidth, int mapHeight);
    ItemType checkCollision(int snakeHeadX, int snakeHeadY) const;
};

#endif