#include "snake.hpp"

// 시작 좌표를 기준으로 길이 3의 뱀을 만들고 초기 진행 방향을 왼쪽으로 설정
Snake::Snake(int startY, int startX) {
    // 뱀이 왼쪽을 바라보는 형태로 초기화 (머리 위치 startX, 몸통은 우측으로 이어짐)
    currentDir = 2; // 2: Left
    body.push_back({startY, startX});       // 머리 (Head)
    body.push_back({startY, startX + 1});   // 몸통 (Body)
    body.push_back({startY, startX + 2});   // 꼬리 (Tail)
}

// 뱀 좌표 컨테이너를 비워 객체 종료 시 상태 정리
Snake::~Snake() {
    body.clear();
}

// 새 방향 입력이 현재 방향과 반대인지 검사하고, 유효하면 진행 방향 변경
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

// 현재 머리 좌표와 진행 방향을 이용해 다음 틱에 도착할 좌표 계산
void Snake::getNextPosition(int& nextY, int& nextX) const {
    const int dx[4] = { 0, 0, -1, 1 }; // 상, 하, 좌, 우
    const int dy[4] = { -1, 1, 0, 0 };

    // 현재 머리(front)의 좌표에 현재 진행방향 변위를 더함
    nextY = body.front().first + dy[currentDir];
    nextX = body.front().second + dx[currentDir];
}

// 일반 이동 처리: 꼬리를 제거하고 새 머리를 추가하여 길이 유지
void Snake::move(int nextY, int nextX, int gameMap[][MAP_SIZE]) {
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

// 성장 아이템 이동 처리: 꼬리를 제거하지 않고 새 머리를 추가하여 길이 1 증가
void Snake::grow(int nextY, int nextX, int gameMap[][MAP_SIZE]) {
    // Growth Item 효과: 꼬리를 자르지 않음으로써 전체 길이가 1 증가함
    int oldHeadY = body.front().first;
    int oldHeadX = body.front().second;
    gameMap[oldHeadY][oldHeadX] = SNAKE_BODY;

    // 머리만 새 공간에 추가
    body.push_front({nextY, nextX});
    gameMap[nextY][nextX] = SNAKE_HEAD;
}

// 독 아이템 이동 처리: 일반 이동보다 꼬리를 하나 더 제거하여 길이 1 감소
bool Snake::shrink(int nextY, int nextX, int gameMap[][MAP_SIZE]) {
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

// 게이트 통과 후 새 방향으로 바꾼 뒤 출구 다음 칸으로 이동
void Snake::teleport(int nextY, int nextX, int newDir, int gameMap[][MAP_SIZE]) {
    // Gate를 통과할 때 넘겨받은 새로운 방향으로 갱신 후 기본 이동 수행
    currentDir = newDir;
    move(nextY, nextX, gameMap);
}

// 충돌 직전 마지막 프레임을 보여주기 위해 뱀을 충돌 칸까지 한 칸 전진
void Snake::crashInto(int nextY, int nextX, int gameMap[][MAP_SIZE]) {
    int tailY = body.back().first;
    int tailX = body.back().second;
    body.pop_back();
    gameMap[tailY][tailX] = EMPTY;

    int oldHeadY = body.front().first;
    int oldHeadX = body.front().second;
    gameMap[oldHeadY][oldHeadX] = SNAKE_BODY;

    body.push_front({nextY, nextX});
    gameMap[nextY][nextX] = SNAKE_HEAD;
}

// 현재 body 덱에 저장된 머리와 몸통 좌표를 공유 gameMap에 다시 반영
void Snake::placeOnMap(int gameMap[][MAP_SIZE]) const {
    if (body.empty()) return;

    gameMap[body.front().first][body.front().second] = SNAKE_HEAD;
    for (size_t i = 1; i < body.size(); ++i) {
        gameMap[body[i].first][body[i].second] = SNAKE_BODY;
    }
}

/*
병합 시작 버전과 비교한 변경점:
- move, grow, shrink, teleport 함수의 맵 배열 크기를 25 기준에서 MAP_SIZE 기준으로 변경
- placeOnMap()을 구현하여 뱀 머리와 몸통을 공유 gameMap에 다시 그릴 수 있도록 변경
- crashInto()를 구현하여 벽이나 몸통 충돌 시 마지막 이동 프레임을 보여준 뒤 게임오버 처리
*/
