#ifndef GATE_HPP
#define GATE_HPP

#include <vector>
#include <cstdlib>

enum Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
    NONE_DIR = 4
};

struct GatePosition {
    int x;
    int y;
};

class GateManager {
public:
    GatePosition gateA = {-1, -1};
    GatePosition gateB = {-1, -1};

    void generateGates(const std::vector<GatePosition>& validWalls);
    GatePosition getExitGate(int headX, int headY) const;
    Direction getEdgeExitDirection(GatePosition exitGate, int mapWidth, int mapHeight) const;
};

#endif