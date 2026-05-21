// item_gate.cpp
#include "item.hpp"

// 맵 테두리를 제외한 무작위 위치에 아이템 생성 및 타입(성장/독) 부여
void ItemManager::generateItem(int mapWidth, int mapHeight) {
    // 맵 크기가 유효하지 않은 경우 안전 종료
    if (mapWidth <= 2 || mapHeight <= 2) {
        return;
    }

    currentItem.x = (rand() % (mapWidth - 2)) + 1;
    currentItem.y = (rand() % (mapHeight - 2)) + 1;

    // 50% 확률로 아이템 타입 결정
    if (rand() % 2 == 0) {
        currentItem.type = GROWTH;
    } else {
        currentItem.type = POISON;
    }
}

// 뱀 머리와 아이템의 좌표 충돌 확인 및 타입 반환
ItemType ItemManager::checkCollision(int snakeHeadX, int snakeHeadY) {
    if (currentItem.x == snakeHeadX && currentItem.y == snakeHeadY) {
        return currentItem.type;
    }
    return NONE;
}

// 유효한 벽 목록에서 무작위로 2개를 선택해 게이트 생성
void GateManager::generateGates(const std::vector<GatePosition>& validWalls) {
    int wallCount = validWalls.size();

    // 생성 가능한 벽이 2개 미만일 경우 생성 취소 (초기화)
    if (wallCount < 2) {
        gateA = {-1, -1};
        gateB = {-1, -1};
        return;
    }

    int idxA = rand() % wallCount;
    int idxB = rand() % wallCount;

    // 두 게이트가 동일한 위치에 생성되지 않도록 중복 방지
    while (idxA == idxB) {
        idxB = rand() % wallCount;
    }

    gateA = validWalls[idxA];
    gateB = validWalls[idxB];
}

// 진입한 게이트에 대응하는 반대편 출구 좌표 반환
GatePosition GateManager::getExitGate(int headX, int headY) {
    if (gateA.x == headX && gateA.y == headY) {
        return gateB;
    }
    if (gateB.x == headX && gateB.y == headY) {
        return gateA;
    }
    
    // 게이트 좌표가 아닐 경우 무효 좌표 반환
    return {-1, -1};
}

// 맵 가장자리 벽에 위치한 게이트의 고정 진출 방향(맵 안쪽) 반환
Direction GateManager::getEdgeExitDirection(GatePosition exitGate, int mapWidth, int mapHeight) {
    if (exitGate.y == 0) {
        return DOWN;  // 상단 벽 -> 아래로 진출
    }
    if (exitGate.y == mapHeight - 1) {
        return UP;    // 하단 벽 -> 위로 진출
    }
    if (exitGate.x == 0) {
        return RIGHT; // 좌측 벽 -> 오른쪽으로 진출
    }
    if (exitGate.x == mapWidth - 1) {
        return LEFT;  // 우측 벽 -> 왼쪽으로 진출
    }
    
    // 가장자리가 아닐 경우 처리 안 함
    return NONE_DIR;
}