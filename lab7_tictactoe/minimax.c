#include "minimax.h"
#include "ticTacToe.h"
#include <stdio.h>

#define NUM_POSSIBLE_LOCATIONS 9
#define MAX 1
#define MIN 0

#define X_SQUARE_VALUE 1
#define EMPTY_SQUARE_VALUE 0
#define O_SQUARE_VALUE -1

#define MAX_WIN_SUM 3
#define MIN_WIN_SUM -3
#define NUM_ROWS_COLUMNS_DIAGS 8 // Three rows, Three columns, Two diagonals

#define COLUMN_OFFSET 3

#define TL_BR_DIAG 6 // top left to bottom right diagonal
#define TR_BL_DIAG 7

#define DIAG_BOUND 3

static tictactoe_location_t choice;

typedef struct {
  tictactoe_location_t move_table[NUM_POSSIBLE_LOCATIONS];

  minimax_score_t score_table[NUM_POSSIBLE_LOCATIONS];
} board_table_t;

// Returns the score of the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the is_Xs_turn argument indicates which player just took their
// turn and makes it possible to speed up this function.
// Assumptions:
// (1) if is_Xs_turn == true, the last thing played was an 'X'.
// (2) if is_Xs_turn == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X' was played,
// you don't need to look for 'O's, and vice-versa.
minimax_score_t minimax_computeBoardScore(tictactoe_board_t *board,
                                          bool is_Xs_turn) {

  // A draw is only possible if there are not empty squares
  bool empty_square_found = false;
  bool x_win = false;
  bool o_win = false;
  bool draw = false;

  // make a copy of the board
  int8_t scoring_board[TICTACTOE_BOARD_ROWS][TICTACTOE_BOARD_COLUMNS];

  // replace all values in board with score calculation values
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; ++row) {

    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; ++column) {

      if (board->squares[row][column] == MINIMAX_EMPTY_SQUARE) {
        scoring_board[row][column] = EMPTY_SQUARE_VALUE;
        empty_square_found = true; // An empty square was found. This means the
                                   // game cannot be a draw

      } else if (board->squares[row][column] == MINIMAX_X_SQUARE) {
        scoring_board[row][column] = X_SQUARE_VALUE;

      } else {
        scoring_board[row][column] = O_SQUARE_VALUE;
      }
    }
  }

  // DEBUG
  // for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; ++row) {
  //   for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; ++column) {
  //     printf("%3d", scoring_board[row][column]);
  //   }
  //   printf("\n");
  // }

  int8_t row_column_diag_sums[NUM_ROWS_COLUMNS_DIAGS] = {
      0, 0, 0, 0, 0, 0, 0, 0}; // [0-2] rows, [3-5]
                               // columns, [6] top left
                               // to bottom right diag,
                               // [7] top right to
                               // bottom left diag

  // Calulate the sums accross rows, columns, and diagonals
  // Calculate row sums

  // DEBUG
  // for (uint8_t i = 0; i < 8; ++i) {
  //   printf("rcds before[%d]: %d\n", i, row_column_diag_sums[i]);
  // }

  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; ++row) {
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; ++column) {
      row_column_diag_sums[row] += scoring_board[row][column];
      // printf("row_column_diag_sums[%d] + scoring_board[%d][%d]:%d = %d\n",
      // row,
      //        row, column, scoring_board[row][column],
      //        row_column_diag_sums[row]);
    }
    // printf("ROW [%d] SUM: %d\n", row, row_column_diag_sums[row]);
  }

  // Calculate column sums
  for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; ++column) {
    for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; ++row) {
      row_column_diag_sums[column + COLUMN_OFFSET] +=
          scoring_board[row][column];
    }
  }

  // Calculate top left to bottom right diagonal sum
  for (uint8_t index = 0; index < DIAG_BOUND; ++index) {
    row_column_diag_sums[TL_BR_DIAG] += scoring_board[index][index];
  }

  // Calculate top right to bottom left diagonal sum
  for (uint8_t index = 0; index < DIAG_BOUND; ++index) {
    row_column_diag_sums[TR_BL_DIAG] +=
        scoring_board[DIAG_BOUND - index - 1][index];
  }

  // DEBUG
  // for (uint8_t i = 0; i < 8; ++i) {
  //   printf("rcds after[%d]: %d\n", i, row_column_diag_sums[i]);
  // }

  // Determine if X won, O won, Draw, or not end game
  // Check to see if any of the rows, columns, or diagonals add up to 3. This
  // indicates an X win
  for (uint8_t i = 0; i < NUM_ROWS_COLUMNS_DIAGS; ++i) {
    if (row_column_diag_sums[i] == MAX_WIN_SUM) {
      x_win = true;
    } else if (row_column_diag_sums[i] == MIN_WIN_SUM) {
      o_win = true;
    } else {
      draw = true;
    }
  }

  if (x_win) {
    return MINIMAX_X_WINNING_SCORE;
  } else if (o_win) {
    return MINIMAX_O_WINNING_SCORE;
  } else if (draw && !empty_square_found) {
    return MINIMAX_DRAW_SCORE;
  }
  // If empty square found and all other conditions failed
  else {
    return MINIMAX_NOT_ENDGAME;
  }
}

// Init the board to all empty squares.
void minimax_initBoard(tictactoe_board_t *board) {
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; ++row) {
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; ++column) {
      board->squares[row][column] = MINIMAX_EMPTY_SQUARE;
    }
  }
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
  if (score != -1) {
    return true;
  } else {
    return false;
  }
}

// false for min. true for max
uint8_t minimax_getMinMaxIndex(minimax_score_t score_table[], bool min_or_max) {

  uint8_t min_max_index;
  tictactoe_location_t choice;
  minimax_score_t prev_score;
  minimax_score_t min_max_score;

  prev_score = score_table[0];
  min_max_score = score_table[0];

  // Compute max
  if (min_or_max) {
    for (uint8_t score_index = 1; score_index < NUM_POSSIBLE_LOCATIONS;
         ++score_index) {
      if (score_table[score_index] > prev_score) {
        min_max_score = score_table[score_index];
        min_max_index = score_index;
      }
      prev_score = score_table[score_index];
    }
  }
  // Compute min
  else {
    for (uint8_t score_index = 1; score_index < NUM_POSSIBLE_LOCATIONS;
         ++score_index) {
      if (score_table[score_index] < prev_score) {
        min_max_score = score_table[score_index];
        min_max_index = score_index;
      }
      prev_score = score_table[score_index];
    }
  }

  return min_max_index;
}

void minimax_initializeBoardTable(board_table_t *table) {
  for (uint8_t i = 0; i < NUM_POSSIBLE_LOCATIONS; ++i) {
    table->move_table[i].row = 0;
    table->move_table[i].column = 0;
    table->score_table[i] = 0;
  }
}

minimax_score_t minimax(tictactoe_board_t *board, bool is_Xs_turn) {

  int8_t score;
  board_table_t board_table;
  minimax_initializeBoardTable(&board_table);

  if (minimax_computeBoardScore(board, is_Xs_turn) != MINIMAX_NOT_ENDGAME) {
    // Recursion base case, there has been a win or a draw.
    // Evaluate board based upon prev player's turn.
    return minimax_computeBoardScore(board, !is_Xs_turn);
  }

  // Otherwise, you need to recurse.
  // This loop will generate all possible boards and call
  // minimax recursively for every empty square
  uint8_t current_square = 0;
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; ++row) {
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; ++column) {
      if (board->squares[row][column] == MINIMAX_EMPTY_SQUARE) {

        // Simulate playing at this location
        board->squares[row][column] =
            is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

        // Recursively call minimax to get the best score, assuming player
        // choses to play at this location.
        score = minimax(board, !is_Xs_turn);

        board_table.score_table[current_square] = score;
        board_table.move_table->row = row;
        board_table.move_table->column = column;
        ++current_square;

        // Undo the change to the board
        board->squares[row][column] = MINIMAX_EMPTY_SQUARE;
      }
    }
  }

  // Once you get here, you have iterated over empty squares at this level. All
  // of the scores have been computed in the move-score table for boards at this
  // level. Now you need to return the score depending upon whether you are
  // computing min or max.
  uint8_t index;
  if (is_Xs_turn) {
    index = minimax_getMinMaxIndex(board_table.score_table, MAX);
    choice = board_table.move_table[index]; // get the move with the highest
                                            // score in the move - score table.;
    score =
        board_table
            .score_table[index]; // highest score in the move - score table.;
  } else {
    index = minimax_getMinMaxIndex(board_table.score_table, MIN);
    choice = board_table.move_table[index]; // get the move with the lowest
                                            // score in the move - score table.;
    score = board_table
                .score_table[index]; // lowest score in the move - score table.;
  }
  return score;
}

// This routine is not recursive but will invoke the recursive minimax function.
// You will call this function from the controlling state machine that you will
// implement in a later milestone. It computes the row and column of the next
// move based upon: the current board and player.
//
// When called from the controlling state machine, you will call this function
// as follows:
// 1. If the computer is playing as X, you will call this function with
// is_Xs_turn = true.
// 2. If the computer is playing as O, you will call this function with
// is_Xs_turn = false.
// This function directly passes the  is_Xs_turn argument into the minimax()
// (helper) function.
tictactoe_location_t minimax_computeNextMove(tictactoe_board_t *board,
                                             bool is_Xs_turn) {
  minimax(board, is_Xs_turn);
  return choice;
}