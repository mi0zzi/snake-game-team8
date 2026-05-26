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

//  맵 타일 출력 문자
#define CHAR_WALL      "■"
#define CHAR_IWALL     "▩"
#define CHAR_HEAD      "@ "
#define CHAR_BODY      "o "
#define CHAR_GROWTH    "G "
#define CHAR_POISON    "P "
#define CHAR_GATE      "D "
#define CHAR_EMPTY     "  "

//  사이드 패널 위치 (맵 오른쪽)
#define PANEL_X  (MAP_SIZE * 2 + 2)  // 맵 폭(열 * 2) + 여백
#define PANEL_Y  0

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

// 게임 오버 메시지를 화면 중앙에 표시하고 키 입력 대기
void showGameOver(int finalScore, int snakeLen, int elapsedSec);

// 게임 클리어(목표 달성) 메시지 표시
void showGameClear(int finalScore, int elapsedSec);

// UI 자원 해제 (endwin 전에 호출)
void cleanupUI();

#endif // UI_HPP
