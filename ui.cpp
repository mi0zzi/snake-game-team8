#include "ui.hpp"
#include <ncurses.h>
#include <algorithm>
#include <cstdio>
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
    if (seconds < 0) {
        seconds = 0;
    }

    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d", seconds / 60, seconds % 60);
    return std::string(buf);
}

static const char* missionMark(bool achieved) {
    return achieved ? "OK" : "  ";
}

static void printValue(int y, int x, const char* fmt, int value) {
    attron(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);
    mvprintw(y, x, fmt, value);
    attroff(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);
}

static void printValueText(int y, int x, const char* value) {
    attron(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);
    mvprintw(y, x, "%7s", value);
    attroff(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);
}

static void printMissionLine(int y, const char* label, int current, int target) {
    int px = PANEL_X;
    bool achieved = current >= target;
    int statusColor = achieved ? COLOR_DONE_PAIR : COLOR_UI_PAIR;

    attron(COLOR_PAIR(COLOR_UI_PAIR));
    mvprintw(y, px, "│ %-8s          [  ] │", label);
    attroff(COLOR_PAIR(COLOR_UI_PAIR));

    attron(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);
    mvprintw(y, px + 11, "%3d/%-3d", current, target);
    attroff(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);

    attron(COLOR_PAIR(statusColor) | A_BOLD);
    mvprintw(y, px + 20, "%s", missionMark(achieved));
    attroff(COLOR_PAIR(statusColor) | A_BOLD);
}

static void clearPanelArea() {
    for (int y = PANEL_Y; y < PANEL_Y + 28; ++y) {
        move(y, PANEL_X);
        clrtoeol();
    }
}

static void drawMapFrame() {
    int top = MAP_ORIGIN_Y - 1;
    int left = MAP_ORIGIN_X - 1;
    int right = MAP_ORIGIN_X + MAP_SIZE * 2;
    int bottom = MAP_ORIGIN_Y + MAP_SIZE;

    attron(COLOR_PAIR(COLOR_PANEL_PAIR) | A_BOLD);
    mvprintw(top - 1, left + 2, "SNAKE GAME");

    mvaddch(top, left, ACS_ULCORNER);
    mvaddch(top, right, ACS_URCORNER);
    mvaddch(bottom, left, ACS_LLCORNER);
    mvaddch(bottom, right, ACS_LRCORNER);

    for (int x = left + 1; x < right; ++x) {
        mvaddch(top, x, ACS_HLINE);
        mvaddch(bottom, x, ACS_HLINE);
    }

    for (int y = top + 1; y < bottom; ++y) {
        mvaddch(y, left, ACS_VLINE);
        mvaddch(y, right, ACS_VLINE);
    }
    attron(COLOR_PAIR(COLOR_TITLE_PAIR) | A_BOLD);
    mvprintw(top - 1, left + 2, "SNAKE GAME");
    attroff(COLOR_PAIR(COLOR_TITLE_PAIR) | A_BOLD);
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR) | A_BOLD);
}

static void drawLegend(int startY) {
    int px = PANEL_X;

    attron(COLOR_PAIR(COLOR_PANEL_PAIR) | A_BOLD);
    mvprintw(startY,     px, "┌──────────────────────┐");
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR) | A_BOLD);

    attron(COLOR_PAIR(COLOR_TITLE_PAIR) | A_BOLD);
    mvprintw(startY + 1, px, "│       SYMBOLS        │");
    attroff(COLOR_PAIR(COLOR_TITLE_PAIR) | A_BOLD);

    attron(COLOR_PAIR(COLOR_PANEL_PAIR) | A_BOLD);
    mvprintw(startY + 2, px, "├──────────────────────┤");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR));

    attron(COLOR_PAIR(COLOR_HEAD_PAIR));
    mvprintw(startY + 3, px, "│ @  Snake Head        │");
    attroff(COLOR_PAIR(COLOR_HEAD_PAIR));

    attron(COLOR_PAIR(COLOR_BODY_PAIR));
    mvprintw(startY + 4, px, "│ o  Snake Body        │");
    attroff(COLOR_PAIR(COLOR_BODY_PAIR));

    attron(COLOR_PAIR(COLOR_GROWTH_PAIR));
    mvprintw(startY + 5, px, "│ G  Growth Item       │");
    attroff(COLOR_PAIR(COLOR_GROWTH_PAIR));

    attron(COLOR_PAIR(COLOR_POISON_PAIR));
    mvprintw(startY + 6, px, "│ P  Poison Item       │");
    attroff(COLOR_PAIR(COLOR_POISON_PAIR));

    attron(COLOR_PAIR(COLOR_GATE_PAIR));
    mvprintw(startY + 7, px, "│ D  Gate              │");
    attroff(COLOR_PAIR(COLOR_GATE_PAIR));

    attron(COLOR_PAIR(COLOR_PANEL_PAIR));
    mvprintw(startY + 8, px, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR));
}

static void drawCenterBox(const char* title, const char* line1, const char* line2,
                          int colorPair) {
    int centerY = std::max(0, MAP_SIZE / 2 - 3);
    int centerX = std::max(0, MAP_SIZE - 11);

    attron(COLOR_PAIR(colorPair) | A_BOLD);
    mvprintw(centerY,     centerX, "┌──────────────────────┐");
    mvprintw(centerY + 1, centerX, "│ %-20s │", title);
    mvprintw(centerY + 2, centerX, "├──────────────────────┤");
    attroff(A_BOLD);
    mvprintw(centerY + 3, centerX, "│ %-20s │", line1);
    mvprintw(centerY + 4, centerX, "│ %-20s │", line2);
    mvprintw(centerY + 5, centerX, "└──────────────────────┘");
    attroff(COLOR_PAIR(colorPair));

    refresh();
}


void initUI() {
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    // ncurses 색상 지원 여부 확인 후 색상 쌍 초기화
    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(COLOR_WALL_PAIR,    COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_IWALL_PAIR,   COLOR_CYAN,    COLOR_BLACK);
        init_pair(COLOR_HEAD_PAIR,    COLOR_YELLOW,  COLOR_BLACK);
        init_pair(COLOR_BODY_PAIR,    COLOR_GREEN,   COLOR_BLACK);
        init_pair(COLOR_GROWTH_PAIR,  COLOR_YELLOW,  COLOR_BLACK);
        init_pair(COLOR_POISON_PAIR,  COLOR_RED,     COLOR_BLACK);
        init_pair(COLOR_GATE_PAIR,    COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_UI_PAIR,      COLOR_CYAN,    COLOR_BLACK);
        init_pair(COLOR_TITLE_PAIR,   COLOR_YELLOW,  COLOR_BLACK);
        init_pair(COLOR_VALUE_PAIR,   COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_DONE_PAIR,    COLOR_GREEN,   COLOR_BLACK);
        init_pair(COLOR_PANEL_PAIR,   COLOR_BLUE,    COLOR_BLACK);
    }

    clear();
    refresh();
}

void drawScreen(const int map[MAP_SIZE][MAP_SIZE]) {
    drawMapFrame();

    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            int tile      = map[y][x];
            int colorPair = tileColor(tile);
            int screenY   = MAP_ORIGIN_Y + y;
            int screenX   = MAP_ORIGIN_X + x * 2;   // 타일 하나가 2칸 너비

            if (colorPair != 0)
                attron(COLOR_PAIR(colorPair) |
                       (tile == SNAKE_HEAD || tile == GATE ? A_BOLD : 0));

            mvprintw(screenY, screenX, "%s", tileChar(tile));

            if (colorPair != 0)
                attroff(COLOR_PAIR(colorPair) |
                        (tile == SNAKE_HEAD || tile == GATE ? A_BOLD : 0));
        }
    }
    refresh();
}

void drawPanel(int score, int snakeLen, int elapsedSec, int level) {
    drawScoreBoard(score, snakeLen, 0, 0, 0, elapsedSec, level);
    drawLegend(PANEL_Y + 13);
    refresh();
}

void drawScoreBoard(int score, int snakeLen, int growthCount,
                    int poisonCount, int gateCount, int elapsedSec, int level) {
    drawScoreBoard(score, snakeLen, snakeLen, growthCount,
                   poisonCount, gateCount, elapsedSec, level);
}

void drawScoreBoard(int score, int snakeLen, int maxLen, int growthCount,
                    int poisonCount, int gateCount, int elapsedSec, int level) {
    int px = PANEL_X;
    int py = PANEL_Y;

    maxLen = std::max(maxLen, snakeLen);

    clearPanelArea();

    attron(COLOR_PAIR(COLOR_PANEL_PAIR) | A_BOLD);

    mvprintw(py,     px, "┌──────────────────────┐");
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR) | A_BOLD);

    attron(COLOR_PAIR(COLOR_TITLE_PAIR) | A_BOLD);
    mvprintw(py + 1, px, "│      SCORE BOARD     │");
    attroff(COLOR_PAIR(COLOR_TITLE_PAIR) | A_BOLD);

    attron(COLOR_PAIR(COLOR_PANEL_PAIR) | A_BOLD);
    mvprintw(py + 2, px, "├──────────────────────┤");

    attroff(A_BOLD);
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR));

    attron(COLOR_PAIR(COLOR_UI_PAIR));
    mvprintw(py + 3,  px, "│ Stage               │");
    mvprintw(py + 4,  px, "│ Time                │");
    mvprintw(py + 5,  px, "│ Score               │");
    mvprintw(py + 6,  px, "│ B Length            │");
    attroff(COLOR_PAIR(COLOR_UI_PAIR));

    printValue(py + 3, px + 15, "%5d", level);
    printValueText(py + 4, px + 13, formatTime(elapsedSec).c_str());
    printValue(py + 5, px + 15, "%5d", score);

    attron(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);
    mvprintw(py + 6, px + 14, "%3d/%-3d", snakeLen, maxLen);
    attroff(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);

    attron(COLOR_PAIR(COLOR_PANEL_PAIR));
    mvprintw(py + 7,  px, "├──────────────────────┤");
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR));

    attron(COLOR_PAIR(COLOR_UI_PAIR));
    mvprintw(py + 8,  px, "│ + Growth            │");
    mvprintw(py + 9,  px, "│ - Poison            │");
    mvprintw(py + 10, px, "│ G Gate              │");
    attroff(COLOR_PAIR(COLOR_UI_PAIR));

    printValue(py + 8,  px + 15, "%5d", growthCount);
    printValue(py + 9,  px + 15, "%5d", poisonCount);
    printValue(py + 10, px + 15, "%5d", gateCount);

    attron(COLOR_PAIR(COLOR_PANEL_PAIR));
    mvprintw(py + 11, px, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR));

    refresh();
}

void drawMissionBoard(int currentLen, int targetLen,
                      int growthCount, int targetGrowth,
                      int poisonCount, int targetPoison,
                      int gateCount, int targetGate) {
    int px = PANEL_X;
    int py = PANEL_Y + 13;

    attron(COLOR_PAIR(COLOR_PANEL_PAIR) | A_BOLD);
    mvprintw(py,     px, "┌──────────────────────┐");
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR) | A_BOLD);

    attron(COLOR_PAIR(COLOR_TITLE_PAIR) | A_BOLD);
    mvprintw(py + 1, px, "│     MISSION BOARD    │");
    attroff(COLOR_PAIR(COLOR_TITLE_PAIR) | A_BOLD);

    attron(COLOR_PAIR(COLOR_PANEL_PAIR) | A_BOLD);
    mvprintw(py + 2, px, "├──────────────────────┤");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR));

    printMissionLine(py + 3, "B Length", currentLen, targetLen);
    printMissionLine(py + 4, "+ Growth", growthCount, targetGrowth);
    printMissionLine(py + 5, "- Poison", poisonCount, targetPoison);
    printMissionLine(py + 6, "G Gate", gateCount, targetGate);

    attron(COLOR_PAIR(COLOR_PANEL_PAIR));
    mvprintw(py + 7, px, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR));

    refresh();
}

void showStartScreen() {
    clear();

    int y = 3;
    int x = 10;

    attron(COLOR_PAIR(COLOR_UI_PAIR) | A_BOLD);
    mvprintw(y,     x, "┌────────────────────────────┐");
    mvprintw(y + 1, x, "│        SNAKE GAME          │");
    mvprintw(y + 2, x, "├────────────────────────────┤");
    attroff(A_BOLD);
    mvprintw(y + 3, x, "│ Arrow Keys : Move          │");
    mvprintw(y + 4, x, "│ P          : Pause         │");
    mvprintw(y + 5, x, "│ Q          : Quit          │");
    mvprintw(y + 6, x, "├────────────────────────────┤");
    mvprintw(y + 7, x, "│ Press any key to start     │");
    mvprintw(y + 8, x, "└────────────────────────────┘");
    attroff(COLOR_PAIR(COLOR_UI_PAIR));

    refresh();
    getch();
    clear();
}

void showPauseMessage() {
    drawCenterBox("PAUSED", "Press P to resume", "Press Q to quit", COLOR_UI_PAIR);
}

void showGameOver(int finalScore, int snakeLen, int elapsedSec) {
    int centerY = std::max(0, MAP_SIZE / 2 - 4);
    int centerX = std::max(0, MAP_SIZE - 11);

    attron(COLOR_PAIR(COLOR_POISON_PAIR) | A_BOLD);
    mvprintw(centerY,     centerX, "┌──────────────────────┐");
    mvprintw(centerY + 1, centerX, "│      GAME OVER       │");
    mvprintw(centerY + 2, centerX, "├──────────────────────┤");
    attroff(A_BOLD);
    mvprintw(centerY + 3, centerX, "│ Score : %-11d │", finalScore);
    mvprintw(centerY + 4, centerX, "│ Length: %-11d │", snakeLen);
    mvprintw(centerY + 5, centerX, "│ Time  : %-11s │", formatTime(elapsedSec).c_str());
    mvprintw(centerY + 6, centerX, "├──────────────────────┤");
    mvprintw(centerY + 7, centerX, "│ Press any key...     │");
    mvprintw(centerY + 8, centerX, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_POISON_PAIR));

    refresh();
    getch();   // 아무 키나 누를 때까지 대기
}

void showGameClear(int finalScore, int elapsedSec) {
    int centerY = std::max(0, MAP_SIZE / 2 - 4);
    int centerX = std::max(0, MAP_SIZE - 11);

    attron(COLOR_PAIR(COLOR_GROWTH_PAIR) | A_BOLD);
    mvprintw(centerY,     centerX, "┌──────────────────────┐");
    mvprintw(centerY + 1, centerX, "│      GAME CLEAR      │");
    mvprintw(centerY + 2, centerX, "├──────────────────────┤");
    attroff(A_BOLD);
    mvprintw(centerY + 3, centerX, "│ Score : %-11d │", finalScore);
    mvprintw(centerY + 4, centerX, "│ Time  : %-11s │", formatTime(elapsedSec).c_str());
    mvprintw(centerY + 5, centerX, "├──────────────────────┤");
    mvprintw(centerY + 6, centerX, "│ Press any key...     │");
    mvprintw(centerY + 7, centerX, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_GROWTH_PAIR));

    refresh();
    getch();
}

void cleanupUI() {
    // 현재는 별도 동적 자원 없음
    // 추후 WINDOW* 등을 사용할 경우 delwin() 호출 위치
}
