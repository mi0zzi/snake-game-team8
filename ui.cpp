#include "ui.hpp"
#include <ncurses.h>
#include <algorithm>
#include <cstdio>
#include <string>

// 타일 값에 대응하는 화면 출력 기호 반환
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
        case FEVER_ITEM:  return CHAR_FEVER;
        case CONDITIONAL_GATE: return CHAR_DANGER_GATE;
        default:          return CHAR_EMPTY;
    }
}

// 타일 값에 대응하는 ncurses 색상 쌍 번호 반환
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
        case FEVER_ITEM:  return COLOR_FEVER_PAIR;
        case CONDITIONAL_GATE: return COLOR_DANGER_PAIR;
        default:          return 0;   // 기본 색상
    }
}

// 초 단위 시간을 점수판에서 쓰는 MM:SS 문자열로 변환
// 초 → "MM:SS" 형식 문자열
static std::string formatTime(int seconds) {
    if (seconds < 0) {
        seconds = 0;
    }

    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d", seconds / 60, seconds % 60);
    return std::string(buf);
}

// 미션 달성 여부에 따라 OK 표시 또는 빈 표시 반환
static const char* missionMark(bool achieved) {
    return achieved ? "OK" : "  ";
}

// 주어진 박스 폭을 스네이크 맵 영역의 가로 중앙에 놓기 위한 x 좌표 계산
static int mapCenteredX(int width) {
    const int mapWidth = MAP_SIZE * 2;
    return std::max(0, MAP_ORIGIN_X + (mapWidth - width) / 2);
}

// 주어진 박스 높이를 스네이크 맵 영역의 세로 중앙에 놓기 위한 y 좌표 계산
static int mapCenteredY(int height) {
    return std::max(0, MAP_ORIGIN_Y + (MAP_SIZE - height) / 2);
}

// 팝업 본문을 항상 20칸 폭으로 출력하여 오른쪽 테두리 위치 고정
static void printPopupLine(int y, int x, const char* text) {
    char buf[21];
    snprintf(buf, sizeof(buf), "%-20.20s", text);
    mvprintw(y, x, "│ %s │", buf);
}

// 팝업 제목을 20칸 본문 영역 안에서 중앙 정렬하여 출력
static void printPopupTitle(int y, int x, const char* text) {
    char buf[21];
    const int textLen = std::min(20, (int)std::string(text).length());
    const int leftPad = (20 - textLen) / 2;
    snprintf(buf, sizeof(buf), "%*s%-*.*s", leftPad, "", 20 - leftPad, textLen, text);
    mvprintw(y, x, "│ %s │", buf);
}

// 정수 값을 색상 강조하여 점수판의 고정 위치에 출력
static void printValue(int y, int x, const char* fmt, int value) {
    attron(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);
    mvprintw(y, x, fmt, value);
    attroff(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);
}

// 문자열 값을 색상 강조하여 점수판의 고정 위치에 출력
static void printValueText(int y, int x, const char* value) {
    attron(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);
    mvprintw(y, x, "%7s", value);
    attroff(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);
}

// 미션보드 한 줄을 기호, 라벨, 현재값/목표값, 달성 여부로 나누어 출력
static void printMissionLine(int y, const char* symbol, const char* label,
                             int current, int target, int symbolColor) {
    int px = PANEL_X;
    bool achieved = current >= target;
    int statusColor = achieved ? COLOR_DONE_PAIR : COLOR_UI_PAIR;

    attron(COLOR_PAIR(COLOR_UI_PAIR));
    mvprintw(y, px, "│                      │");
    attroff(COLOR_PAIR(COLOR_UI_PAIR));

    attron(COLOR_PAIR(symbolColor) | A_BOLD);
    mvprintw(y, px + 2, "%s", symbol);
    mvprintw(y, px + 5, "%-7s", label);
    attroff(COLOR_PAIR(symbolColor) | A_BOLD);

    attron(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);
    mvprintw(y, px + 13, "%3d/%-3d", current, target);
    attroff(COLOR_PAIR(COLOR_VALUE_PAIR) | A_BOLD);

    attron(COLOR_PAIR(statusColor) | A_BOLD);
    mvprintw(y, px + 20, "%s", missionMark(achieved));
    attroff(COLOR_PAIR(statusColor) | A_BOLD);
}

// 오른쪽 사이드 패널 영역을 새로 그리기 전에 삭제
static void clearPanelArea() {
    for (int y = PANEL_Y; y < PANEL_Y + 28; ++y) {
        move(y, PANEL_X);
        clrtoeol();
    }
}

// 맵 외곽 프레임과 제목 출력
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

// 오른쪽 패널에 게임 기호 범례 출력
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
    mvprintw(startY + 5, px, "│ +  Growth Item       │");
    attroff(COLOR_PAIR(COLOR_GROWTH_PAIR));

    attron(COLOR_PAIR(COLOR_POISON_PAIR));
    mvprintw(startY + 6, px, "│ -  Poison Item       │");
    attroff(COLOR_PAIR(COLOR_POISON_PAIR));

    attron(COLOR_PAIR(COLOR_GATE_PAIR));
    mvprintw(startY + 7, px, "│ G  Gate              │");
    attroff(COLOR_PAIR(COLOR_GATE_PAIR));

    attron(COLOR_PAIR(COLOR_FEVER_PAIR) | A_BOLD);
    mvprintw(startY + 8, px, "│ F  Fever Item        │");
    attroff(COLOR_PAIR(COLOR_FEVER_PAIR) | A_BOLD);

    attron(COLOR_PAIR(COLOR_DANGER_PAIR) | A_BOLD);
    mvprintw(startY + 9, px, "│ !  Len 5 Gate        │");
    attroff(COLOR_PAIR(COLOR_DANGER_PAIR) | A_BOLD);

    attron(COLOR_PAIR(COLOR_PANEL_PAIR));
    mvprintw(startY + 10, px, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR));
}

// 일시정지처럼 짧은 중앙 팝업 박스 출력
static void drawCenterBox(const char* title, const char* line1, const char* line2,
                          int colorPair) {
    const int boxWidth = 24;
    const int boxHeight = 6;
    int centerY = mapCenteredY(boxHeight);
    int centerX = mapCenteredX(boxWidth);

    attron(COLOR_PAIR(colorPair) | A_BOLD);
    mvprintw(centerY,     centerX, "┌──────────────────────┐");
    printPopupTitle(centerY + 1, centerX, title);
    mvprintw(centerY + 2, centerX, "├──────────────────────┤");
    attroff(A_BOLD);
    printPopupLine(centerY + 3, centerX, line1);
    printPopupLine(centerY + 4, centerX, line2);
    mvprintw(centerY + 5, centerX, "└──────────────────────┘");
    attroff(COLOR_PAIR(colorPair));

    refresh();
}


// ncurses 입력, 커서, 색상 쌍 초기화
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
        init_pair(COLOR_FEVER_PAIR,   COLOR_CYAN,    COLOR_BLACK);
        init_pair(COLOR_DANGER_PAIR,  COLOR_RED,     COLOR_BLACK);
    }

    clear();
    refresh();
}

// 공유 맵 배열 전체를 현재 타일 기호와 색상으로 다시 출력
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
                       (tile == SNAKE_HEAD || tile == GATE ||
                        tile == FEVER_ITEM || tile == CONDITIONAL_GATE ? A_BOLD : 0));

            mvprintw(screenY, screenX, "%s", tileChar(tile));

            if (colorPair != 0)
                attroff(COLOR_PAIR(colorPair) |
                        (tile == SNAKE_HEAD || tile == GATE ||
                         tile == FEVER_ITEM || tile == CONDITIONAL_GATE ? A_BOLD : 0));
        }
    }
    refresh();
}

// 기존 호출 호환용 패널 출력 함수로 점수판과 범례 함께 출력
void drawPanel(int score, int snakeLen, int elapsedSec, int level) {
    drawScoreBoard(score, snakeLen, 0, 0, 0, elapsedSec, level);
    drawLegend(PANEL_Y + 13);
    refresh();
}

// 최대 길이 값이 없을 때 현재 길이를 최대 길이로 간주해 점수판 출력
void drawScoreBoard(int score, int snakeLen, int growthCount,
                    int poisonCount, int gateCount, int elapsedSec, int level) {
    drawScoreBoard(score, snakeLen, snakeLen, growthCount,
                   poisonCount, gateCount, elapsedSec, level);
}

// PDF 명세 형식에 맞춰 현재 점수, 길이, 아이템/게이트 카운트 출력
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
    mvprintw(py + 3,  px, "│ Stage                │");
    mvprintw(py + 4,  px, "│ Time                 │");
    mvprintw(py + 5,  px, "│ Score                │");
    mvprintw(py + 6,  px, "│ B Length             │");
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
    mvprintw(py + 8,  px, "│ + Growth             │");
    mvprintw(py + 9,  px, "│ - Poison             │");
    mvprintw(py + 10, px, "│ G Gate               │");
    attroff(COLOR_PAIR(COLOR_UI_PAIR));

    printValue(py + 8,  px + 15, "%5d", growthCount);
    printValue(py + 9,  px + 15, "%5d", poisonCount);
    printValue(py + 10, px + 15, "%5d", gateCount);

    attron(COLOR_PAIR(COLOR_PANEL_PAIR));
    mvprintw(py + 11, px, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR));

    refresh();
}

// 현재 미션 진행도와 목표값, 달성 여부를 오른쪽 미션보드에 출력
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

    printMissionLine(py + 3, "B", "Length", currentLen, targetLen,
                     COLOR_BODY_PAIR);
    printMissionLine(py + 4, CHAR_GROWTH, "Growth", growthCount, targetGrowth,
                     COLOR_GROWTH_PAIR);
    printMissionLine(py + 5, CHAR_POISON, "Poison", poisonCount, targetPoison,
                     COLOR_POISON_PAIR);
    printMissionLine(py + 6, CHAR_GATE, "Gate", gateCount, targetGate,
                     COLOR_GATE_PAIR);

    attron(COLOR_PAIR(COLOR_PANEL_PAIR));
    mvprintw(py + 7, px, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_PANEL_PAIR));

    refresh();
}

// 게임 시작 전 조작법 안내 화면을 맵 중앙에 출력하고 입력 대기
void showStartScreen() {
    clear();

    const int boxWidth = 30;
    const int boxHeight = 9;
    int y = mapCenteredY(boxHeight);
    int x = mapCenteredX(boxWidth);

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

// 일시정지 상태 안내 팝업을 맵 중앙에 표시
void showPauseMessage() {
    drawCenterBox("PAUSED", "Press P to resume", "Press Q to quit", COLOR_UI_PAIR);
}

// 게임오버 팝업을 출력하고 R 또는 Q 입력을 받을 때까지 대기
int showGameOver(int finalScore, int snakeLen, int elapsedSec) {
    const int boxWidth = 24;
    const int boxHeight = 9;
    int centerY = mapCenteredY(boxHeight);
    int centerX = mapCenteredX(boxWidth);

    char scoreText[32];
    char lengthText[32];
    char timeText[32];
    snprintf(scoreText, sizeof(scoreText), "Score : %d", finalScore);
    snprintf(lengthText, sizeof(lengthText), "Length: %d", snakeLen);
    snprintf(timeText, sizeof(timeText), "Time  : %s", formatTime(elapsedSec).c_str());

    attron(COLOR_PAIR(COLOR_POISON_PAIR) | A_BOLD);
    mvprintw(centerY,     centerX, "┌──────────────────────┐");
    printPopupTitle(centerY + 1, centerX, "GAME OVER");
    mvprintw(centerY + 2, centerX, "├──────────────────────┤");
    attroff(A_BOLD);
    printPopupLine(centerY + 3, centerX, scoreText);
    printPopupLine(centerY + 4, centerX, lengthText);
    printPopupLine(centerY + 5, centerX, timeText);
    mvprintw(centerY + 6, centerX, "├──────────────────────┤");
    printPopupLine(centerY + 7, centerX, "R Restart / Q Quit");
    mvprintw(centerY + 8, centerX, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_POISON_PAIR));

    refresh();
    while (true) {
        int key = getch();
        if (key == 'r' || key == 'R' || key == 'q' || key == 'Q') {
            return key;
        }
        napms(50);
    }
}

// 스테이지 미션 달성 시 클리어 팝업을 출력하고 입력 대기
void showGameClear(int finalScore, int elapsedSec) {
    const int boxWidth = 24;
    const int boxHeight = 8;
    int centerY = mapCenteredY(boxHeight);
    int centerX = mapCenteredX(boxWidth);

    char scoreText[32];
    char timeText[32];
    snprintf(scoreText, sizeof(scoreText), "Score : %d", finalScore);
    snprintf(timeText, sizeof(timeText), "Time  : %s", formatTime(elapsedSec).c_str());

    attron(COLOR_PAIR(COLOR_GROWTH_PAIR) | A_BOLD);
    mvprintw(centerY,     centerX, "┌──────────────────────┐");
    printPopupTitle(centerY + 1, centerX, "GAME CLEAR");
    mvprintw(centerY + 2, centerX, "├──────────────────────┤");
    attroff(A_BOLD);
    printPopupLine(centerY + 3, centerX, scoreText);
    printPopupLine(centerY + 4, centerX, timeText);
    mvprintw(centerY + 5, centerX, "├──────────────────────┤");
    printPopupLine(centerY + 6, centerX, "Press any key...");
    mvprintw(centerY + 7, centerX, "└──────────────────────┘");
    attroff(COLOR_PAIR(COLOR_GROWTH_PAIR));

    refresh();
    getch();
}

// 현재 UI는 동적 ncurses 창을 만들지 않으므로 별도 해제 없이 확장 지점만 유지
void cleanupUI() {
    // 현재는 별도 동적 자원 없음
    // 추후 WINDOW* 등을 사용할 경우 delwin() 호출 위치
}

/*
병합 시작 버전과 비교한 변경점:
- drawScreen()에서 FEVER_ITEM과 CONDITIONAL_GATE를 인식해 각각 다른 기호와 색으로 출력하도록 수정
- 범례에 피버 아이템과 길이 5 조건부 게이트 설명 추가
- 미션보드의 B, +, -, G 기호 색을 실제 게임 화면의 뱀, 성장 아이템, 독 아이템, 게이트 색과 일치화
- 게임오버 화면 문구를 R Restart / Q Quit 형태로 바꾸고 입력된 키를 반환하도록 변경
- 게임오버 화면에서 R 또는 Q가 들어올 때까지 기다리게 하여 재시작 입력이 다른 키 입력에 묻히지 않도록 변경
- 성장 아이템, 독 아이템, 일반 게이트, 조건부 게이트 기호를 +, -, G, !로 통일
- 스코어보드 본문 줄의 폭을 테두리 줄과 맞춰 오른쪽 선이 울퉁불퉁하게 보이는 문제를 수정
- 시작 안내 화면과 게임오버/클리어 팝업을 스네이크 맵 영역 중앙 기준으로 정렬하고, 팝업 본문을 고정 폭으로 출력하도록 수정
- 각 함수 위에 역할 설명 주석을 추가하여 UI 출력 흐름을 더 쉽게 파악하도록 정리
*/
