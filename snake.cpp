#include "snake.hpp"

Snake::Snake(int startY, int startX) {
    // 뱀이 왼쪽을 바라보는 형태로 초기화 (머리 위치 startX, 몸통은 우측으로 이어짐)
    currentDir = 2; // 2: Left
    body.push_back({startY, startX});       // 머리 (Head)
    body.push_back({startY, startX + 1});   // 몸통 (Body)
    body.push_back({startY, startX + 2});   // 꼬리 (Tail)
}

Snake::~Snake() {
    body.clear();
}

bool Snake::setDirection(int newDir) {
    // 1. 진행방향과 같은 방향키 입력은 무시 (상태 유지)
    if (currentDir == newDir) return true;

    // 2. 진행방향의 반대 방향키를 입력하는 경우 실패 (Game Over)
    if ((currentDir == 0 && newDir == 1) || // 상 -> 하
        (currentDir == 1 && newDir == 0) || // 하 -> 상
        (currentDir == 2 && newDir == 3) || // 좌 -> 우
        (currentDir == 3 && newDir == 2)) { // 우 -> 좌
        return false; 
    }

    currentDir = newDir;
    return true;
}

void Snake::getNextPosition(int& nextY, int& nextX) const {
    const int dx[4] = { 0, 0, -1, 1 }; // 상, 하, 좌, 우
    const int dy[4] = { -1, 1, 0, 0 };
    
    // 현재 머리(front)의 좌표에 현재 진행방향 변위를 더함
    nextY = body.front().first + dy[currentDir];
    nextX = body.front().second + dx[currentDir];
}

void Snake::move(int nextY, int nextX, int gameMap[][25]) {
    // 1. 꼬리 지우기 (pop_back)
    int tailY = body.back().first;
    int tailX = body.back().second;
    body.pop_back();
    gameMap[tailY][tailX] = EMPTY;

    // 2. 예전 머리를 몸통 상수(4)로 변경
    int oldHeadY = body.front().first;
    int oldHeadX = body.front().second;
    gameMap[oldHeadY][oldHeadX] = SNAKE_BODY;

    // 3. 새 머리를 진행방향 쪽에 추가 (push_front)
    body.push_front({nextY, nextX});
    gameMap[nextY][nextX] = SNAKE_HEAD;
}

void Snake::grow(int nextY, int nextX, int gameMap[][25]) {
    // Growth Item 효과: 꼬리를 자르지 않음으로써 전체 길이가 1 증가함
    int oldHeadY = body.front().first;
    int oldHeadX = body.front().second;
    gameMap[oldHeadY][oldHeadX] = SNAKE_BODY;

    // 머리만 새 공간에 추가
    body.push_front({nextY, nextX});
    gameMap[nextY][nextX] = SNAKE_HEAD;
}

bool Snake::shrink(int nextY, int nextX, int gameMap[][25]) {
    // Poison Item 효과: 꼬리를 두 번 제거하여 전체 길이가 1 감소함

    // 첫 번째 꼬리 제거 (일반 이동에 의한 꼬리 당겨짐 효과)
    int tailY1 = body.back().first;
    int tailX1 = body.back().second;
    body.pop_back();
    gameMap[tailY1][tailX1] = EMPTY;

    // 두 번째 꼬리 제거 (Poison 페널티로 인한 실제 길이 감소 효과)
    int tailY2 = body.back().first;
    int tailX2 = body.back().second;
    body.pop_back();
    gameMap[tailY2][tailX2] = EMPTY;

    // 머리 전진 처리
    int oldHeadY = body.front().first;
    int oldHeadX = body.front().second;
    gameMap[oldHeadY][oldHeadX] = SNAKE_BODY;

    body.push_front({nextY, nextX});
    gameMap[nextY][nextX] = SNAKE_HEAD;

    // Game Rule #2: 몸의 길이가 3보다 작아지면 실패(Game Over)
    if (body.size() < 3) {
        return false;
    }
    return true;
}

void Snake::teleport(int nextY, int nextX, int newDir, int gameMap[][25]) {
    // Gate를 통과할 때 넘겨받은 새로운 방향으로 갱신 후 기본 이동 수행
    currentDir = newDir;
    move(nextY, nextX, gameMap);
}
