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
#define COLOR_FEVER_PAIR     13  // 피버 아이템
#define COLOR_DANGER_PAIR    14  // 조건부 게이트
#define COLOR_GAMEOVER_PAIR  15  // 게임오버 팝업
#define COLOR_GROWTH_TEXT_PAIR 16 // 미션보드 성장 기호
#define COLOR_POISON_TEXT_PAIR 17 // 미션보드 독 기호

//  맵 타일 출력 문자
#define CHAR_WALL      "■ "
#define CHAR_IWALL     "▩ "
#define CHAR_HEAD      "● "
#define CHAR_BODY      "○ "
#define CHAR_GROWTH    "＋"
#define CHAR_POISON    "－"
#define CHAR_GATE      "G "
#define CHAR_FEVER     "F "
#define CHAR_DANGER_GATE "! "
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

// 게임 오버 메시지를 화면 중앙에 표시하고 입력 키를 반환
int showGameOver(int finalScore, int snakeLen, int elapsedSec);

// 게임 클리어(목표 달성) 메시지 표시
void showGameClear(int finalScore, int elapsedSec);

// UI 자원 해제 (endwin 전에 호출)
void cleanupUI();

#endif // UI_HPP

/*
병합 시작 버전과 비교한 변경점:
- 피버 아이템과 길이 5 조건부 게이트를 구분해서 표시하기 위한 색상 상수와 출력 문자 추가
- 게임오버 화면에서 입력된 키를 main.cpp가 받을 수 있도록 showGameOver() 반환형을 int로 변경
- 성장 아이템, 독 아이템, 일반 게이트, 조건부 게이트 기호를 +, -, G, !로 통일
- 성장 아이템은 초록 배경/진한 노란색 ＋, 독 아이템은 빨간 배경/흰색 －, 조건부 게이트는 검은 배경/분홍 !로 표시하도록 변경
- 미션보드 성장/독 기호는 배경 없이 초록 +, 빨간 -로 표시하도록 색상 상수 추가
- 벽과 면역벽 문자도 2칸을 차지하도록 바꿔 이전 프레임 문자가 남는 잔상 문제 해결
*/
