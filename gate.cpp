#include "gate.hpp"

// 유효한 벽 목록에서 무작위로 2개를 선택해 게이트 생성
void GateManager::generateGates(const std::vector<GatePosition>& validWalls) {
    int wallCount = validWalls.size();
    
    // 1. 맵에 설치할 수 있는 벽이 2개 미만이면 안전하게 생성 취소
    if (wallCount < 2) {
        gateA = {-1, -1};
        gateB = {-1, -1};
        return;
    }

    // 2. 서로 겹치지 않는 무작위 벽 위치 2곳 뽑기
    int idxA = rand() % wallCount;
    int idxB = rand() % wallCount;

    while (idxA == idxB) {
        idxB = rand() % wallCount;
    }

    // 3. 뽑힌 위치를 게이트 A와 B로 최종 지정
    gateA = validWalls[idxA];
    gateB = validWalls[idxB];
}

// 뱀 머리가 진입한 게이트의 반대쪽 출구 좌표 반환
GatePosition GateManager::getExitGate(int headX, int headY) const {
    // A로 들어가면 B로, B로 들어가면 A로 나오도록 좌표 연결
    if (gateA.x == headX && gateA.y == headY) {
        return gateB;
    }
    if (gateB.x == headX && gateB.y == headY) {
        return gateA;
    }
    return {-1, -1};
}

// 가장자리 테두리에 위치한 출구 게이트의 맵 안쪽 진출 방향 계산
Direction GateManager::getEdgeExitDirection(GatePosition exitGate, int mapWidth, int mapHeight) const {
    // 출구가 맵의 상하좌우 가장자리에 있을 경우, 뱀이 무조건 맵 안쪽을 향해 나오도록 방향 고정
    if (exitGate.y == 0) return DOWN;
    if (exitGate.y == mapHeight - 1) return UP;
    if (exitGate.x == 0) return RIGHT;
    if (exitGate.x == mapWidth - 1) return LEFT;
    
    return NONE_DIR;
}