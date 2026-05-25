#ifndef GATE_HPP
#define GATE_HPP

#include <vector>
#include <cstdlib>

// 뱀의 이동 방향(상, 우, 하, 좌) 정의
enum Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
    NONE_DIR = 4
};

// 맵 내 게이트의 좌표 저장
struct GatePosition {
    int x;
    int y;
};

class GateManager {
public:
    GatePosition gateA = {-1, -1};
    GatePosition gateB = {-1, -1};

    // 유효한 벽 목록에서 무작위로 2개를 선택해 게이트 생성
    void generateGates(const std::vector<GatePosition>& validWalls);
    
    // 뱀 머리가 진입한 게이트의 반대쪽 출구 좌표 반환
    GatePosition getExitGate(int headX, int headY) const;
    
    // 가장자리 테두리에 위치한 출구 게이트의 맵 안쪽 진출 방향 계산
    Direction getEdgeExitDirection(GatePosition exitGate, int mapWidth, int mapHeight) const;
};

#endif