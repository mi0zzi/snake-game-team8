#include <iostream>
#include <vector>
#include "gate.hpp"

using namespace std;

int main() {
    GateManager gm;
    
    vector<vector<int>> mapData = {
        {1, 1, 1, 1, 1},
        {1, 0, 4, 0, 1}, 
        {1, 2, 1, 0, 1}, 
        {1, 0, 1, 0, 1},
        {1, 1, 1, 1, 1}
    };

    vector<GatePosition> walls = {{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {0,1}, {4,1}, {0,2}, {2,2}, {4,2}};
    
    cout << "=== 1. Gate Generation Test ===" << endl;
    gm.generateGates(walls, mapData);
    cout << "Gate A: (" << gm.gateA.x << ", " << gm.gateA.y << ")" << endl;
    cout << "Gate B: (" << gm.gateB.x << ", " << gm.gateB.y << ")" << endl;
    cout << "Lifetime: " << gm.lifetime << endl;

    cout << "\n=== 2. Obstacle Evasion Test (Inner Gate) ===" << endl;
    gm.gateA = {2, 2}; 
    Direction entry = UP; 
    
    cout << "Exit Gate: (2,2), Entry Direction: " << entry << " (UP=0)" << endl;
    cout << "[Surroundings of (2,2)]" << endl;
    cout << "- UP(2,1)   : 4 (Snake Body) -> Blocked" << endl;
    cout << "- RIGHT(3,2): 0 (Empty)      -> Open!" << endl;
    cout << "- DOWN(2,3) : 1 (Wall)       -> Blocked" << endl;
    cout << "- LEFT(1,2) : 2 (Immune Wall)-> Blocked" << endl;

    Direction exitDir = gm.getInnerExitDirection(entry, gm.gateA, mapData);
    cout << "Calculated Exit Direction: " << exitDir << " (Expected: 1 for RIGHT)" << endl;

    cout << "\n=== 3. Boundary Out-of-Bounds Defense Test ===" << endl;
    gm.gateA = {0, 0};
    Direction edgeExitDir = gm.getInnerExitDirection(UP, gm.gateA, mapData);
    cout << "Exit Gate: (0,0), Entry Direction: UP(0)" << endl;
    cout << "Result: Handled safely without Segfault! Direction: " << edgeExitDir << endl;

    cout << "\n=== 4. Timer & Snake Passing Test ===" << endl;
    gm.updateGates(walls, mapData, false);
    cout << "After 1 turn (Snake NOT passing) -> Lifetime: " << gm.lifetime << endl;
    
    gm.updateGates(walls, mapData, true);
    cout << "After 1 turn (Snake IS passing)  -> Lifetime: " << gm.lifetime << " (Should not decrease)" << endl;

    return 0;
}