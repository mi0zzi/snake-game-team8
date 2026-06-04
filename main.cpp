#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <locale.h>
#include <ncursesw/ncurses.h>
#include <utility>
#include <vector>

#include "constants.hpp"
#include "gate.hpp"
#include "item.hpp"
#include "map.hpp"
#include "snake.hpp"
#include "ui.hpp"

struct MissionTarget {
    int length;
    int growth;
    int poison;
    int gate;
};

static const MissionTarget MISSIONS[4] = {
    {4, 1, 1, 1},
    {5, 2, 1, 1},
    {6, 3, 2, 2},
    {7, 4, 2, 2}
};

// initStage()로 만든 현재 gameMap을 스테이지 원본 맵으로 복사
static void copyStageMap(int stageMap[MAP_SIZE][MAP_SIZE]) {
    for (int y = 0; y < MAP_SIZE; ++y) {
        for (int x = 0; x < MAP_SIZE; ++x) {
            stageMap[y][x] = gameMap[y][x];
        }
    }
}

// 벽과 장애물만 담긴 스테이지 원본 맵을 공유 gameMap에 복원
static void resetSharedMap(const int stageMap[MAP_SIZE][MAP_SIZE]) {
    for (int y = 0; y < MAP_SIZE; ++y) {
        for (int x = 0; x < MAP_SIZE; ++x) {
            gameMap[y][x] = stageMap[y][x];
        }
    }
}

// 게이트 방향 계산 함수가 사용할 수 있도록 공유 gameMap을 vector 형태로 변환
static std::vector<std::vector<int>> toVectorMap() {
    std::vector<std::vector<int>> mapData(MAP_SIZE, std::vector<int>(MAP_SIZE));

    for (int y = 0; y < MAP_SIZE; ++y) {
        for (int x = 0; x < MAP_SIZE; ++x) {
            mapData[y][x] = gameMap[y][x];
        }
    }
    return mapData;
}

// 게이트가 생성될 수 있는 일반 벽 좌표 목록 생성
static std::vector<GatePosition> buildValidWalls(const int stageMap[MAP_SIZE][MAP_SIZE]) {
    std::vector<GatePosition> validWalls;

    for (int y = 0; y < MAP_SIZE; ++y) {
        for (int x = 0; x < MAP_SIZE; ++x) {
            if (stageMap[y][x] == WALL) {
                validWalls.push_back({x, y});
            }
        }
    }
    return validWalls;
}

// 아이템이 생성되면 안 되는 벽, 뱀, 게이트, 진행 방향 앞칸 좌표 목록 생성
static std::vector<Position> buildBlockedPositions(const int stageMap[MAP_SIZE][MAP_SIZE],
                                                   const Snake& snake,
                                                   const GateManager& gateMgr) {
    std::vector<Position> blocked;

    for (int y = 0; y < MAP_SIZE; ++y) {
        for (int x = 0; x < MAP_SIZE; ++x) {
            if (stageMap[y][x] != EMPTY) {
                blocked.push_back({x, y});
            }
        }
    }

    for (const auto& part : snake.getBody()) {
        blocked.push_back({part.second, part.first});
    }

    int forwardY = 0;
    int forwardX = 0;
    snake.getNextPosition(forwardY, forwardX);

    const int dir = snake.getDirection();
    const int dx[4] = {0, 0, -1, 1};
    const int dy[4] = {-1, 1, 0, 0};

    for (int i = 0; i < 3; ++i) {
        if (forwardY <= 0 || forwardY >= MAP_SIZE - 1 ||
            forwardX <= 0 || forwardX >= MAP_SIZE - 1) {
            break;
        }

        blocked.push_back({forwardX, forwardY});
        forwardY += dy[dir];
        forwardX += dx[dir];
    }

    if (gateMgr.isActive) {
        blocked.push_back({gateMgr.gateA.x, gateMgr.gateA.y});
        blocked.push_back({gateMgr.gateB.x, gateMgr.gateB.y});
    }

    return blocked;
}

// ItemType 값을 UI가 그릴 수 있는 맵 타일 상수로 변환
static int tileForItem(ItemType type) {
    if (type == GROWTH) return GROWTH_ITEM;
    if (type == POISON) return POISON_ITEM;
    if (type == FEVER) return FEVER_ITEM;
    return EMPTY;
}

// 스테이지 원본, 게이트, 아이템, 뱀 상태를 합쳐 현재 화면용 gameMap 생성
static void syncSharedMap(const int stageMap[MAP_SIZE][MAP_SIZE],
                          const Snake& snake,
                          const ItemManager& itemMgr,
                          const GateManager& gateMgr) {
    resetSharedMap(stageMap);

    if (gateMgr.isActive) {
        const int gateTile = (gateMgr.gateType == LEN5_GATE) ? CONDITIONAL_GATE : GATE;
        gameMap[gateMgr.gateA.y][gateMgr.gateA.x] = gateTile;
        gameMap[gateMgr.gateB.y][gateMgr.gateB.x] = gateTile;
    }

    for (const auto& item : itemMgr.items) {
        gameMap[item.y][item.x] = tileForItem(item.type);
    }

    snake.placeOnMap(gameMap);
}

// 스테이지별 구조를 고려해 뱀의 시작 좌표를 안전한 빈 공간에서 선택
static std::pair<int, int> findStartPosition(const int stageMap[MAP_SIZE][MAP_SIZE],
                                             int stage) {
    const int centerY = (stage == 3) ? MAP_SIZE - 5 : MAP_SIZE / 2;
    const int centerX = MAP_SIZE / 2;
    int bestY = -1;
    int bestX = -1;
    int bestDistance = MAP_SIZE * MAP_SIZE;

    for (int y = 1; y < MAP_SIZE - 1; ++y) {
        for (int x = 3; x < MAP_SIZE - 2; ++x) {
            if (stageMap[y][x - 3] == EMPTY &&
                stageMap[y][x - 2] == EMPTY &&
                stageMap[y][x - 1] == EMPTY &&
                stageMap[y][x] == EMPTY &&
                stageMap[y][x + 1] == EMPTY &&
                stageMap[y][x + 2] == EMPTY) {
                const int distance = abs(y - centerY) + abs(x - centerX);
                if (distance < bestDistance) {
                    bestDistance = distance;
                    bestY = y;
                    bestX = x;
                }
            }
        }
    }

    if (bestY != -1) {
        return {bestY, bestX};
    }

        return {centerY, centerX};
}

// ncurses 키 입력 값을 Snake 클래스가 사용하는 방향 번호로 변환
static int keyToSnakeDirection(int key) {
    if (key == KEY_UP) return 0;
    if (key == KEY_DOWN) return 1;
    if (key == KEY_LEFT) return 2;
    if (key == KEY_RIGHT) return 3;
    return -1;
}

// 입력 큐에 쌓인 키를 모두 읽고 이번 틱에 사용할 마지막 유효 입력만 반환
static int readLatestInput() {
    int latestKey = ERR;
    int key = getch();

    while (key != ERR) {
        if (key == 'q' || key == 'Q') {
            return key;
        }

        if (key == 'p' || key == 'P' || keyToSnakeDirection(key) != -1) {
            latestKey = key;
        }

        key = getch();
    }

    return latestKey;
}

// 일시정지 팝업을 띄우고 P로 재개하거나 Q로 종료할 때까지 대기
static void waitPauseInput(bool& quit) {
    showPauseMessage();
    while (true) {
        int pauseKey = getch();
        if (pauseKey == 'q' || pauseKey == 'Q') {
            quit = true;
            break;
        }
        if (pauseKey == 'p' || pauseKey == 'P') {
            clear();
            break;
        }
        napms(40);
    }
}

// 단일 입력 키를 종료, 일시정지, 방향 전환 중 하나로 처리
static void handleGameInput(int key, Snake& snake, bool& quit, bool& gameOver) {
    if (key == ERR) return;

    if (key == 'q' || key == 'Q') {
        quit = true;
        return;
    }

    if (key == 'p' || key == 'P') {
        waitPauseInput(quit);
        return;
    }

    const int newDir = keyToSnakeDirection(key);
    if (newDir != -1 && !snake.setDirection(newDir)) {
        gameOver = true;
    }
}

// 틱 사이 대기 시간을 짧게 나누어 입력을 계속 읽고 다음 이동 방향에 즉시 반영
static void waitForNextTick(int delayMs, Snake& snake, bool& quit, bool& gameOver) {
    const int pollMs = 20;
    int waited = 0;

    while (waited < delayMs && !quit && !gameOver) {
        const int key = readLatestInput();
        handleGameInput(key, snake, quit, gameOver);

        if (quit || gameOver) break;

        const int sleepMs = std::min(pollMs, delayMs - waited);
        napms(sleepMs);
        waited += sleepMs;
    }
}

// Snake 방향 번호를 GateManager 방향 enum으로 변환
static Direction snakeToGateDirection(int dir) {
    if (dir == 0) return UP;
    if (dir == 1) return DOWN;
    if (dir == 2) return LEFT;
    if (dir == 3) return RIGHT;
    return NONE_DIR;
}

// GateManager 방향 enum을 Snake 방향 번호로 변환
static int gateToSnakeDirection(Direction dir) {
    if (dir == UP) return 0;
    if (dir == DOWN) return 1;
    if (dir == LEFT) return 2;
    if (dir == RIGHT) return 3;
    return 0;
}

// 게이트 출구 좌표와 출구 방향을 이용해 실제로 뱀이 나올 다음 칸 계산
static void nextByGateDirection(GatePosition gate, Direction dir, int& nextY, int& nextX) {
    const int dx[4] = {0, 1, 0, -1};
    const int dy[4] = {-1, 0, 1, 0};

    nextY = gate.y + dy[dir];
    nextX = gate.x + dx[dir];
}

// 다음 이동 좌표가 현재 꼬리 칸인지 확인하여 정상 이동 가능한 몸통 예외 처리
static bool isTailCell(const Snake& snake, int y, int x) {
    const auto& body = snake.getBody();
    return !body.empty() && body.back().first == y && body.back().second == x;
}

// 현재 스테이지 미션 목표를 모두 달성했는지 검사
static bool isMissionClear(const MissionTarget& mission,
                           int snakeLen,
                           int growthCount,
                           int poisonCount,
                           int gateCount) {
    return snakeLen >= mission.length &&
           growthCount >= mission.growth &&
           poisonCount >= mission.poison &&
           gateCount >= mission.gate;
}

// 현재 길이, 아이템 획득 수, 게이트 통과 수, 시간을 이용해 점수 계산
static int calculateScore(int snakeLen, int growthCount, int poisonCount,
                          int gateCount, int elapsedSec) {
    return std::max(0, snakeLen * 10 + growthCount * 20 -
                       poisonCount * 10 + gateCount * 30 + elapsedSec);
}

// 스테이지가 올라갈수록 빨라지는 틱 지연 시간 반환
static int getStageTickDelay(int stage) {
    const int delayByStage[4] = {280, 240, 210, 180};
    if (stage < 1) return delayByStage[0];
    if (stage > 4) return delayByStage[3];
    return delayByStage[stage - 1];
}

// ncurses 초기화 후 스테이지 루프, 입력 처리, 충돌 판정, UI 갱신 총괄
int main() {
    setlocale(LC_ALL, "");
    srand((unsigned int)time(nullptr));

    initscr();
    initUI();

    bool restartRequested = false;
    bool firstRun = true;
    do {
        restartRequested = false;
        clear();
        if (firstRun) {
            showStartScreen();
            firstRun = false;
        }
        nodelay(stdscr, TRUE);

        bool quit = false;
        bool gameOver = false;
        int finalScore = 0;
        int finalLength = 3;
        int finalElapsed = 0;

        for (int stage = 1; stage <= 4 && !quit && !gameOver;) {
            initStage(stage);

            int stageMap[MAP_SIZE][MAP_SIZE];
            copyStageMap(stageMap);

            const auto start = findStartPosition(stageMap, stage);
            Snake snake(start.first, start.second);
            ItemManager itemMgr;
            GateManager gateMgr;

            int growthCount = 0;
            int poisonCount = 0;
            int gateCount = 0;
            int maxLength = snake.getLength();
            int gatePassHold = 0;

            const std::vector<GatePosition> validWalls = buildValidWalls(stageMap);
            resetSharedMap(stageMap);
            snake.placeOnMap(gameMap);
            gateMgr.updateGates(validWalls, toVectorMap(), false);
            itemMgr.updateItems(MAP_SIZE, MAP_SIZE,
                                buildBlockedPositions(stageMap, snake, gateMgr));
            syncSharedMap(stageMap, snake, itemMgr, gateMgr);

            const auto stageStart = std::chrono::steady_clock::now();

            while (!quit && !gameOver) {
            const auto now = std::chrono::steady_clock::now();
            const int elapsedSec =
                (int)std::chrono::duration_cast<std::chrono::seconds>(now - stageStart).count();
            const int score = calculateScore(snake.getLength(), growthCount,
                                             poisonCount, gateCount, elapsedSec);

            drawScreen(gameMap);
            drawScoreBoard(score, snake.getLength(), maxLength, growthCount,
                           poisonCount, gateCount, elapsedSec, stage);
            drawMissionBoard(snake.getLength(), MISSIONS[stage - 1].length,
                             growthCount, MISSIONS[stage - 1].growth,
                             poisonCount, MISSIONS[stage - 1].poison,
                             gateCount, MISSIONS[stage - 1].gate);

            if (isMissionClear(MISSIONS[stage - 1], snake.getLength(),
                               growthCount, poisonCount, gateCount)) {
                nodelay(stdscr, FALSE);
                showGameClear(score, elapsedSec);
                nodelay(stdscr, TRUE);
                ++stage;
                break;
            }

            int key = readLatestInput();
            handleGameInput(key, snake, quit, gameOver);
            if (quit || gameOver) break;

            int nextY = 0;
            int nextX = 0;
            snake.getNextPosition(nextY, nextX);

            if (nextY < 0 || nextY >= MAP_SIZE || nextX < 0 || nextX >= MAP_SIZE) {
                gameOver = true;
                break;
            }

            const int nextTile = gameMap[nextY][nextX];

            if (nextTile == WALL || nextTile == IMMUNE_WALL) {
                snake.crashInto(nextY, nextX, gameMap);
                drawScreen(gameMap);
                napms(600);
                gameOver = true;
            } else if (nextTile == SNAKE_BODY && !isTailCell(snake, nextY, nextX)) {
                snake.crashInto(nextY, nextX, gameMap);
                drawScreen(gameMap);
                napms(600);
                gameOver = true;
            } else if (nextTile == GROWTH_ITEM || nextTile == POISON_ITEM ||
                       nextTile == FEVER_ITEM) {
                const ItemType itemType = itemMgr.checkCollision(nextX, nextY);

                if (itemType == GROWTH) {
                    snake.grow(nextY, nextX, gameMap);
                    ++growthCount;
                } else if (itemType == POISON) {
                    ++poisonCount;
                    if (!snake.shrink(nextY, nextX, gameMap)) {
                        gameOver = true;
                    }
                } else {
                    snake.move(nextY, nextX, gameMap);
                }
            } else if (nextTile == GATE || nextTile == CONDITIONAL_GATE) {
                if (!gateMgr.canPassConditionalGate(snake.getLength())) {
                    gameOver = true;
                } else {
                    const GatePosition exitGate = gateMgr.getExitGate(nextX, nextY);
                    Direction exitDir = gateMgr.getEdgeExitDirection(exitGate, MAP_SIZE, MAP_SIZE);
                    if (exitDir == NONE_DIR) {
                        exitDir = gateMgr.getInnerExitDirection(
                            snakeToGateDirection(snake.getDirection()), exitGate, toVectorMap());
                    }

                    int exitY = 0;
                    int exitX = 0;
                    if (exitDir == NONE_DIR) {
                        gameOver = true;
                    } else {
                        nextByGateDirection(exitGate, exitDir, exitY, exitX);

                        if (exitY < 0 || exitY >= MAP_SIZE ||
                            exitX < 0 || exitX >= MAP_SIZE ||
                            gameMap[exitY][exitX] == WALL ||
                            gameMap[exitY][exitX] == IMMUNE_WALL ||
                            gameMap[exitY][exitX] == SNAKE_BODY) {
                            gameOver = true;
                        } else {
                            snake.teleport(exitY, exitX, gateToSnakeDirection(exitDir), gameMap);
                            ++gateCount;
                            gatePassHold = snake.getLength();
                        }
                    }
                }
            } else {
                snake.move(nextY, nextX, gameMap);
            }

            maxLength = std::max(maxLength, snake.getLength());
            finalScore = calculateScore(snake.getLength(), growthCount,
                                        poisonCount, gateCount, elapsedSec);
            finalLength = snake.getLength();
            finalElapsed = elapsedSec;

            if (gameOver) break;

            const bool isSnakePassingGate = gatePassHold > 0;
            gateMgr.updateGates(validWalls, toVectorMap(), isSnakePassingGate);
            if (gatePassHold > 0) {
                --gatePassHold;
            }

            itemMgr.updateItems(MAP_SIZE, MAP_SIZE,
                                buildBlockedPositions(stageMap, snake, gateMgr));
            syncSharedMap(stageMap, snake, itemMgr, gateMgr);

            waitForNextTick(getStageTickDelay(stage), snake, quit, gameOver);
            }
        }

        nodelay(stdscr, FALSE);
        if (gameOver) {
            const int endKey = showGameOver(finalScore, finalLength, finalElapsed);
            restartRequested = (endKey == 'r' || endKey == 'R');
        }
    } while (restartRequested);

    cleanupUI();
    endwin();
    return 0;
}

/*
병합 시작 버전과 비교한 변경점:
- 단순히 스테이지 맵만 출력하던 테스트 코드를 실제 게임 루프로 변경
- map, snake, item, gate, ui 모듈을 연결하여 이동, 충돌, 아이템, 게이트, 점수판, 미션판이 함께 동작하도록 변경
- PDF 조건에 맞게 1~4스테이지 진행과 스테이지별 미션 달성 시 다음 스테이지 이동 구현
- 시작 위치를 맵 중앙 근처의 안전한 위치로 고르도록 변경, 3스테이지는 아래쪽 넓은 공간을 우선 사용하도록 조정
- 30x30 맵 크기에 맞춰 스테이지별 이동 속도를 1스테이지 280ms, 2스테이지 240ms, 3스테이지 210ms, 4스테이지 180ms로 조정
- 게임오버 후 R 키로 1스테이지부터 즉시 재시작할 수 있도록 재시작 흐름을 추가
- 게이트 생성 후보를 일반 벽 전체로 열어 외곽 벽과 내부 장애물 벽 모두에 게이트가 생길 수 있도록 수정
- 벽이나 몸통 충돌 시 뱀이 충돌 칸까지 실제로 전진한 최종 프레임을 600ms 보여준 뒤 게임오버 처리
- 방향키 연타로 입력 큐에 오래된 키가 쌓여 방향 전환이 늦게 반영되는 문제를 막기 위해 매 틱 마지막 유효 입력만 처리하도록 변경
- 아이템 생성 차단 위치에 뱀 진행 방향 앞쪽 3칸을 추가하여 스테이지 시작 직후 바로 앞에 독 아이템이 생기는 상황 방지
- 각 헬퍼 함수 위에 역할 설명 주석을 추가하여 병합 흐름을 더 쉽게 파악하도록 정리
- 틱 대기 시간을 20ms 단위로 나누어 입력을 계속 읽도록 바꿔 방향 전환 입력이 한 템포 늦게 반영되는 이질감 완화
*/
