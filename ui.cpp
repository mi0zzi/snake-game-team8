#include "ui.hpp"
#include <ncurses.h>
#include <string>

// 타일 값 → 출력 문자열 반환
static const char* tileChar(int tile) {
    switch (tile) {
        case WALL:        return CHAR_WALL;
        case IMMUNE_WALL: return CHAR_IWALL;
        case SNAKE_HEAD:  return CHAR_HEAD;
        case SNAKE_BODY:  return CHAR_BODY;
        case GROWTH_ITEM: return CHAR_GROWTH;
        case POISON_ITEM: return CHAR_POISON;
        case GATE:        return CHAR_GATE;
        default:          return CHAR_EMPTY;
    }
}

// 타일 값 → 색상 쌍 ID 반환
static int tileColor(int tile) {
    switch (tile) {
        case WALL:        return COLOR_WALL_PAIR;
        case IMMUNE_WALL: return COLOR_IWALL_PAIR;
        case SNAKE_HEAD:  return COLOR_HEAD_PAIR;
        case SNAKE_BODY:  return COLOR_BODY_PAIR;
        case GROWTH_ITEM: return COLOR_GROWTH_PAIR;
        case POISON_ITEM: return COLOR_POISON_PAIR;
        case GATE:        return COLOR_GATE_PAIR;
        default:          return 0;   // 기본 색상
    }
}

// 초 → "MM:SS" 형식 문자열
static std::string formatTime(int seconds) {
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", seconds / 60, seconds % 60);
    return std::string(buf);
}


void initUI() {
    // ncurses 색상 지원 여부 확인 후 색상 쌍 초기화
    if (has_colors()) {
        start_color();
        init_pair(COLOR_WALL_PAIR,    COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_IWALL_PAIR,   COLOR_CYAN,    COLOR_BLACK);
        init_pair(COLOR_HEAD_PAIR,    COLOR_GREEN,   COLOR_BLACK);
        init_pair(COLOR_BODY_PAIR,    COLOR_GREEN,   COLOR_BLACK);
        init_pair(COLOR_GROWTH_PAIR,  COLOR_YELLOW,  COLOR_BLACK);
        init_pair(COLOR_POISON_PAIR,  COLOR_RED,     COLOR_BLACK);
        init_pair(COLOR_GATE_PAIR,    COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_UI_PAIR,      COLOR_CYAN,    COLOR_BLACK);
    }
}

void drawScreen(const int map[MAP_SIZE][MAP_SIZE]) {
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            int tile      = map[y][x];
            int colorPair = tileColor(tile);
            int screenX   = x * 2;   // 타일 하나가 2칸 너비

            if (colorPair != 0)
                attron(COLOR_PAIR(colorPair));

            mvprintw(y, screenX, "%s", tileChar(tile));

            if (colorPair != 0)
                attroff(COLOR_PAIR(colorPair));
        }
    }
    refresh();
}

void drawPanel(int score, int snakeLen, int elapsedSec, int level) {
    int px = PANEL_X;
    int py = PANEL_Y;

    attron(COLOR_PAIR(COLOR_UI_PAIR) | A_BOLD);

    // 패널 제목
    mvprintw(py,     px, "┌──────────────┐");
    mvprintw(py + 1, px, "│  SNAKE GAME  │");
    mvprintw(py + 2, px, "├──────────────┤");

    attroff(A_BOLD);

    // 게임 정보
    mvprintw(py + 3,  px, "│ Level  : %-4d │", level);
    mvprintw(py + 4,  px, "│ Score  : %-4d │", score);
    mvprintw(py + 5,  px, "│ Length : %-4d │", snakeLen);
    mvprintw(py + 6,  px, "│ Time   : %-5s│", formatTime(elapsedSec).c_str());
    mvprintw(py + 7,  px, "├──────────────┤");

    // 범례
    mvprintw(py + 8,  px, "│    LEGEND    │");
    mvprintw(py + 9,  px, "│              │");

    attroff(COLOR_PAIR(COLOR_UI_PAIR));

    attron(COLOR_PAIR(COLOR_HEAD_PAIR));
    mvprintw(py + 10, px, "│ @ Snake Head │");
    attroff(COLOR_PAIR(COLOR_HEAD_PAIR));

    attron(COLOR_PAIR(COLOR_GROWTH_PAIR));
    mvprintw(py + 11, px, "│ G Growth (+) │");
    attroff(COLOR_PAIR(COLOR_GROWTH_PAIR));

    attron(COLOR_PAIR(COLOR_POISON_PAIR));
    mvprintw(py + 12, px, "│ P Poison (-) │");
    attroff(COLOR_PAIR(COLOR_POISON_PAIR));

    attron(COLOR_PAIR(COLOR_GATE_PAIR));
    mvprintw(py + 13, px, "│ D Gate       │");
    attroff(COLOR_PAIR(COLOR_GATE_PAIR));

    attron(COLOR_PAIR(COLOR_UI_PAIR));
    mvprintw(py + 14, px, "└──────────────┘");
    attroff(COLOR_PAIR(COLOR_UI_PAIR));

    refresh();
}

void showGameOver(int finalScore, int snakeLen, int elapsedSec) {
    int centerY = MAP_SIZE / 2 - 3;
    int centerX = MAP_SIZE - 8;

    attron(COLOR_PAIR(COLOR_POISON_PAIR) | A_BOLD);
    mvprintw(centerY,     centerX, "  ╔══════════════╗  ");
    mvprintw(centerY + 1, centerX, "  ║  GAME  OVER  ║  ");
    mvprintw(centerY + 2, centerX, "  ╠══════════════╣  ");
    attroff(A_BOLD);
    mvprintw(centerY + 3, centerX, "  ║ Score : %-4d ║  ", finalScore);
    mvprintw(centerY + 4, centerX, "  ║ Len   : %-4d ║  ", snakeLen);
    mvprintw(centerY + 5, centerX, "  ║ Time  : %-5s║  ", formatTime(elapsedSec).c_str());
    mvprintw(centerY + 6, centerX, "  ║              ║  ");
    mvprintw(centerY + 7, centerX, "  ║  Press any   ║  ");
    mvprintw(centerY + 8, centerX, "  ║    key...    ║  ");
    mvprintw(centerY + 9, centerX, "  ╚══════════════╝  ");
    attroff(COLOR_PAIR(COLOR_POISON_PAIR));

    refresh();
    getch();   // 아무 키나 누를 때까지 대기
}

void showGameClear(int finalScore, int elapsedSec) {
    int centerY = MAP_SIZE / 2 - 3;
    int centerX = MAP_SIZE - 8;

    attron(COLOR_PAIR(COLOR_GROWTH_PAIR) | A_BOLD);
    mvprintw(centerY,     centerX, "  ╔══════════════╗  ");
    mvprintw(centerY + 1, centerX, "  ║ GAME  CLEAR! ║  ");
    mvprintw(centerY + 2, centerX, "  ╠══════════════╣  ");
    attroff(A_BOLD);
    mvprintw(centerY + 3, centerX, "  ║ Score : %-4d ║  ", finalScore);
    mvprintw(centerY + 4, centerX, "  ║ Time  : %-5s║  ", formatTime(elapsedSec).c_str());
    mvprintw(centerY + 5, centerX, "  ║              ║  ");
    mvprintw(centerY + 6, centerX, "  ║  Press any   ║  ");
    mvprintw(centerY + 7, centerX, "  ║    key...    ║  ");
    mvprintw(centerY + 8, centerX, "  ╚══════════════╝  ");
    attroff(COLOR_PAIR(COLOR_GROWTH_PAIR));

    refresh();
    getch();
}

void cleanupUI() {
    // 현재는 별도 동적 자원 없음
    // 추후 WINDOW* 등을 사용할 경우 delwin() 호출 위치
}
