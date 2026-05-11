#include <ncurses.h>

int main() {
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    mvprintw(0, 0, "Snake Game Team 8");

    refresh();

    getch();
    endwin();

    return 0;
}