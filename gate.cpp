#include "gate.hpp"

void GateManager::generateGates(const std::vector<GatePosition>& validWalls) {
    int wallCount = validWalls.size();

    if (wallCount < 2) {
        gateA = {-1, -1};
        gateB = {-1, -1};
        return;
    }

    int idxA = rand() % wallCount;
    int idxB = rand() % wallCount;

    while (idxA == idxB) {
        idxB = rand() % wallCount;
    }

    gateA = validWalls[idxA];
    gateB = validWalls[idxB];
}

GatePosition GateManager::getExitGate(int headX, int headY) const {
    if (gateA.x == headX && gateA.y == headY) {
        return gateB;
    }
    if (gateB.x == headX && gateB.y == headY) {
        return gateA;
    }
    return {-1, -1};
}

Direction GateManager::getEdgeExitDirection(GatePosition exitGate, int mapWidth, int mapHeight) const {
    if (exitGate.y == 0) {
        return DOWN;
    }
    if (exitGate.y == mapHeight - 1) {
        return UP;
    }
    if (exitGate.x == 0) {
        return RIGHT;
    }
    if (exitGate.x == mapWidth - 1) {
        return LEFT;
    }
    return NONE_DIR;
}