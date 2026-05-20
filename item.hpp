#ifndef ITEM_HPP
#define ITEM_HPP

#include <vector>
#include <deque>
#include <utility>

// PDF 명세에 따른 Map 요소 정의 -> 이걸로 그림그리기 용이
enum MapElement {
    EMPTY = 0,
    WALL = 1,
    IMMUNE_WALL = 2, // 게이트로 변할 수 없는 벽
    SNAKE_HEAD = 3,
    SNAKE_BODY = 4,
    GROWTH_ITEM = 5,
    POISON_ITEM = 6,
    GATE = 7
};

struct Item {
    int x;
    int y;
    int type; 
    int life = 5000; // 5초 기준 (틱 단위에 따라 조정 필요) -> 이건 추가구현 아이디어!

    Item(int y, int x, int type) : x(x), y(y), type(type) {}
};

struct Gate {
    int gx1, gy1, gx2, gy2;
    bool isactive = false;
    // 진입 중인 Gate는 사라지지 않음
    int life = 10000; 

    Gate(int a, int b, int c, int d) {
        this->gy1 = a;
        this->gx1 = b;
        this->gy2 = c;
        this->gx2 = d;
    }
};

class ItemAndGateManager {
private:
    std::vector<Item*> itemdb;
    Gate* gate;
    int wait_delgate;
    int mapSize; // 최소 21x21 이상

    const int dx[4] = { 0, 0, -1, 1 };
    const int dy[4] = { -1, 1, 0, 0 };
    
    // 규칙 4: 중앙 벽 통과 시 회전 우선순위 (진행 -> 시계 -> 반시계 -> 역방향)
    const int turn_seq[4][4] = {
        {0, 3, 2, 1}, // 상(0) -> 상, 우(시계), 좌(반시계), 하
        {1, 2, 3, 0}, // 하(1) -> 하, 좌(시계), 우(반시계), 상
        {2, 0, 1, 3}, // 좌(2) -> 좌, 상(시계), 하(반시계), 우
        {3, 1, 0, 2}  // 우(3) -> 우, 하(시계), 상(반시계), 좌
    };

public:
    ItemAndGateManager(int mapSize = 21);
    ~ItemAndGateManager();

    void update(int gameMap[][25], int tick, int& currentItemCount, int& missionGateCount);
    
    // 일반 WALL(1)만 필터링하여 Gate를 생성하도록 개선
    void generateObjects(int gameMap[][25], int& currentItemCount, const std::deque<std::pair<int, int>>& snake, const std::vector<std::pair<int, int>>& walls);

    int checkCollision(int& nextY, int& nextX, int gameMap[][25], std::deque<std::pair<int, int>>& snake, int& currentItemCount, int& currentDir);

    void clear();
};

#endif // ITEM_HPP
