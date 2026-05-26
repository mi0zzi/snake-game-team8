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

    // 가장 위
    gameMap[4][10] = WALL;

    // 위 중간
    for (int x = 8; x <= 12; x++) {
        gameMap[6][x] = WALL;
    }

    // 중앙 위
    for (int x = 6; x <= 14; x++) {
        gameMap[8][x] = WALL;
    }

    // 중앙
    for (int x = 4; x <= 16; x++) {
        gameMap[10][x] = WALL;
    }

    // 중앙 아래
    for (int x = 6; x <= 14; x++) {
        gameMap[12][x] = WALL;
    }

    // 아래 중간
    for (int x = 8; x <= 12; x++) {
        gameMap[14][x] = WALL;
    }

    // 가장 아래
    gameMap[16][10] = WALL;
}


    // ====================
    // Stage 3 : Infinity
    // ====================
    else if (stage == 3) {

        // 왼쪽 링
        for (int x = 3; x <= 8; x++) {

            gameMap[6][x] = WALL;
            gameMap[14][x] = WALL;
        }

        for (int y = 7; y <= 13; y++) {

            gameMap[y][3] = WALL;
            gameMap[y][8] = WALL;
        }


        // 오른쪽 링
        for (int x = 12; x <= 17; x++) {

            gameMap[6][x] = WALL;
            gameMap[14][x] = WALL;
        }

        for (int y = 7; y <= 13; y++) {

            gameMap[y][12] = WALL;
            gameMap[y][17] = WALL;
        }


        // 가운데 연결 벽
        gameMap[9][9] = WALL;
        gameMap[10][10] = WALL;
        gameMap[11][11] = WALL;
    }


    // ====================
    // Stage 4 : Spiral
    // ====================
    else if (stage == 4) {

        // 바깥쪽 벽
        for (int x = 3; x <= 17; x++) {
            gameMap[3][x] = WALL;
        }

        for (int y = 3; y <= 17; y++) {
            gameMap[y][17] = WALL;
        }

        for (int x = 5; x <= 17; x++) {
            gameMap[17][x] = WALL;
        }

        for (int y = 6; y <= 17; y++) {
            gameMap[y][5] = WALL;
        }


        // 안쪽 Spiral 구조
        for (int x = 5; x <= 14; x++) {
            gameMap[6][x] = WALL;
        }

        for (int y = 6; y <= 14; y++) {
            gameMap[y][14] = WALL;
        }

        for (int x = 8; x <= 14; x++) {
            gameMap[14][x] = WALL;
        }

        for (int y = 9; y <= 14; y++) {
            gameMap[y][8] = WALL;
        }

        for (int x = 8; x <= 11; x++) {
            gameMap[9][x] = WALL;
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

                mvaddwstr(y + 3, x * 2 + 5, L"■");
            }

            // Immune Wall 출력
            else if (gameMap[y][x] == IMMUNE_WALL) {

                mvaddwstr(y + 3, x * 2 + 5, L"□");
            }

            // 빈 공간 출력
            else {

                mvprintw(y + 3, x * 2 + 5, "  ");
            }
        }
    }
}