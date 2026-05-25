#include "item.hpp"

void ItemManager::generateItem(int mapWidth, int mapHeight) {
    if (mapWidth <= 2 || mapHeight <= 2) {
        return;
    }

    currentItem.x = (rand() % (mapWidth - 2)) + 1;
    currentItem.y = (rand() % (mapHeight - 2)) + 1;

    if (rand() % 2 == 0) {
        currentItem.type = GROWTH;
    } else {
        currentItem.type = POISON;
    }
}

ItemType ItemManager::checkCollision(int snakeHeadX, int snakeHeadY) const {
    if (currentItem.x == snakeHeadX && currentItem.y == snakeHeadY) {
        return currentItem.type;
    }
    return NONE;
}