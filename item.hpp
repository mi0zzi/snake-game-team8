#ifndef ITEM_HPP
#define ITEM_HPP

#include "map.hpp"
#include <cstdlib>

// 아이템 종류 정의
enum ItemType {
    NONE = 0,
    GROWTH = 1,
    POISON = 2
};

// 아이템 데이터 구조체
struct ItemData {
    int x;
    int y;
    ItemType type;
};

class ItemManager {
public:
    ItemData currentItem;

    void generateItem(int mapWidth, int mapHeight);
    ItemType checkCollision(int snakeHeadX, int snakeHeadY);
};

#endif