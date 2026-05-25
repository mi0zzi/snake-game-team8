#ifndef ITEM_HPP
#define ITEM_HPP

#include <vector>
#include <cstdlib>

// 아이템 종류 (성장, 독, 없음)
enum ItemType {
    GROWTH,
    POISON,
    NONE_ITEM
};

// 맵에 배치될 아이템의 좌표와 상태 정보
struct Item {
    int x;
    int y;
    ItemType type;
    int lifetime;
};

// 뱀, 벽 등 아이템이 생성되면 안 되는 막힌 좌표 정보
struct Position {
    int x;
    int y;
};

// 아이템 생성, 충돌 판정, 수명 관리를 총괄하는 클래스
class ItemManager {
public:
    std::vector<Item> items;
    const int MAX_ITEMS = 3;
    const int ITEM_LIFETIME = 50;
    const int MAX_SPAWN_TRIES = 100;

    void generateItem(int mapWidth, int mapHeight, const std::vector<Position>& blockedPositions);
    ItemType checkCollision(int headX, int headY);
    void updateItems(int mapWidth, int mapHeight, const std::vector<Position>& blockedPositions);

private:
    bool isDuplicatePosition(int x, int y) const;
    bool isBlockedPosition(int x, int y, const std::vector<Position>& blockedPositions) const;
};

#endif