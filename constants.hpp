#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

// 맵 크기
const int MAP_SIZE = 30;


// 맵 요소
const int EMPTY = 0;

const int WALL = 1;
const int IMMUNE_WALL = 2;

const int SNAKE_HEAD = 3;
const int SNAKE_BODY = 4;

const int GROWTH_ITEM = 5;
const int POISON_ITEM = 6;

const int GATE = 7;
const int FEVER_ITEM = 8;
const int CONDITIONAL_GATE = 9;

#endif

/*
병합 시작 버전과 비교한 변경점:
- 팀원이 추가한 피버 아이템을 맵과 UI에서 구분하기 위해 FEVER_ITEM 상수 추가
- 길이 5 이상 조건부 게이트를 일반 게이트와 다르게 표시하기 위해 CONDITIONAL_GATE 상수 추가
- 맵 크기를 21x21에서 30x30으로 확장
*/
