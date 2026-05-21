#include "item.hpp"

// 맵 테두리를 제외한 무작위 좌표에 아이템 생성 및 타입 결정
void ItemManager::generateItem(int mapWidth, int mapHeight) {
    currentItem.x = (rand() % (mapWidth - 2)) + 1;
    currentItem.y = (rand() % (mapHeight - 2)) + 1;

    // 50% 확률로 Growth 또는 Poison 타입 부여
    if (rand() % 2 == 0) {
        currentItem.type = GROWTH;
    } else {
        currentItem.type = POISON;
    }
}

// 뱀 머리와 아이템 좌표 충돌 검사
ItemType ItemManager::checkCollision(int snakeHeadX, int snakeHeadY) {
    if (currentItem.x == snakeHeadX && currentItem.y == snakeHeadY) {
        return currentItem.type;
    }
    
    return NONE;
}