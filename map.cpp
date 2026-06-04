#include "map.hpp"
#include <ncursesw/ncurses.h>


// 게임 맵 배열
int gameMap[MAP_SIZE][MAP_SIZE];


// 기본 맵 초기화
void initMap() {

    for (int y = 0; y < MAP_SIZE; y++) {

        for (int x = 0; x < MAP_SIZE; x++) {

            // 테두리 벽 생성
            if (y == 0 || y == MAP_SIZE - 1 ||
                x == 0 || x == MAP_SIZE - 1) {

                gameMap[y][x] = WALL;
            }

            // 내부는 빈 공간
            else {
                gameMap[y][x] = EMPTY;
            }
        }
    }


    // 모서리는 Immune Wall로 설정
    gameMap[0][0] = IMMUNE_WALL;
    gameMap[0][MAP_SIZE - 1] = IMMUNE_WALL;

    gameMap[MAP_SIZE - 1][0] = IMMUNE_WALL;
    gameMap[MAP_SIZE - 1][MAP_SIZE - 1] = IMMUNE_WALL;
}



// Stage별 맵 생성
void initStage(int stage) {

    // 기본 맵 생성
    initMap();


    // ====================
    // Stage 1 : 기본 맵
    // ====================
    if (stage == 1) {

        return;
    }


    // ====================
    // Stage 2 : Crystal
    // ====================
    else if (stage == 2) {

        // 30x30 맵 중앙에 맞춘 확장 Crystal 구조
        const int c = MAP_SIZE / 2;

        gameMap[5][c] = WALL;

        for (int x = c - 3; x <= c + 3; x++) {
            gameMap[8][x] = WALL;
        }

        for (int x = c - 6; x <= c + 6; x++) {
            gameMap[11][x] = WALL;
        }

        for (int x = c - 9; x <= c + 9; x++) {
            gameMap[14][x] = WALL;
        }

        for (int x = c - 6; x <= c + 6; x++) {
            gameMap[17][x] = WALL;
        }

        for (int x = c - 3; x <= c + 3; x++) {
            gameMap[20][x] = WALL;
        }

        gameMap[23][c] = WALL;
    }


    // ====================
    // Stage 3 : Infinity
    // ====================
    else if (stage == 3) {

        // 30x30 맵 중앙에 맞춘 확장 Infinity 구조
        for (int x = 4; x <= 12; x++) {

            gameMap[8][x] = WALL;
            gameMap[20][x] = WALL;
        }

        for (int y = 9; y <= 19; y++) {

            gameMap[y][4] = WALL;
            gameMap[y][12] = WALL;
        }


        // 오른쪽 링
        for (int x = 17; x <= 25; x++) {

            gameMap[8][x] = WALL;
            gameMap[20][x] = WALL;
        }

        for (int y = 9; y <= 19; y++) {

            gameMap[y][17] = WALL;
            gameMap[y][25] = WALL;
        }


        // 가운데 연결 벽
        gameMap[13][13] = WALL;
        gameMap[14][14] = WALL;
        gameMap[15][15] = WALL;
        gameMap[16][16] = WALL;
    }


    // ====================
    // Stage 4 : Spiral
    // ====================
    else if (stage == 4) {

        // 30x30 맵 중앙에 맞춘 확장 Spiral 구조
        for (int x = 4; x <= 25; x++) {
            gameMap[4][x] = WALL;
        }

        for (int y = 4; y <= 25; y++) {
            gameMap[y][25] = WALL;
        }

        for (int x = 7; x <= 25; x++) {
            gameMap[25][x] = WALL;
        }

        for (int y = 8; y <= 25; y++) {
            gameMap[y][7] = WALL;
        }


        // 안쪽 Spiral 구조
        for (int x = 7; x <= 21; x++) {
            gameMap[8][x] = WALL;
        }

        for (int y = 8; y <= 21; y++) {
            gameMap[y][21] = WALL;
        }

        for (int x = 11; x <= 21; x++) {
            gameMap[21][x] = WALL;
        }

        for (int y = 12; y <= 21; y++) {
            gameMap[y][11] = WALL;
        }

        for (int x = 11; x <= 17; x++) {
            gameMap[12][x] = WALL;
        }
    }
}



// 맵 출력
void drawMap(int stage) {

    // Stage 표시
    mvprintw(1, 50, "Stage : %d", stage);


    for (int y = 0; y < MAP_SIZE; y++) {

        for (int x = 0; x < MAP_SIZE; x++) {

            // 일반 벽 출력
            if (gameMap[y][x] == WALL) {

                mvprintw(y + 3, x * 2 + 5, "■ ");
            }

            // Immune Wall 출력
            else if (gameMap[y][x] == IMMUNE_WALL) {

                mvprintw(y + 3, x * 2 + 5, "▩ ");
            }

            // 빈 공간 출력
            else {

                mvprintw(y + 3, x * 2 + 5, "  ");
            }
        }
    }
}

/*
병합 시작 버전과 비교한 변경점:
- MAP_SIZE가 30으로 확장된 것에 맞춰 Stage 2 Crystal, Stage 3 Infinity, Stage 4 Spiral 장애물 좌표를 30x30 중앙 기준으로 재배치
- 기존 21x21 기준으로 작게 배치되던 장애물 패턴을 더 큰 맵에 맞게 확장
- drawMap()의 벽 출력도 UI와 동일하게 2칸 폭 문자로 맞춰 잔상과 정렬 문제 완화
*/
