#include "touchscreen.h"
#include <stdio.h>

#define SETTLE_TIME 0.05F // 50ms

#define INIT_ST_MSG "TRANSITIONED TO STATE: TOUCHSCREEN INITIALIZE\n"
#define WAITING_ST_MSG "TRANSITIONED TO STATE: TOUCHSCREEN WAITING\n"
#define ADC_SETTLING_MSG "TRANSITIONED TO STATE: TOUCHSCREEN ADC SETTLING\n"
#define PRESSED_ST_MSG "TRANSITIONED TO STATE: TOUCHSCREEN PRESSED ST\n"

volatile static enum touchscreen_state_t {
  TOUCHSCREEN_INITIALIZE,
  TOUCHSCREEN_WAITING,
  TOUCHSCREEN_ADC_SETTLING,
  TOUCHSCREEN_PRESSED_ST
} touchscreen_state;

volatile static touchscreen_status_t touchscreen_status;

static uint32_t adc_timer, adc_settle_ticks;
static uint8_t pressed;

// Touchscreen touch location variables
static uint16_t x = 0;
static uint16_t y = 0;
static uint8_t z = 0;

// Initialize the touchscreen driver state machine, with a given tick period (in
// seconds).
void touchscreen_init(double period_seconds) {
  pressed = 0;
  adc_timer = 0;
  // Settles after 50ms
  adc_settle_ticks = (SETTLE_TIME / period_seconds);
}

// State machine debugging. When a state transition occurs this function prints
// what the new state is.
void debugStatePrint() {
  static enum touchscreen_state_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != touchscreen_state || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        touchscreen_state; // keep track of the last state that you were in.
    switch (touchscreen_state) { // This prints messages based upon the state
                                 // that the FSM was in.
    case TOUCHSCREEN_INITIALIZE:
      printf(INIT_ST_MSG);
      break;
    case TOUCHSCREEN_WAITING:
      printf(WAITING_ST_MSG);
      break;
    case TOUCHSCREEN_ADC_SETTLING:
      printf(ADC_SETTLING_MSG);
      break;
    case TOUCHSCREEN_PRESSED_ST:
      printf(PRESSED_ST_MSG);
      break;
    }
  }
}

// Tick the touchscreen driver state machine
void touchscreen_tick() {
  // Enable debug
  // debugStatePrint();

  // State transition
  switch (touchscreen_state) {

  case TOUCHSCREEN_INITIALIZE: // Basic init function for startup
    touchscreen_state = TOUCHSCREEN_WAITING;
    break;

  case TOUCHSCREEN_WAITING: // Waits for a touch to be detected
    // If a touch is detected transition to ADC_SETTLING and clear old touch
    // data
    if (display_isTouched()) {
      touchscreen_state = TOUCHSCREEN_ADC_SETTLING;
      display_clearOldTouchData();
    } else {
      touchscreen_state = TOUCHSCREEN_WAITING;
    }
    break;

  case TOUCHSCREEN_ADC_SETTLING: // Wait 50ms to make sure the touchscreen has
                                 // time to settle its output
    // Check that the timer has reached 50ms and that the display is still
    // touched. Then, move to touchscreen pressed
    if (!display_isTouched()) {
      touchscreen_state = TOUCHSCREEN_WAITING;
      touchscreen_status = TOUCHSCREEN_IDLE;
    } else if (display_isTouched() && (adc_timer == adc_settle_ticks)) {
      touchscreen_state = TOUCHSCREEN_PRESSED_ST;
      display_getTouchedPoint(&x, &y, &z);
      touchscreen_status = TOUCHSCREEN_PRESSED;
    } else {
      touchscreen_state = TOUCHSCREEN_ADC_SETTLING;
    }
    break;

  case TOUCHSCREEN_PRESSED_ST: // Wait for screen to no longer be touched
    // If screen is no longer touched transition back to the WAITING state.
    if (!display_isTouched()) {
      touchscreen_state = TOUCHSCREEN_WAITING;
      touchscreen_status = TOUCHSCREEN_RELEASED;
    } else {
      touchscreen_state = TOUCHSCREEN_PRESSED_ST;
    }
    break;

  default:
    touchscreen_state = TOUCHSCREEN_INITIALIZE;
    break;
  }

  // State actions
  switch (touchscreen_state) {

  case TOUCHSCREEN_INITIALIZE:
    touchscreen_status = TOUCHSCREEN_IDLE; // Set status to IDLE
    break;

  case TOUCHSCREEN_WAITING:
    adc_timer = 0; // Clear timer
    break;

  case TOUCHSCREEN_ADC_SETTLING:
    adc_timer++; // Increment timer
    break;

  case TOUCHSCREEN_PRESSED_ST:
    pressed = true;
    break;

  default:
    // Do Nothing
    break;
  }
}

// Return the current status of the touchscreen
touchscreen_status_t touchscreen_get_status() {
  // printf("STATUS: %d\n", touchscreen_status);
  return touchscreen_status;
}

// Acknowledge the touchscreen touch.  This function will only have effect when
// the touchscreen is in the TOUCHSCREEN_RELEASED status, and will cause it to
// switch to the TOUCHSCREEN_IDLE status.
void touchscreen_ack_touch() {
  if (touchscreen_status == TOUCHSCREEN_RELEASED) {
    touchscreen_status = TOUCHSCREEN_IDLE;
  }
}

// Get the (x,y) location of the last touchscreen touch
display_point_t touchscreen_get_location() {
  display_point_t touchpoint;
  touchpoint.x = x;
  touchpoint.y = y;
  return touchpoint;
}