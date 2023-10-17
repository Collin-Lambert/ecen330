#include "clockControl.h"
#include "clockDisplay.h"
#include "touchscreen.h"
#include <stdio.h>

#define DELAY_SECONDS 0.5  // Amount of time to wait before going to fast update
#define UPDATE_SECONDS 0.1 // 1/10th of a second or 10Hz. Used in fast update

// Debug Messages
#define CLOCK_CONTROL_INIT_MSG "TRANSITIONED TO STATE: INITIALIZE\n"
#define CLOCK_CONTROL_WAITING_MSG "TRANSITIONED TO STATE: WAITING\n"
#define CLOCK_CONTROL_INC_DEC_MSG "TRANSITIONED TO STATE: INC DEC\n"
#define CLOCK_CONTROL_LONG_PRESS_MSG "TRANSITIONED TO STATE: LONG PRESS\n"
#define CLOCK_CONTROL_FAST_UPDATE_MSG "TRANSITIONED TO STATE: FAST UPDATE\n"

// State enum
volatile static enum clockControl_state_t {
  CLOCK_CONTROL_INITIALIZE,
  CLOCK_CONTROL_WAITING,
  CLOCK_CONTROL_INC_DEC,
  CLOCK_CONTROL_LONG_PRESS_DELAY,
  CLOCK_CONTROL_FAST_UPDATE,
} clockControl_state;

static uint16_t delay_count;
static uint16_t delay_num_ticks;
static uint16_t update_count;
static uint16_t update_num_ticks;
static touchscreen_status_t status;

// Initialize the clock control state machine, with a given period in seconds.
void clockControl_init(double period_s) {
  clockControl_state = CLOCK_CONTROL_INITIALIZE;
  delay_num_ticks = DELAY_SECONDS / period_s;
  update_num_ticks = UPDATE_SECONDS / period_s;
}

// State machine debugging. When a state transition occurs this function prints
// what the new state is.
void debugClockStatePrint() {
  static enum clockControl_state_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != clockControl_state || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        clockControl_state; // keep track of the last state that you were in.
    switch (clockControl_state) { // This prints messages based upon the state
                                  // that the FSM was in.
    case CLOCK_CONTROL_INITIALIZE:
      printf(CLOCK_CONTROL_INIT_MSG);
      break;
    case CLOCK_CONTROL_WAITING:
      printf(CLOCK_CONTROL_WAITING_MSG);
      break;
    case CLOCK_CONTROL_INC_DEC:
      printf(CLOCK_CONTROL_INC_DEC_MSG);
      break;
    case CLOCK_CONTROL_LONG_PRESS_DELAY:
      printf(CLOCK_CONTROL_LONG_PRESS_MSG);
      break;
    case CLOCK_CONTROL_FAST_UPDATE:
      printf(CLOCK_CONTROL_FAST_UPDATE_MSG);
      break;
    }
  }
}

// Tick the clock control state machine
void clockControl_tick() {
  debugClockStatePrint(); // Print debug messages
  status = touchscreen_get_status();

  // Transistion part of the state machine
  switch (clockControl_state) {
  case CLOCK_CONTROL_INITIALIZE:
    clockControl_state = CLOCK_CONTROL_WAITING;
    break;
  case CLOCK_CONTROL_WAITING:
    // Check if touchscreen is pressed. If so move to long press. If not, move
    // to inc dec
    if (status == TOUCHSCREEN_PRESSED) {
      clockControl_state = CLOCK_CONTROL_LONG_PRESS_DELAY;
      delay_count = 0;
    } else if (status == TOUCHSCREEN_RELEASED) {
      clockControl_state = CLOCK_CONTROL_INC_DEC;
    } else {
      clockControl_state = CLOCK_CONTROL_WAITING;
    }
    break;
  case CLOCK_CONTROL_INC_DEC:
    // Check if touchscreen is released. If so, acknowledge the touch and move
    // to waiting.
    if (status == TOUCHSCREEN_RELEASED) {
      touchscreen_ack_touch();
      clockControl_state = CLOCK_CONTROL_WAITING;
    } else {
      clockControl_state = CLOCK_CONTROL_INC_DEC;
    }
    break;
  case CLOCK_CONTROL_LONG_PRESS_DELAY:
    // Check if touchscreen is released. If so, move to inc dec.
    // Otherwise check if the delay count has reached "DELAY_SECONDS" above.
    // Move to fast update
    if (status == TOUCHSCREEN_RELEASED) {
      clockControl_state = CLOCK_CONTROL_INC_DEC;
    } else if (delay_count == delay_num_ticks) {
      update_count = 0;
      clockControl_state = CLOCK_CONTROL_FAST_UPDATE;
    } else {
      clockControl_state = CLOCK_CONTROL_LONG_PRESS_DELAY;
    }
    break;
  case CLOCK_CONTROL_FAST_UPDATE:
    // If thouchscreen is held and the 10Hz timer has gone off, inc/dec counter.
    // Move to fast update. Otherwise, move to waiting
    if ((status != TOUCHSCREEN_RELEASED) &&
        (update_count == update_num_ticks)) {
      update_count = 0;
      clockDisplay_performIncDec(touchscreen_get_location());
      clockControl_state = CLOCK_CONTROL_FAST_UPDATE;
    } else if (status == TOUCHSCREEN_RELEASED) {
      clockControl_state = CLOCK_CONTROL_WAITING;
      touchscreen_ack_touch();
    }
    break;
  default:
    clockControl_state = CLOCK_CONTROL_INITIALIZE;
    break;
  }

  // Action part of the state machine
  switch (clockControl_state) {
  case CLOCK_CONTROL_INITIALIZE:
    break;
  case CLOCK_CONTROL_WAITING:
    break;
  case CLOCK_CONTROL_INC_DEC:
    clockDisplay_performIncDec(touchscreen_get_location());
    break;
  case CLOCK_CONTROL_LONG_PRESS_DELAY:
    ++delay_count;
    break;
  case CLOCK_CONTROL_FAST_UPDATE:
    ++update_count;
    break;
  default:
    clockControl_state = CLOCK_CONTROL_INITIALIZE;
    break;
  }
}