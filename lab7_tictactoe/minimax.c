#include "minimax.h"
#include "ticTacToe.h"
#include <stdio.h>

#define DEBUG 0

#define NUM_POSSIBLE_LOCATIONS 9 // The number of possible places on a board

#define MAX 1
#define MIN 0

// The following values are used to calculate the board score
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
// END

#define NULL_SPACE                                                             \
  20 // Used as a filler in the score table. Prevents unused spaces from being
     // considered valid

static tictactoe_location_t choice;

// Stores the score table and move table for an iteration
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

  // A draw is only possible if there are not empty squares. Thus is an empty
  // square is found we must indicate so
  bool empty_square_found = false;
  bool x_win = false;
  bool o_win = false;
  bool draw = false;

  // make a copy of the board
  int8_t scoring_board[TICTACTOE_BOARD_ROWS][TICTACTOE_BOARD_COLUMNS];

  // replace all values in board with score calculation values
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; ++row) {

    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; ++column) {

      // Check if square is empty. If so fill with the empty square valued
      // Do the same for X and O squares
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

  int8_t row_column_diag_sums[NUM_ROWS_COLUMNS_DIAGS] = {
      0, 0, 0, 0, 0, 0, 0, 0}; // [0-2] rows, [3-5]
                               // columns, [6] top left
                               // to bottom right diag,
                               // [7] top right to
                               // bottom left diag

  // Calulate the sums accross rows, columns, and diagonals
  // Calculate row sums
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; ++row) {
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; ++column) {
      row_column_diag_sums[row] += scoring_board[row][column];
    }
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

  // Determine if X won, O won, Draw, or not end game
  // Check to see if any of the rows, columns, or diagonals add up to 3. This
  // indicates an X win
  for (uint8_t i = 0; i < NUM_ROWS_COLUMNS_DIAGS; ++i) {
    // Check sums
    if (row_column_diag_sums[i] == MAX_WIN_SUM) {
      x_win = true;
    } else if (row_column_diag_sums[i] == MIN_WIN_SUM) {
      o_win = true;
    } else {
      draw = true;
    }
  }

  // Check all three end game states
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
  // Iterate over all squares and fill them with empty squares
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; ++row) {
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; ++column) {
      board->squares[row][column] = MINIMAX_EMPTY_SQUARE;
    }
  }
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
  // Check if game is over
  if (score != MINIMAX_NOT_ENDGAME) {
    return true;
  } else {
    return false;
  }
}

// false for min. true for max
// get the index in the score table for a min win or a max win
uint8_t minimax_getMinMaxIndex(minimax_score_t score_table[], bool min_or_max) {

  uint8_t min_max_index;

  tictactoe_location_t choice;
  minimax_score_t prev_score;
  minimax_score_t min_max_score;

  // Iterate over score table. find the first non null value and
  // record the corresponding index and value
  for (uint8_t index = 0; index < NUM_POSSIBLE_LOCATIONS; ++index) {
    if (score_table[index] != NULL_SPACE) {
      min_max_index = index;
      min_max_score = score_table[index];
      break;
    }
  }

  // Compute max
  if (min_or_max) {
    // Iterate over score table. Find the max value
    for (uint8_t score_index = 0; score_index < NUM_POSSIBLE_LOCATIONS;
         ++score_index) {
      if ((score_table[score_index] != NULL_SPACE) &&
          (score_table[score_index] > min_max_score)) {
        min_max_score = score_table[score_index];
        min_max_index = score_index;
      }
    }
  }
  // Compute min
  else {
    // Iterate over score table. Find the min value
    for (uint8_t score_index = 0; score_index < NUM_POSSIBLE_LOCATIONS;
         ++score_index) {
      if ((score_table[score_index] != NULL_SPACE) &&
          (score_table[score_index] < min_max_score)) {
        min_max_score = score_table[score_index];
        min_max_index = score_index;
      }
    }
  }

  return min_max_index;
}

// Initializes board table
void minimax_initializeBoardTable(board_table_t *table) {
  // sets all values to NULL
  for (uint8_t i = 0; i < NUM_POSSIBLE_LOCATIONS; ++i) {
    table->move_table[i].row = NULL_SPACE;
    table->move_table[i].column = NULL_SPACE;
    table->score_table[i] = NULL_SPACE;
  }
}

// Minimax algorithm for Tic Tac Toe
minimax_score_t minimax(tictactoe_board_t *board, bool is_Xs_turn,
                        uint8_t depth) {

  int8_t score;
  board_table_t board_table;
  minimax_initializeBoardTable(&board_table);

  // Check if game is over. If so return score
  if (minimax_isGameOver(minimax_computeBoardScore(board, is_Xs_turn))) {
    // Recursion base case, there has been a win or a draw.
    // Evaluate board based upon prev player's turn.
    return (minimax_computeBoardScore(board, !is_Xs_turn) / depth);
  }

  // Otherwise, you need to recurse.
  // This loop will generate all possible boards and call
  // minimax recursively for every empty square
  uint8_t current_square = 0;
  // Iterate over entire board. Run minimax for each empty square
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; ++row) {
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; ++column) {
      if (board->squares[row][column] == MINIMAX_EMPTY_SQUARE) {

        // Simulate playing at this location
        board->squares[row][column] =
            is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

        score = minimax(board, !is_Xs_turn, (depth + 1));

        board_table.score_table[current_square] = score;
        board_table.move_table[current_square].row = row;
        board_table.move_table[current_square].column = column;

        // Undo the change to the board
        board->squares[row][column] = MINIMAX_EMPTY_SQUARE;
      }
      ++current_square;
    }
  }

  // Once you get here, you have iterated over empty squares at this level.
  // All of the scores have been computed in the move-score table for boards
  // at this level. Now you need to return the score depending upon whether
  // you are computing min or max.
  uint8_t index;
  // Check if X or O turn
  // Return corresponding score for X or O turn
  if (is_Xs_turn) {
    index = minimax_getMinMaxIndex(board_table.score_table, MAX);
    choice = board_table.move_table[index]; // get the move with the highest
                                            // score in the move - score table.;
    score = board_table.score_table[index]; // highest score in the move -
                                            // score table.;
  } else {
    index = minimax_getMinMaxIndex(board_table.score_table, MIN);
    choice = board_table.move_table[index]; // get the move with the lowest
                                            // score in the move - score table.;
    score = board_table
                .score_table[index]; // lowest score in the move - score table.;
  }
  return score;
}

// When called from the controlling state machine, you will call this
// function as follows:
// 1. If the computer is playing as X, you will call this function with
// is_Xs_turn = true.
// 2. If the computer is playing as O, you will call this function with
// is_Xs_turn = false.
// This function directly passes the  is_Xs_turn argument into the minimax()
// (helper) function.
tictactoe_location_t minimax_computeNextMove(tictactoe_board_t *board,
                                             bool is_Xs_turn) {
  minimax(board, is_Xs_turn, 0);
  return choice;
}