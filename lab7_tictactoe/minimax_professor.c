#include "minimax.h"
#include <stdio.h>
#define DA (TICTACTOE_BOARD_ROWS * TICTACTOE_BOARD_COLUMNS)
static tictactoe_location_t ga;
static uint32_t gb;
void printBoard(tictactoe_board_t *aa) {
  for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    for (int j = 0; j < TICTACTOE_BOARD_COLUMNS; j++) {
      if (aa->squares[i][j] == MINIMAX_X_SQUARE)
        printf("X");
      else if (aa->squares[i][j] == MINIMAX_O_SQUARE)
        printf("O");
      else
        printf(" ");
    }
    printf("\n");
  }
}
static minimax_score_t fa(tictactoe_board_t *aa, bool ab) {
  uint8_t la = 0;
  minimax_score_t lb[DA];
  tictactoe_location_t lc[DA];
  minimax_score_t ld = minimax_computeBoardScore(aa, !ab);
  if (minimax_isGameOver(ld)) {
    if (ld < -1)
      ld -= DA - gb;
    else if (ld > 1)
      ld += DA - gb;
    return ld;
  }
  for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    for (int j = 0; j < TICTACTOE_BOARD_COLUMNS; j++) {
      if (aa->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
        if (ab)
          aa->squares[i][j] = MINIMAX_X_SQUARE;
        else
          aa->squares[i][j] = MINIMAX_O_SQUARE;
        gb++;
        ld = fa(aa, !ab);
        gb--;
        lb[la] = ld;
        lc[la].row = i;
        lc[la].column = j;
        la++;
        aa->squares[i][j] = MINIMAX_EMPTY_SQUARE;
      }
    }
  }
  if (ab) {
    ld = lb[0];
    ga = lc[0];
    for (int i = 1; i < la; i++) {
      if (lb[i] > ld) {
        ld = lb[i];
        ga = lc[i];
      }
    }
  } else {
    ld = lb[0];
    ga = lc[0];
    for (int i = 1; i < la; i++) {
      if (lb[i] < ld) {
        ld = lb[i];
        ga = lc[i];
      }
    }
  }
  return ld;
}
tictactoe_location_t minimax_computeNextMove(tictactoe_board_t *aa, bool ab) {
  fa(aa, ab);
  return ga;
}
bool minimax_isGameOver(minimax_score_t aa) {
  return aa != MINIMAX_NOT_ENDGAME;
}
minimax_score_t minimax_computeBoardScore(tictactoe_board_t *aa, bool ab) {
  uint8_t la = ab ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;
  minimax_score_t lb = ab ? MINIMAX_X_WINNING_SCORE : MINIMAX_O_WINNING_SCORE;
  bool lc = true;
  for (int j = 0; j < TICTACTOE_BOARD_COLUMNS; j++) {
    bool ld = true;
    for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
      if (aa->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
        lc = false;
        ld = false;
      } else if (aa->squares[i][j] != la) {
        ld = false;
      }
    }
    if (ld)
      return lb;
  }
  for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    bool ld = true;
    for (int j = 0; j < TICTACTOE_BOARD_COLUMNS; j++) {
      if (aa->squares[i][j] != la) {
        ld = false;
      }
    }
    if (ld)
      return lb;
  }
  bool ld = true;
  for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    if (aa->squares[i][i] != la) {
      ld = false;
    }
  }
  if (ld)
    return lb;
  ld = true;
  for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    if (aa->squares[i][TICTACTOE_BOARD_ROWS - 1 - i] != la) {
      ld = false;
    }
  }
  if (ld)
    return lb;
  if (lc)
    return MINIMAX_DRAW_SCORE;
  else
    return MINIMAX_NOT_ENDGAME;
}
void minimax_initBoard(tictactoe_board_t *aa) {
  for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    for (int j = 0; j < TICTACTOE_BOARD_COLUMNS; j++) {
      aa->squares[i][j] = MINIMAX_EMPTY_SQUARE;
    }
  }
}