#include <ncurses.h>
#include "map.hpp"

int main() {

    // ncurses 시작
    initscr();

    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);


    // 맵 초기화 및 출력
    initMap();

    drawMap();


    // 화면 갱신
    refresh();

    // 입력 대기
    getch();

    // ncurses 종료
    endwin();

    return 0;
}