#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <deque>
#include <utility>
#include "constants.hpp"

class Snake {
private:
    // 뱀의 좌표를 저장하는 덱. front()가 머리, back()이 꼬리.
    std::deque<std::pair<int, int>> body;

    // 현재 진행 방향 (0:상, 1:하, 2:좌, 3:우)
    int currentDir;

public:
    // 생성자: 시작 좌표를 받아 기본 길이 3의 뱀을 세팅
    Snake(int startY, int startX);
    ~Snake();

    // 뱀의 진행 방향을 설정. (Game Rule #1: 반대 방향 이동 실패 조건 처리)
    // 반환값: false면 진행방향의 반대키 입력으로 인한 게임 오버
    bool setDirection(int newDir);

    // 다음 틱에 머리가 도달할 예정인 좌표를 계산하여 참조(Reference)로 반환
    void getNextPosition(int& nextY, int& nextX) const;

    // 빈 공간(EMPTY)으로의 일반적인 이동
    void move(int nextY, int nextX, int gameMap[][MAP_SIZE]);

    // Growth Item 획득 시 이동 (Game Rule #2: 길이 1 증가)
    void grow(int nextY, int nextX, int gameMap[][MAP_SIZE]);

    // Poison Item 획득 시 이동 (Game Rule #2: 길이 1 감소)
    // 반환값: false면 꼬리 감소 후 길이가 3 미만이 되어 게임 오버
    bool shrink(int nextY, int nextX, int gameMap[][MAP_SIZE]);

    // Gate 통과 시 텔레포트 및 방향 갱신 처리
    void teleport(int nextY, int nextX, int newDir, int gameMap[][MAP_SIZE]);

    // 충돌 순간 표시용 이동: 벽/몸통에 머리가 닿은 최종 프레임 생성
    void crashInto(int nextY, int nextX, int gameMap[][MAP_SIZE]);

    // 현재 뱀 좌표를 맵 배열에 반영
    void placeOnMap(int gameMap[][MAP_SIZE]) const;

    // Getter 메서드
    int getLength() const { return body.size(); }
    int getDirection() const { return currentDir; }
    const std::deque<std::pair<int, int>>& getBody() const { return body; }
};

#endif // SNAKE_HPP

/*
병합 시작 버전과 비교한 변경점:
- 고정 크기 25 배열 매개변수를 MAP_SIZE 기준 배열로 30x30으로 변경
- EMPTY, SNAKE_HEAD, SNAKE_BODY 같은 맵 상수를 직접 사용하기 위해 constants.hpp를 포함하도록 변경
- 매 틱마다 뱀 좌표를 공유 gameMap에 다시 반영하기 위한 placeOnMap() 함수 추가
- 충돌 순간 뱀이 실제 충돌 칸까지 도달한 화면을 보여주기 위해 crashInto() 함수 추가
*/
