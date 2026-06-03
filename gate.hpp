#ifndef GATE_HPP
#define GATE_HPP

#include <vector>
#include <cstdlib>

// 뱀의 맵 내 이동 방향
enum Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
    NONE_DIR = 4
};

// 게이트가 생성될 벽의 좌표 구조체
struct GatePosition {
    int x;
    int y;
};

// 게이트의 종류 (누구나 통과 가능한 일반 게이트 / 길이 5 이상 조건부 게이트)
enum GateType {
    NORMAL_GATE = 0,
    LEN5_GATE = 1
};

class GateManager {
public:
    GatePosition gateA = {-1, -1};
    GatePosition gateB = {-1, -1};

    bool isActive = false;          // 맵에 게이트가 활성화되어 있는지 여부
    int lifetime = 0;               // 현재 활성화된 게이트의 남은 수명
    const int GATE_LIFETIME = 50;   // 게이트 기본 수명 상수

    GateType gateType = NORMAL_GATE; // 현재 활성화된 게이트의 타입

    int respawnCooldown = 0;             // 게이트 소멸 후 재생성까지 남은 대기 턴
    const int RESPAWN_DELAY_TURNS = 1;   // 플레이어 피로도 방지용 게이트 공백기 상수 (1턴)

    // 게이트 생성 및 진출입 관련 핵심 메서드
    void generateGates(const std::vector<GatePosition>& validWalls, const std::vector<std::vector<int>>& mapData);
    GatePosition getExitGate(int headX, int headY) const;
    Direction getEdgeExitDirection(GatePosition exitGate, int mapWidth, int mapHeight) const;
    Direction getInnerExitDirection(Direction entryDir, GatePosition exitGate, const std::vector<std::vector<int>>& mapData) const;
    void updateGates(const std::vector<GatePosition>& validWalls, const std::vector<std::vector<int>>& mapData, bool isSnakePassing);

    // 외부(Main)에서 뱀 머리 충돌 판정 시 사용할 상태 확인 메서드
    bool isGateCell(int x, int y) const;
    bool canPassConditionalGate(int snakeLength) const;
};

#endif