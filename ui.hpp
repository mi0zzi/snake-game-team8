#ifndef UI_HPP
#define UI_HPP

#include <ncurses.h>
#include "constants.hpp"

//  색상 쌍 ID (ncurses color pair 번호)
#define COLOR_WALL_PAIR      1   // 일반 벽
#define COLOR_IWALL_PAIR     2   // 면역 벽
#define COLOR_HEAD_PAIR      3   // 뱀 머리
#define COLOR_BODY_PAIR      4   // 뱀 몸통
#define COLOR_GROWTH_PAIR    5   // 성장 아이템
#define COLOR_POISON_PAIR    6   // 독 아이템
#define COLOR_GATE_PAIR      7   // 게이트
#define COLOR_UI_PAIR        8   // 사이드 UI 패널
#define COLOR_TITLE_PAIR     9   // 제목
#define COLOR_VALUE_PAIR     10  // 점수 값
#define COLOR_DONE_PAIR      11  // 미션 완료
#define COLOR_PANEL_PAIR     12  // 패널 테두리

//  맵 타일 출력 문자
#define CHAR_WALL      "■"
#define CHAR_IWALL     "▩"
#define CHAR_HEAD      "● "
#define CHAR_BODY      "○ "
#define CHAR_GROWTH    "G "
#define CHAR_POISON    "P "
#define CHAR_GATE      "D "
#define CHAR_EMPTY     "  "

//  맵과 사이드 패널 위치
#define MAP_ORIGIN_X  2
#define MAP_ORIGIN_Y  3
#define PANEL_X  (MAP_ORIGIN_X + MAP_SIZE * 2 + 5)
#define PANEL_Y  2
#define PANEL_WIDTH  24

//  UI 공개 함수

// 초기화: 색상·윈도우 설정 (게임 시작 시 한 번 호출)
void initUI();

// 맵 배열을 받아 게임 화면 전체를 다시 그림
void drawScreen(const int map[MAP_SIZE][MAP_SIZE]);

// 오른쪽 패널에 게임 정보를 표시
//   score      : 현재 점수
//   snakeLen   : 현재 뱀 길이
//   elapsedSec : 경과 시간(초)
//   level      : 현재 스테이지 레벨
void drawPanel(int score, int snakeLen, int elapsedSec, int level);

// 점수판을 표시
//   growthCount : 성장 아이템 획득 수
//   poisonCount : 독 아이템 획득 수
//   gateCount   : 게이트 통과 수
void drawScoreBoard(int score, int snakeLen, int growthCount,
                    int poisonCount, int gateCount, int elapsedSec, int level);

// PDF 명세 형식의 점수판을 표시
//   snakeLen : 현재 뱀 길이
//   maxLen   : 게임 중 달성한 최대 뱀 길이
void drawScoreBoard(int score, int snakeLen, int maxLen, int growthCount,
                    int poisonCount, int gateCount, int elapsedSec, int level);

// 미션 달성 현황을 표시
// 각 current 값이 target 이상이면 완료 표시
void drawMissionBoard(int currentLen, int targetLen,
                      int growthCount, int targetGrowth,
                      int poisonCount, int targetPoison,
                      int gateCount, int targetGate);

// 조작법과 시작 안내 화면을 표시
void showStartScreen();

// 일시정지 메시지를 화면 중앙에 표시
void showPauseMessage();

// 게임 오버 메시지를 화면 중앙에 표시하고 키 입력 대기
void showGameOver(int finalScore, int snakeLen, int elapsedSec);

// 게임 클리어(목표 달성) 메시지 표시
void showGameClear(int finalScore, int elapsedSec);

// UI 자원 해제 (endwin 전에 호출)
void cleanupUI();

#endif // UI_HPP
