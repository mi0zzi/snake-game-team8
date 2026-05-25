#include "item.hpp"

// 새로 뽑은 좌표에 이미 다른 아이템이 존재하는지 검사
bool ItemManager::isDuplicatePosition(int x, int y) const {
    for (const auto& it : items) {
        if (it.x == x && it.y == y) return true;
    }
    return false;
}

// 새로 뽑은 좌표가 뱀의 몸통이나 벽 등으로 막혀있는지 검사
bool ItemManager::isBlockedPosition(int x, int y, const std::vector<Position>& blockedPositions) const {
    for (const auto& p : blockedPositions) {
        if (p.x == x && p.y == y) return true;
    }
    return false;
}

// 맵 내 빈 공간을 확인하여 아이템을 최대 개수(3개)까지 무작위 위치에 생성
void ItemManager::generateItem(int mapWidth, int mapHeight, const std::vector<Position>& blockedPositions) {
    if (mapWidth <= 2 || mapHeight <= 2) return;

    int tries = 0;
    while (items.size() < MAX_ITEMS && tries < MAX_SPAWN_TRIES) {
        Item newItem;
        newItem.x = (rand() % (mapWidth - 2)) + 1;
        newItem.y = (rand() % (mapHeight - 2)) + 1;

        if (isDuplicatePosition(newItem.x, newItem.y)) {
            ++tries;
            continue;
        }

        if (isBlockedPosition(newItem.x, newItem.y, blockedPositions)) {
            ++tries;
            continue;
        }

        newItem.type = (rand() % 2 == 0) ? GROWTH : POISON;
        newItem.lifetime = ITEM_LIFETIME;
        items.push_back(newItem);
        ++tries;
    }
}

// 뱀 머리와 아이템의 충돌을 검사하여, 먹힌 아이템은 삭제하고 그 종류를 반환
ItemType ItemManager::checkCollision(int headX, int headY) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->x == headX && it->y == headY) {
            ItemType type = it->type;
            items.erase(it);
            return type;
        }
    }
    return NONE_ITEM;
}

// 매 턴마다 전체 아이템의 수명을 깎고, 수명이 다 된 아이템은 삭제 후 빈자리에 재생성
void ItemManager::updateItems(int mapWidth, int mapHeight, const std::vector<Position>& blockedPositions) {
    for (auto it = items.begin(); it != items.end(); ) {
        it->lifetime--;
        if (it->lifetime <= 0) {
            it = items.erase(it);
        } else {
            ++it;
        }
    }
    generateItem(mapWidth, mapHeight, blockedPositions);
}