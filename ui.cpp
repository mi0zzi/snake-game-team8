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
    return achieved ? "v" : " ";
}

static void clearPanelArea() {
    for (int y = PANEL_Y; y < PANEL_Y + 24; ++y) {
        move(y, PANEL_X);
        clrtoeol();
    }
}

static void drawLegend(int startY) {
    int px = PANEL_X;

    attron(COLOR_PAIR(COLOR_UI_PAIR) | A_BOLD);
    mvprintw(startY,     px, "┌──────────────────────┐");
    mvprintw(startY + 1, px, "│        LEGEND        │");
    mvprintw(startY + 2, px, "├──────────────────────┤");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(COLOR_UI_PAIR));

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

    attron(COLOR_PAIR(COLOR_UI_PAIR));
    mvprintw(startY + 8, px, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_UI_PAIR));
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

    clear();
    refresh();
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
    drawScoreBoard(score, snakeLen, 0, 0, 0, elapsedSec, level);
    drawLegend(PANEL_Y + 13);
    refresh();
}

void drawScoreBoard(int score, int snakeLen, int growthCount,
                    int poisonCount, int gateCount, int elapsedSec, int level) {
    int px = PANEL_X;
    int py = PANEL_Y;

    clearPanelArea();

    attron(COLOR_PAIR(COLOR_UI_PAIR) | A_BOLD);

    mvprintw(py,     px, "┌──────────────────────┐");
    mvprintw(py + 1, px, "│      SCORE BOARD     │");
    mvprintw(py + 2, px, "├──────────────────────┤");

    attroff(A_BOLD);

    mvprintw(py + 3,  px, "│ Stage        : %-5d │", level);
    mvprintw(py + 4,  px, "│ Score        : %-5d │", score);
    mvprintw(py + 5,  px, "│ Length       : %-5d │", snakeLen);
    mvprintw(py + 6,  px, "│ Time         : %-5s │", formatTime(elapsedSec).c_str());
    mvprintw(py + 7,  px, "├──────────────────────┤");
    mvprintw(py + 8,  px, "│ Growth Item  : %-5d │", growthCount);
    mvprintw(py + 9,  px, "│ Poison Item  : %-5d │", poisonCount);
    mvprintw(py + 10, px, "│ Gate Used    : %-5d │", gateCount);
    mvprintw(py + 11, px, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_UI_PAIR));

    refresh();
}

void drawMissionBoard(int currentLen, int targetLen,
                      int growthCount, int targetGrowth,
                      int poisonCount, int targetPoison,
                      int gateCount, int targetGate) {
    int px = PANEL_X;
    int py = PANEL_Y + 13;

    attron(COLOR_PAIR(COLOR_UI_PAIR) | A_BOLD);
    mvprintw(py,     px, "┌──────────────────────┐");
    mvprintw(py + 1, px, "│     MISSION BOARD    │");
    mvprintw(py + 2, px, "├──────────────────────┤");
    attroff(A_BOLD);

    mvprintw(py + 3, px, "│ B  %2d / %-2d       [%s] │",
             currentLen, targetLen, missionMark(currentLen >= targetLen));
    mvprintw(py + 4, px, "│ +  %2d / %-2d       [%s] │",
             growthCount, targetGrowth, missionMark(growthCount >= targetGrowth));
    mvprintw(py + 5, px, "│ -  %2d / %-2d       [%s] │",
             poisonCount, targetPoison, missionMark(poisonCount >= targetPoison));
    mvprintw(py + 6, px, "│ G  %2d / %-2d       [%s] │",
             gateCount, targetGate, missionMark(gateCount >= targetGate));
    mvprintw(py + 7, px, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_UI_PAIR));

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
