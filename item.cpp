#include "item.hpp"
#include <cstdlib>

ItemAndGateManager::ItemAndGateManager(int mapSize) 
    : gate(nullptr), wait_delgate(0), mapSize(mapSize) {
    // 최대 동시 출현 Item 수 3개 제한 반영
    itemdb.reserve(3); 
}

ItemAndGateManager::~ItemAndGateManager() {
    clear();
}

void ItemAndGateManager::clear() {
    for (auto& item : itemdb) {
        if (item) delete item;
    }
    itemdb.clear();
    if (gate) {
        delete gate;
        gate = nullptr;
    }
    wait_delgate = 0;
}

void ItemAndGateManager::update(int gameMap[][25], int tick, int& currentItemCount, int& missionGateCount) {
    if (gate && gate->isactive) {
        if (wait_delgate == 0) {
            gameMap[gate->gy1][gate->gx1] = WALL;
            gameMap[gate->gy2][gate->gx2] = WALL;
            delete gate;
            gate = nullptr;
            missionGateCount++; 
        } else {
            wait_delgate--; // 진입 중에는 게이트 소멸 대기
        }
    }

    // 아이템 수명(일정 시간 5초 경과) 처리
    for (auto it = itemdb.begin(); it != itemdb.end(); ) {
        if (*it) {
            (*it)->life -= tick;
            if ((*it)->life <= 0) {
                gameMap[(*it)->y][(*it)->x] = EMPTY;
                delete *it;
                it = itemdb.erase(it);
                currentItemCount--;
                continue;
            }
        }
        ++it;
    }

    if (gate && !gate->isactive) {
        gate->life -= tick;
        if (gate->life <= 0) {
            gameMap[gate->gy1][gate->gx1] = WALL;
            gameMap[gate->gy2][gate->gx2] = WALL;
            delete gate;
            gate = nullptr;
        }
    }
}

void ItemAndGateManager::generateObjects(int gameMap[][25], int& currentItemCount, const std::deque<std::pair<int, int>>& snake, const std::vector<std::pair<int, int>>& walls) {
    int maxEvent = (snake.size() > 9) ? 4 : 3;
    int e = (rand() % maxEvent) + 1;
    if (e == 4) e = 3;

    switch (e) {
        case 1: 
        case 2: 
            if (currentItemCount < 3) {
                int nx = rand() % (mapSize - 2) + 1;
                int ny = rand() % (mapSize - 2) + 1;
                
                // 임의의 빈 공간에 출현 (Snake Body가 있지 않은 곳)
                if (gameMap[ny][nx] == EMPTY) {
                    int itemType = (e == 1) ? GROWTH_ITEM : POISON_ITEM;
                    gameMap[ny][nx] = itemType;
                    itemdb.push_back(new Item(ny, nx, itemType));
                    currentItemCount++;
                }
            }
            break;

        case 3: // Gate
            // 면역 벽(Immune Wall)이 아닌 일반 Wall 리스트를 넘겨받았다고 가정
            if (!gate && walls.size() >= 2) {
                int i = rand() % walls.size();
                int j = rand() % walls.size();
                while (i == j) {
                    j = rand() % walls.size();
                }
                
                // 중복 체크 및 안전 장치: 해당 위치가 정말 1(Wall)인지 한 번 더 확인
                if (gameMap[walls[i].first][walls[i].second] == WALL && 
                    gameMap[walls[j].first][walls[j].second] == WALL) {
                    
                    gate = new Gate(walls[i].first, walls[i].second, walls[j].first, walls[j].second);
                    gameMap[walls[i].first][walls[i].second] = GATE;
                    gameMap[walls[j].first][walls[j].second] = GATE;
                }
            }
            break;
    }
}

int ItemAndGateManager::checkCollision(int& nextY, int& nextX, int gameMap[][25], std::deque<std::pair<int, int>>& snake, int& currentItemCount, int& currentDir) {
    int targetType = gameMap[nextY][nextX];

    if (targetType == GROWTH_ITEM || targetType == POISON_ITEM) {
        for (auto it = itemdb.begin(); it != itemdb.end(); ++it) {
            if (*it && (*it)->y == nextY && (*it)->x == nextX) {
                delete *it;
                itemdb.erase(it);
                currentItemCount--;
                break;
            }
        }
        return targetType;
    } 
    
    if (targetType == GATE) {
        if (gate && !gate->isactive) {
            gate->isactive = true;
            wait_delgate = snake.size(); // Snake 전체가 통과할 때까지 유지
        }

        int out_x = (gate->gx1 == nextX) ? gate->gx2 : gate->gx1;
        int out_y = (gate->gy1 == nextY) ? gate->gy2 : gate->gy1;

        // Rule 4: 가장자리 벽 판단 및 맵 안쪽 고정 진출
        if (out_x == 0) {
            nextX = 1; nextY = out_y; currentDir = 3; // 우측 방향
        } else if (out_x == mapSize - 1) {
            nextX = mapSize - 2; nextY = out_y; currentDir = 2; // 좌측 방향
        } else if (out_y == 0) {
            nextX = out_x; nextY = 1; currentDir = 1; // 하단 방향
        } else if (out_y == mapSize - 1) {
            nextX = out_x; nextY = mapSize - 2; currentDir = 0; // 상단 방향
        } 
        // Rule 4: 맵 내부 벽 판단 (진입방향, 시계방향, 반시계방향, 반대방향 순)
        else {
            int tmpDir = currentDir;
            for (int i = 0; i < 4; i++) {
                int testDir = turn_seq[currentDir][i];
                int tx = out_x + dx[testDir];
                int ty = out_y + dy[testDir];
                
                // 빈 공간이나 아이템이 있는 공간으로 진출 (벽이나 자신의 몸이 아닌 곳)
                if (gameMap[ty][tx] != WALL && gameMap[ty][tx] != IMMUNE_WALL && gameMap[ty][tx] != GATE) {
                    tmpDir = testDir;
                    break;
                }
            }
            currentDir = tmpDir;
            nextX = out_x + dx[currentDir];
            nextY = out_y + dy[currentDir];
        }
        return GATE;
    }

    return EMPTY;
}
