// item_gate.hpp
#ifndef ITEM_HPP
#define ITEM_HPP

#include <vector>
#include <cstdlib>

// 아이템 종류
enum ItemType {
    NONE = 0,
    GROWTH = 1,
    POISON = 2
};

// 아이템 좌표 및 타입 정보
struct ItemData {
    int x;
    int y;
    ItemType type;
};

class ItemManager {
public:
    // 현재 생성된 아이템 (초기 상태: 무효 좌표 및 타입 없음)
    ItemData currentItem = {-1, -1, NONE};

    // 맵 내부에 무작위 좌표로 아이템 생성
    void generateItem(int mapWidth, int mapHeight);
    // 뱀 머리 좌표와 충돌 여부 확인 및 획득한 아이템 타입 반환
    ItemType checkCollision(int snakeHeadX, int snakeHeadY);
};

// 이동 및 진출 방향
enum Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
    NONE_DIR = 4
};

// 게이트 좌표 정보
struct GatePosition {
    int x;
    int y;
};

class GateManager {
public:
    // 현재 활성화된 한 쌍의 게이트 (초기 상태: 무효 좌표)
    GatePosition gateA = {-1, -1};
    GatePosition gateB = {-1, -1};

    // 유효한 벽(Wall) 리스트 중 2곳에 중복 없이 게이트 생성
    void generateGates(const std::vector<GatePosition>& validWalls);
    // 뱀이 진입한 게이트의 반대편 출구 좌표 반환
    GatePosition getExitGate(int headX, int headY);
    // 가장자리 테두리에 생성된 게이트의 맵 안쪽 고정 진출 방향 반환
    Direction getEdgeExitDirection(GatePosition exitGate, int mapWidth, int mapHeight);
};

#endif