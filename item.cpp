#include "item.hpp"

// 주어진 좌표에 이미 생성된 아이템이 있는지 중복 여부 검사
bool ItemManager::isDuplicatePosition(int x, int y) const {
    for (const auto& it : items) {
        if (it.x == x && it.y == y) return true;
    }
    return false;
}

// 주어진 좌표가 뱀의 몸통이나 벽 등 장애물 위치와 겹치는지 검사
bool ItemManager::isBlockedPosition(int x, int y, const std::vector<Position>& blockedPositions) const {
    for (const auto& p : blockedPositions) {
        if (p.x == x && p.y == y) return true;
    }
    return false;
}

// 해당 좌표가 아이템을 생성할 수 있는 유효한 빈 공간인지 최종 확인
bool ItemManager::canPlaceAt(int x, int y, const std::vector<Position>& blockedPositions) const {
    if (isDuplicatePosition(x, y)) return false;
    if (isBlockedPosition(x, y, blockedPositions)) return false;
    return true;
}

// 맵 위에 미획득 상태의 FEVER 아이템이 존재하는지 확인 (중복 생성 방지용)
bool ItemManager::hasFeverItemOnMap() const {
    for (const auto& it : items) {
        if (it.type == FEVER) return true;
    }
    return false;
}

// 현재 피버타임 효과가 지속 중인지 상태 반환
bool ItemManager::isFeverActive() const {
    return feverActive;
}

// 피버타임 종료까지 남은 턴 수 반환
int ItemManager::getFeverRemainingTurns() const {
    return feverRemainingTurns;
}

// 피버타임 발동 시 맵을 비우고 성장(GROWTH) 아이템으로만 강제 스폰하는 특수 처리 함수
void ItemManager::spawnForcedGrowthItems(int mapWidth, int mapHeight, const std::vector<Position>& blockedPositions) {
    int tries = 0;
    // 빈 공간을 찾아 최대치(MAX_ITEMS)까지 GROWTH 아이템으로 채움
    while ((int)items.size() < MAX_ITEMS && tries < MAX_SPAWN_TRIES) {
        const int x = (rand() % (mapWidth - 2)) + 1;
        const int y = (rand() % (mapHeight - 2)) + 1;

        if (!canPlaceAt(x, y, blockedPositions)) {
            ++tries;
            continue;
        }

        Item it;
        it.x = x;
        it.y = y;
        it.type = GROWTH;
        it.lifetime = ITEM_LIFETIME;
        items.push_back(it);
        ++tries;
    }
}

// 맵 내 빈 공간에 일반 아이템과 특수(FEVER) 아이템을 확률에 따라 무작위 생성
void ItemManager::generateItem(int mapWidth, int mapHeight, const std::vector<Position>& blockedPositions) {
    if (mapWidth <= 2 || mapHeight <= 2) return;

    int tries = 0;
    while ((int)items.size() < MAX_ITEMS && tries < MAX_SPAWN_TRIES) {
        const int x = (rand() % (mapWidth - 2)) + 1;
        const int y = (rand() % (mapHeight - 2)) + 1;

        if (!canPlaceAt(x, y, blockedPositions)) {
            ++tries;
            continue;
        }

        Item newItem;
        newItem.x = x;
        newItem.y = y;

        // 피버타임 중에는 종류를 추첨하지 않고 무조건 GROWTH로 고정
        if (feverActive) {
            newItem.type = GROWTH;
            newItem.lifetime = ITEM_LIFETIME;
        } else {
            // 맵에 FEVER 아이템이 하나도 없을 때만 5% 확률로 FEVER 스폰 판정
            const bool canSpawnFever = !hasFeverItemOnMap();
            const int roll = rand() % 1000; // 0~999 범위 난수

            if (canSpawnFever && roll < FEVER_CHANCE_PER_THOUSAND) {
                newItem.type = FEVER;
                newItem.lifetime = FEVER_ITEM_LIFETIME;
            } else {
                // FEVER가 아니면 일반 아이템인 GROWTH와 POISON 중 각각 50% 확률로 결정
                const int normalRoll = rand() % 2;
                newItem.type = (normalRoll == 0) ? GROWTH : POISON;
                newItem.lifetime = ITEM_LIFETIME;
            }
        }

        items.push_back(newItem);
        ++tries;
    }
}

// 뱀의 머리 좌표를 받아 아이템 획득(충돌) 여부를 검사하고 해당 아이템 타입을 반환
ItemType ItemManager::checkCollision(int headX, int headY) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->x == headX && it->y == headY) {
            const ItemType type = it->type;
            items.erase(it); // 획득한 아이템을 리스트에서 즉시 제거

            // FEVER 아이템 획득 시 진행 로직 꼬임 방지를 위해 당장 효과를 적용하지 않고 트리거 플래그만 세팅
            if (type == FEVER) {
                pendingFeverTrigger = true;
            }
            return type;
        }
    }
    return NONE_ITEM;
}

// 스테이지 기믹: 특정 턴에 메인 루프에서 호출 시 맵 위 아이템(GROWTH <-> POISON) 상태 일괄 반전
void ItemManager::handleTurnStartInvert(bool requestInvert) {
    if (!requestInvert) return;

    // 보호 규칙: 유저에게 유리한 피버 진행 중이거나 대기 중인 FEVER가 있으면 시스템 반전 명령을 강제 무시
    if (feverActive || hasFeverItemOnMap()) return;

    for (auto& it : items) {
        if (it.type == GROWTH) it.type = POISON;
        else if (it.type == POISON) it.type = GROWTH;
    }
}

// 매 턴 호출되어 전체 아이템 수명 관리, 피버타임 지속 갱신 및 재생성을 총괄하는 메인 루틴
void ItemManager::updateItems(int mapWidth, int mapHeight, const std::vector<Position>& blockedPositions) {
    if (mapWidth <= 2 || mapHeight <= 2) return;

    suppressSpawnThisTurn = false;

    // 1. 모든 기존 아이템 수명 1 감소 및 소명이 다한 아이템 자동 삭제
    for (auto it = items.begin(); it != items.end();) {
        it->lifetime--;
        if (it->lifetime <= 0) it = items.erase(it);
        else ++it;
    }

    // 2. FEVER 트리거 발동 처리: 기존 아이템 완전 제거 및 40턴 제한시간 시작, GROWTH 보상 스폰
    if (pendingFeverTrigger) {
        pendingFeverTrigger = false;
        feverActive = true;
        feverRemainingTurns = FEVER_DURATION;

        items.clear();
        spawnForcedGrowthItems(mapWidth, mapHeight, blockedPositions);
    }

    // 3. FEVER 제한 시간 감소 및 종료 제어
    if (feverActive) {
        feverRemainingTurns--; // 획득한 현재 턴부터 즉시 차감 시작

        // 피버타임 종료 턴의 예외 처리 (A안 확정 로직)
        if (feverRemainingTurns <= 0) {
            feverActive = false;
            feverRemainingTurns = 0;
            items.clear(); // 맵을 깨끗하게 비움
            suppressSpawnThisTurn = true; // 일반 아이템과 생성 타이밍이 겹치는 것을 막기 위해 이번 턴은 재생성 건너뜀
        }
    }

    // 4. 부족해진 개수만큼 새로운 아이템을 맵에 보충 (단, 피버 종료 턴은 제외)
    if (!suppressSpawnThisTurn) {
        generateItem(mapWidth, mapHeight, blockedPositions);
    }
}