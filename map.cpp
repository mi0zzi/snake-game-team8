#include "map.hpp"
#include <ncurses.h>

// 게임 맵 배열
int gameMap[MAP_SIZE][MAP_SIZE];


// 맵 초기화
void initMap() {

    for (int y = 0; y < MAP_SIZE; y++) {

        for (int x = 0; x < MAP_SIZE; x++) {

            // 테두리는 벽으로 설정
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
}


// 맵 출력
void drawMap() {

    for (int y = 0; y < MAP_SIZE; y++) {

        for (int x = 0; x < MAP_SIZE; x++) {

            // 벽 출력
            if (gameMap[y][x] == WALL) {
                mvprintw(y, x * 2, "##");
            }

            // 빈 공간 출력
            else {
                mvprintw(y, x * 2, "  ");
            }
        }
    }
}