#include "ticTacToeControl.h"
#include "buttons.h"
#include "display.h"
#include "minimax.h"
#include "ticTacToe.h"
#include "ticTacToeDisplay.h"
#include "touchscreen.h"
#include <stdio.h>

#define WAIT_TIME 5 // 3 seconds

// Menu X offsets
#define LINE_1_X 30
#define LINE_2_X 120
#define LINE_3_X 30
#define LINE_4_X 90

// Menus Y offsets
#define LINE_1_Y 80
#define LINE_2_Y 100
#define LINE_3_Y 120
#define LINE_4_Y 140

#define ERASE_FALSE 0
#define ERASE_TRUE 1

// Num rows, Num columns
#define ROWS 3
#define COLUMNS 3

#define TEXT_SIZE 2

// Global variables
static uint32_t
    wait_ticks; // Number of ticks to wait for computer to begin playing
static uint32_t tick_counter;
static tictactoe_board_t board; // Tic Tac Toe board
static tictactoe_location_t user_move;
static tictactoe_location_t computer_move;

static bool is_xs_turn; // Is Xs turn or Os turn

// States
typedef enum {
  TTT_INIT,
  TTT_MENU,
  TTT_RESTART,
  TTT_WAIT_FOR_RELEASE,
  TTT_COMPUTER_TURN,
  TTT_COMPUTER_TURN_GAME_OVER,
  TTT_PROCESS_COMPUTER_TURN,
  TTT_USER_TURN,
  TTT_GET_USER_MOVE,
  TTT_WAIT_FOR_USER_MOVE_RELEASE,
  TTT_VALIDATE_USER_MOVE,
  TTT_END
} ttt_control_state_t;

// state variable
static ttt_control_state_t ttt_state;

// Draw menu
void ticTacToeControl_drawMenu() {
  display_setTextSize(TEXT_SIZE);
  display_setTextColor(DISPLAY_WHITE);
  display_setCursor(LINE_1_X, LINE_1_Y);
  display_print("Touch board to play X");
  display_setCursor(LINE_2_X, LINE_2_Y);
  display_print("--or--");
  display_setCursor(LINE_3_X, LINE_3_Y);
  display_print("wait for the computer");
  display_setCursor(LINE_4_X, LINE_4_Y);
  display_print("and play O");
}

// Clear menu
void ticTacToeControl_clearMenu() {
  display_setTextSize(TEXT_SIZE);
  display_setTextColor(DISPLAY_DARK_BLUE);
  display_setCursor(LINE_1_X, LINE_1_Y);
  display_print("Touch board to play X");
  display_setCursor(LINE_2_X, LINE_2_Y);
  display_print("--or--");
  display_setCursor(LINE_3_X, LINE_3_Y);
  display_print("wait for the computer");
  display_setCursor(LINE_4_X, LINE_4_Y);
  display_print("and play O");
}

// Initialize board with the computer's first move
// This move is always the top left
// This makes it so we don't miss ticks due to
// The long amount of time minimax takes on an
// Empty board
void ticTacDoe_computerFirstMoveInit() {
  board.squares[0][0] = MINIMAX_X_SQUARE;
}

// Takes in a move and updates the board accordingly
void ticTacToeControl_updateBoard(tictactoe_location_t move) {
  // If X turn, Fill in X. If O turn, Fill in O
  if (is_xs_turn) {
    board.squares[move.row][move.column] = MINIMAX_X_SQUARE;
  } else {
    board.squares[move.row][move.column] = MINIMAX_O_SQUARE;
  }
}

// Checks to make sure the the given move isn't for an already occupied square
bool ticTacToeControl_isValidMove(tictactoe_location_t move) {
  // Check if the desired move is on an empty square
  if (board.squares[move.row][move.column] == MINIMAX_EMPTY_SQUARE) {
    return true;
  } else {
    return false;
  }
}

// Erases all moves
void ticTacToeControl_clearScreen() {
  tictactoe_location_t move;

  // Iterate over all moves. Overwrite them with the background color
  for (uint8_t i = 0; i < ROWS; ++i) {
    for (uint8_t j = 0; j < COLUMNS; ++j) {
      move.row = i;
      move.column = j;
      // If X overwrite with X, If O overwrite with O
      if (board.squares[i][j] == MINIMAX_X_SQUARE) {
        ticTacToeDisplay_drawX(move, ERASE_TRUE);
      } else if (board.squares[i][j] == MINIMAX_O_SQUARE) {
        ticTacToeDisplay_drawO(move, ERASE_TRUE);
      }
      is_xs_turn = true;
    }
  }
}

// Tick the tic-tac-toe controller state machine
void ticTacToeControl_tick() {
  // Transistions
  switch (ttt_state) {

  // Initializes
  case TTT_INIT:
    ttt_state = TTT_MENU;
    ticTacToeControl_drawMenu();
    break;

  case TTT_MENU:
    // If tick counter exceeds wait ticks then it is the computer's turn
    // else if the screen is pressed before the timer expires
    // it is player's turn
    if (tick_counter >= wait_ticks) {
      ttt_state = TTT_USER_TURN;
      is_xs_turn = false;
      ticTacToeControl_clearMenu();
      ticTacToeDisplay_init();
      ticTacDoe_computerFirstMoveInit();
      computer_move.row = 0;
      computer_move.column = 0;
      ticTacToeDisplay_drawX(computer_move, ERASE_FALSE);

    } else if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      ttt_state = TTT_WAIT_FOR_RELEASE;
    } else
      ttt_state = TTT_MENU;
    break;

  case TTT_RESTART:
    // Only called if game reset
    // If tick counter exceeds wait ticks then it is the computer's turn
    // else if the screen is pressed before the timer expires
    // it is player's turn
    if (tick_counter >= wait_ticks) {
      ttt_state = TTT_USER_TURN;
      is_xs_turn = false;
      ticTacToeControl_clearMenu();
      ticTacToeDisplay_init();
      ticTacDoe_computerFirstMoveInit();
      computer_move.row = 0;
      computer_move.column = 0;
      ticTacToeDisplay_drawX(computer_move, ERASE_FALSE);

    } else if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      is_xs_turn = true;
      user_move =
          ticTacToeDisplay_getLocationFromPoint(touchscreen_get_location());
      ttt_state = TTT_WAIT_FOR_USER_MOVE_RELEASE;
    } else
      ttt_state = TTT_RESTART;
    break;

  case TTT_WAIT_FOR_RELEASE:
    // If touchscreen is released acknowledge touch and move to next state,
    // Else remain in same state
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      touchscreen_ack_touch();
      ttt_state = TTT_USER_TURN;
      is_xs_turn = true;
      ticTacToeControl_clearMenu();
      ticTacToeDisplay_init();
    } else {
      ttt_state = TTT_WAIT_FOR_RELEASE;
    }
    break;

  case TTT_COMPUTER_TURN_GAME_OVER:
    // Check if game is over.
    // This state is separate from COMPUTER_TURN to try and reduce latency due
    // to minimax
    if (!minimax_isGameOver(minimax_computeBoardScore(&board, is_xs_turn))) {
      ttt_state = TTT_COMPUTER_TURN;
    } else {
      ttt_state = TTT_END;
      is_xs_turn = !is_xs_turn;
    }
    break;

  case TTT_COMPUTER_TURN:
    // Compute next move using minimax
    computer_move = minimax_computeNextMove(&board, is_xs_turn);
    ttt_state = TTT_PROCESS_COMPUTER_TURN;
    break;

  case TTT_PROCESS_COMPUTER_TURN:
    // Check if the computer is playing as X or O. Draw accordingly
    if (is_xs_turn) {
      ticTacToeDisplay_drawX(computer_move, ERASE_FALSE);
    } else {
      ticTacToeDisplay_drawO(computer_move, ERASE_FALSE);
    }
    ticTacToeControl_updateBoard(computer_move);
    is_xs_turn = !is_xs_turn;
    ttt_state = TTT_USER_TURN;
    break;

  case TTT_USER_TURN:
    // Check if game is over. If it isn't move to GET_USER_MOVE
    if (!minimax_isGameOver(minimax_computeBoardScore(&board, is_xs_turn))) {
      ttt_state = TTT_GET_USER_MOVE;
    } else {
      ttt_state = TTT_END;
      is_xs_turn = !is_xs_turn;
    }
    break;

  case TTT_GET_USER_MOVE:
    // If touchscreen is pressed aqcuire touch location.
    // Else remain in state
    if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      user_move =
          ticTacToeDisplay_getLocationFromPoint(touchscreen_get_location());
      ttt_state = TTT_WAIT_FOR_USER_MOVE_RELEASE;
    } else {
      ttt_state = TTT_GET_USER_MOVE;
    }
    break;

  case TTT_WAIT_FOR_USER_MOVE_RELEASE:
    // If released make sure the selected move is valid
    // Else remain in state.
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      ttt_state = TTT_VALIDATE_USER_MOVE;
      touchscreen_ack_touch();
    } else {
      ttt_state = TTT_WAIT_FOR_USER_MOVE_RELEASE;
    }
    break;

  case TTT_VALIDATE_USER_MOVE:
    // Check if move is valid.
    // If so draw X or O depending on what symbol the player is.
    // If invalid move to GET_USER_MOVE to get a new move.
    if (ticTacToeControl_isValidMove(user_move)) {

      if (is_xs_turn) {
        ticTacToeDisplay_drawX(user_move, ERASE_FALSE);
      } else {
        ticTacToeDisplay_drawO(user_move, ERASE_FALSE);
      }
      ticTacToeControl_updateBoard(user_move);
      is_xs_turn = !is_xs_turn;
      ttt_state = TTT_COMPUTER_TURN;
    } else {
      ttt_state = TTT_GET_USER_MOVE;
    }
    break;

  case TTT_END:
    // Check if reset button is pressed.
    // If so move to RESTART.
    // Else remain in END
    if (buttons_read() & BUTTONS_BTN0_MASK) {
      tick_counter = 0;
      ticTacToeControl_clearScreen();
      minimax_initBoard(&board);
      ttt_state = TTT_RESTART;
    } else {
      ttt_state = TTT_END;
    }
    break;
  default:
    break;
  }

  // Actions
  switch (ttt_state) {
  case TTT_INIT:
    break;
  case TTT_MENU:
    // Increase tick counter
    ++tick_counter;
  case TTT_RESTART:
    // Increase tick counter
    ++tick_counter;
    break;
  case TTT_COMPUTER_TURN_GAME_OVER:
    break;
  case TTT_PROCESS_COMPUTER_TURN:
    break;
  case TTT_USER_TURN:
    break;
  case TTT_GET_USER_MOVE:
    break;
  case TTT_WAIT_FOR_USER_MOVE_RELEASE:
    break;
  case TTT_VALIDATE_USER_MOVE:
    break;
  case TTT_END:
    break;
  default:
    break;
  }
}

// Initialize the tic-tac-toe controller state machine,
// providing the tick period, in seconds.
void ticTacToeControl_init(double period_s) {
  wait_ticks = WAIT_TIME / period_s;
  ttt_state = TTT_INIT;
  tick_counter = 0;
  display_init();
  buttons_init();
  minimax_initBoard(&board);
  display_fillScreen(DISPLAY_DARK_BLUE); // Blank the screen.
}