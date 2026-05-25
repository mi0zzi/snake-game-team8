#include "gate.hpp"

// 주어진 벽 목록에서 무작위로 두 곳을 선택해 한 쌍의 게이트 생성
void GateManager::generateGates(const std::vector<GatePosition>& validWalls, const std::vector<std::vector<int>>& mapData) {
    // [보완] 컴파일러의 미사용 파라미터 경고를 방지하기 위해 의도적으로 명시
    (void)mapData; 

    int wallCount = validWalls.size();
    
    if (wallCount < 2) {
        gateA = {-1, -1};
        gateB = {-1, -1};
        isActive = false;
        return;
    }

    int idxA = rand() % wallCount;
    int idxB = rand() % wallCount;

    while (idxA == idxB) {
        idxB = rand() % wallCount;
    }

    gateA = validWalls[idxA];
    gateB = validWalls[idxB];
    
    isActive = true;
    lifetime = GATE_LIFETIME;
}

// 뱀 머리가 진입한 게이트 좌표를 확인하여 반대편 출구 좌표 반환
GatePosition GateManager::getExitGate(int headX, int headY) const {
    if (gateA.x == headX && gateA.y == headY) return gateB;
    if (gateB.x == headX && gateB.y == headY) return gateA;
    return {-1, -1};
}

// 가장자리 테두리에 위치한 게이트의 맵 안쪽 방향(상하좌우 고정) 진출 계산
Direction GateManager::getEdgeExitDirection(GatePosition exitGate, int mapWidth, int mapHeight) const {
    if (exitGate.y == 0) return DOWN;
    if (exitGate.y == mapHeight - 1) return UP;
    if (exitGate.x == 0) return RIGHT;
    if (exitGate.x == mapWidth - 1) return LEFT;
    return NONE_DIR;
}

// 내부 벽에 위치한 게이트 진출 시, 진입 방향 기준 우선순위(직진->우->좌->후진)로 장애물 없는 빈 방향 계산
Direction GateManager::getInnerExitDirection(Direction entryDir, GatePosition exitGate, const std::vector<std::vector<int>>& mapData) const {
    // [보완] 방향 값이 없거나 맵 데이터가 비어있어 프로그램이 강제 종료되는 상황을 사전 차단
    if (entryDir == NONE_DIR) return NONE_DIR;
    if (mapData.empty() || mapData[0].empty()) return NONE_DIR;

    // [보완] 맵 크기를 동적으로 계산하여 하드코딩으로 인한 버그 방지
    int mapH = (int)mapData.size();
    int mapW = (int)mapData[0].size();

    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};

    int checkOrder[4] = {
        entryDir,
        (entryDir + 1) % 4,
        (entryDir + 3) % 4,
        (entryDir + 2) % 4
    };

    for (int i = 0; i < 4; ++i) {
        int dir = checkOrder[i];
        int nextX = exitGate.x + dx[dir];
        int nextY = exitGate.y + dy[dir];

        // [보완] 다음 칸 좌표가 맵 경계를 벗어나면 검사하지 않고 다음 방향으로 넘김
        if (nextX < 0 || nextX >= mapW || nextY < 0 || nextY >= mapH) continue; 

        // [보완] 일반 벽, 면역 벽, 뱀 몸통이 아닐 때만 진출 가능하도록 예외 처리
        int cell = mapData[nextY][nextX];
        if (cell != 1 && cell != 2 && cell != 4) { 
            return static_cast<Direction>(dir);
        }
    }

    return entryDir;
}

// 매 턴마다 게이트 수명을 관리하고, 통과 중이 아닐 때 수명이 0이 되면 삭제 후 재생성
void GateManager::updateGates(const std::vector<GatePosition>& validWalls, const std::vector<std::vector<int>>& mapData, bool isSnakePassing) {
    if (!isActive) {
        generateGates(validWalls, mapData);
        return;
    }

    if (!isSnakePassing) {
        lifetime--;
        
        if (lifetime <= 0) {
            gateA = {-1, -1};
            gateB = {-1, -1};
            isActive = false;
        }
    }
}