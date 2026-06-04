#ifndef ITEM_HPP
#define ITEM_HPP

#include <vector>
#include <cstdlib>

// 맵에 등장할 수 있는 아이템의 종류 (성장, 독, 피버, 없음)
enum ItemType {
    GROWTH,
    POISON,
    FEVER,
    NONE_ITEM
};

// 아이템 생성 시 장애물(뱀, 벽 등)의 위치를 확인하기 위한 좌표 구조체
struct Position {
    int x;
    int y;
};

// 개별 아이템의 위치, 종류, 남은 수명 데이터를 담는 구조체
struct Item {
    int x;
    int y;
    ItemType type;
    int lifetime;
};

class ItemManager {
public:
    std::vector<Item> items; // 현재 맵에 활성화된 아이템 목록

    // 아이템 생성 및 유지 관련 기본 설정 상수
    const int MAX_ITEMS = 3;
    const int ITEM_LIFETIME = 50;
    const int FEVER_ITEM_LIFETIME = 25;
    const int FEVER_DURATION = 40;
    const int FEVER_CHANCE_PER_THOUSAND = 50; // 5.0% 확률
    const int MAX_SPAWN_TRIES = 200; // 무한 루프 방지용 빈 공간 탐색 최대 시도 횟수

    // 핵심 동작 메서드
    void generateItem(int mapWidth, int mapHeight, const std::vector<Position>& blockedPositions);
    ItemType checkCollision(int headX, int headY);
    void updateItems(int mapWidth, int mapHeight, const std::vector<Position>& blockedPositions);

    // 스테이지 기믹 (아이템 상태 반전) 처리
    void handleTurnStartInvert(bool requestInvert);

    // 외부(Main/UI)에서 피버타임 상태를 확인하고 화면에 그리기 위한 Getter 메서드
    bool isFeverActive() const;
    bool hasFeverItemOnMap() const;
    int getFeverRemainingTurns() const;

private:
    // 피버타임 관련 상태 제어 플래그 및 변수
    bool feverActive = false;
    int feverRemainingTurns = 0;
    bool pendingFeverTrigger = false;   // FEVER 획득 후 즉시 발동을 예약하는 내부 트리거
    bool suppressSpawnThisTurn = false; // 피버 종료 턴 등 특정 상황에서 아이템 스폰을 일시 중지하는 플래그

    // 아이템 스폰 위치 유효성 검사 로직 (내부 전용)
    bool isDuplicatePosition(int x, int y) const;
    bool isBlockedPosition(int x, int y, const std::vector<Position>& blockedPositions) const;
    bool canPlaceAt(int x, int y, const std::vector<Position>& blockedPositions) const;

    // 피버타임 발동 시 기존 아이템을 지우고 GROWTH만 확정 스폰하는 특수 함수
    void spawnForcedGrowthItems(int mapWidth, int mapHeight, const std::vector<Position>& blockedPositions);
};

#endif