#ifndef GATE_HPP
#define GATE_HPP

#include <vector>
#include <cstdlib>

// 뱀의 이동 방향 (상, 우, 하, 좌)
enum Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
    NONE_DIR = 4
};

// 맵 내 게이트의 좌표
struct GatePosition {
    int x;
    int y;
};

class GateManager {
public:
    GatePosition gateA = {-1, -1};
    GatePosition gateB = {-1, -1};
    
    bool isActive = false;         // 게이트가 맵에 존재하는지 확인하는 스위치
    int lifetime = 0;              // 게이트가 사라지기까지 남은 시간
    const int GATE_LIFETIME = 50;  // 게이트의 기본 수명 (50턴)

    // 벽 목록을 받아 게이트 한 쌍 생성
    void generateGates(const std::vector<GatePosition>& validWalls, const std::vector<std::vector<int>>& mapData);
    // 진입한 게이트의 반대쪽 출구 좌표 반환
    GatePosition getExitGate(int headX, int headY) const;
    // 가장자리 게이트의 맵 안쪽 진출 방향 계산
    Direction getEdgeExitDirection(GatePosition exitGate, int mapWidth, int mapHeight) const;
    // 내부 게이트의 진출 방향 계산 (우선순위: 직진->우회전->좌회전->후진)
    Direction getInnerExitDirection(Direction entryDir, GatePosition exitGate, const std::vector<std::vector<int>>& mapData) const;
    // 매 턴마다 게이트 수명 관리 및 재생성 (뱀 통과 중 예외 처리 포함)
    void updateGates(const std::vector<GatePosition>& validWalls, const std::vector<std::vector<int>>& mapData, bool isSnakePassing);
};

#endif