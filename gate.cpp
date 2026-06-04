#include "gate.hpp"

// 주어진 유효한 벽 목록(validWalls)에서 무작위로 2곳을 골라 게이트 한 쌍을 생성
void GateManager::generateGates(const std::vector<GatePosition>& validWalls, const std::vector<std::vector<int>>& mapData) {
    (void)mapData;

    const int wallCount = (int)validWalls.size();
    
    // 유효한 벽이 2개 미만일 경우 게이트를 생성할 수 없으므로 비활성화 처리
    if (wallCount < 2) {
        gateA = {-1, -1};
        gateB = {-1, -1};
        isActive = false;
        return;
    }

    const int idxA = rand() % wallCount;
    int idxB = rand() % wallCount;
    
    // 두 게이트가 같은 벽에 생성되지 않도록 중복 방지 루프
    while (idxA == idxB) {
        idxB = rand() % wallCount;
    }

    gateA = validWalls[idxA];
    gateB = validWalls[idxB];
    isActive = true;
    lifetime = GATE_LIFETIME;

    // 10% 확률로 조건부(길이 5 이상) 게이트 생성, 90% 확률로 일반 게이트 생성
    const int typeRoll = rand() % 100;
    gateType = (typeRoll < 10) ? LEN5_GATE : NORMAL_GATE; 
}

// 뱀이 진입한 게이트 좌표를 확인하여 반대편 출구 좌표를 반환
GatePosition GateManager::getExitGate(int headX, int headY) const {
    if (!isActive) return {-1, -1};
    if (gateA.x == headX && gateA.y == headY) return gateB;
    if (gateB.x == headX && gateB.y == headY) return gateA;
    return {-1, -1};
}

// 맵 가장자리 테두리에 위치한 게이트의 안쪽 진출 방향 계산 (상하좌우 고정)
Direction GateManager::getEdgeExitDirection(GatePosition exitGate, int mapWidth, int mapHeight) const {
    if (exitGate.y == 0) return DOWN;
    if (exitGate.y == mapHeight - 1) return UP;
    if (exitGate.x == 0) return RIGHT;
    if (exitGate.x == mapWidth - 1) return LEFT;
    return NONE_DIR;
}

// 맵 내부 벽에 위치한 게이트 진출 방향 계산 (우선순위: 직진 -> 우회전 -> 좌회전 -> 후진)
Direction GateManager::getInnerExitDirection(Direction entryDir, GatePosition exitGate, const std::vector<std::vector<int>>& mapData) const {
    if (entryDir == NONE_DIR) return NONE_DIR;
    if (mapData.empty() || mapData[0].empty()) return NONE_DIR;

    const int mapH = (int)mapData.size();
    const int mapW = (int)mapData[0].size();

    const int dx[4] = {0, 1, 0, -1};
    const int dy[4] = {-1, 0, 1, 0};
    const int checkOrder[4] = {
        entryDir,
        (entryDir + 1) % 4,
        (entryDir + 3) % 4,
        (entryDir + 2) % 4
    };

    // 설정된 우선순위에 따라 4방향을 검사하여 빈 공간(장애물이 없는 곳)을 찾음
    for (int i = 0; i < 4; ++i) {
        const int dir = checkOrder[i];
        const int nextX = exitGate.x + dx[dir];
        const int nextY = exitGate.y + dy[dir];

        if (nextX < 0 || nextX >= mapW || nextY < 0 || nextY >= mapH) continue;

        const int cell = mapData[nextY][nextX];
        // 1(벽), 2(면역 벽), 4(뱀 몸통)가 아닌 통과 가능한 칸일 경우 방향 반환
        if (cell != 1 && cell != 2 && cell != 4) {
            return static_cast<Direction>(dir);
        }
    }

    return entryDir;
}

// 매 턴 호출되어 게이트 수명 차감, 소멸 처리 및 재생성 타이밍(쿨다운)을 관리
void GateManager::updateGates(const std::vector<GatePosition>& validWalls, const std::vector<std::vector<int>>& mapData, bool isSnakePassing) {
    if (isActive) {
        // 뱀이 게이트를 통과하는 중(머리는 들어갔고 꼬리는 덜 나온 상태)에는 수명 차감을 정지
        if (!isSnakePassing) lifetime--;

        if (lifetime <= 0) {
            gateA = {-1, -1};
            gateB = {-1, -1};
            isActive = false;
            respawnCooldown = RESPAWN_DELAY_TURNS; // 즉시 생성하지 않고 1턴 공백기 부여
        }
        return;
    }

    // 게이트 공백기(쿨다운) 처리
    if (respawnCooldown > 0) {
        respawnCooldown--;
        return; // 쿨다운이 끝날 때까지 생성을 보류하고 턴 종료
    }

    // 쿨다운이 0이 되면 새로운 게이트 쌍 생성
    generateGates(validWalls, mapData);
}

// 특정 좌표가 현재 열려있는 게이트의 위치인지 확인
bool GateManager::isGateCell(int x, int y) const {
    if (!isActive) return false;
    if (gateA.x == x && gateA.y == y) return true;
    if (gateB.x == x && gateB.y == y) return true;
    return false;
}

// 현재 생성된 게이트의 타입과 뱀의 길이를 비교하여 진입 가능 여부를 최종 판정
bool GateManager::canPassConditionalGate(int snakeLength) const {
    if (!isActive) return false; 
    
    // 조건부 게이트일 경우 뱀의 길이가 5 이상인지 검사
    if (gateType == LEN5_GATE) return snakeLength >= 5;
    
    // 일반 게이트일 경우 조건 없이 무조건 통과 허용
    return true;
}