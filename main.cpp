#include <locale.h>
#include <ncursesw/ncurses.h>

#include "map.hpp"

int main() {

    // 유니코드 출력 설정
    setlocale(LC_ALL, "");

    // ncurses 시작
    initscr();

    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);


    // 테스트할 Stage 번호
    int stage = 4;


    // Stage 초기화
    initStage(stage);

    // 맵 출력
    drawMap(stage);


    refresh();

    // 키 입력 대기
    getch();

    // ncurses 종료
    endwin();

    return 0;
}