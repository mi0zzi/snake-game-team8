#ifndef MAP_HPP
#define MAP_HPP

#include "constants.hpp"

// 모든 모듈이 같은 30x30 게임 맵을 공유하기 위한 전역 배열 선언
extern int gameMap[MAP_SIZE][MAP_SIZE];

// 기본 맵 초기화
void initMap();

// Stage별 맵 생성
void initStage(int stage);

// 맵 출력
void drawMap(int stage);

#endif

/*
병합 시작 버전과 비교한 변경점:
- map.cpp에 정의된 gameMap을 main, snake, item, gate, ui에서 함께 사용하도록 extern 선언 추가
- 여러 모듈이 같은 30x30 맵 상태를 기준으로 동작할 수 있게 병합 연결 지점 추가
*/
