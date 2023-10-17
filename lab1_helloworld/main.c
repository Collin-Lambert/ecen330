/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdio.h>

#include "display.h"

#define ORIGIN 0

// Width defines
#define EIGHTH_WIDTH (DISPLAY_WIDTH / 8)
#define QUARTER_WIDTH (DISPLAY_WIDTH / 4)
#define HALF_WIDTH (DISPLAY_WIDTH / 2)
#define THREE_QUARTER_WIDTH (3 * (DISPLAY_WIDTH / 4))

// Height defines
#define EIGHTH_HEIGHT (DISPLAY_HEIGHT / 8)
#define QUARTER_HEIGHT (DISPLAY_HEIGHT / 4)
#define HALF_HEIGHT (DISPLAY_HEIGHT / 2)
#define THREE_QUARTER_HEIGHT (3 * (DISPLAY_HEIGHT / 4))

// Circle defines
#define CIRCLE_RADIUS 30

// Triangle defines
#define UPPER_TRIANGLE_UPPER_BOUND QUARTER_HEIGHT - EIGHTH_HEIGHT
#define UPPER_TRIANGLE_LOWER_BOUND HALF_HEIGHT - EIGHTH_HEIGHT
#define LOWER_TRIANGLE_LOWER_BOUND THREE_QUARTER_HEIGHT + EIGHTH_HEIGHT
#define LOWER_TRIANGLE_UPPER_BOUND HALF_HEIGHT + EIGHTH_HEIGHT
#define TRIANGLE_LEFT_BOUND HALF_WIDTH - 30
#define TRIANGLE_RIGHT_BOUND HALF_WIDTH + 30

// Print out example pattern
int main() {

  // Initialize display driver, and fill scren with black
  display_init();
  display_fillScreen(DISPLAY_BLACK); // Blank the screen.

  // Draw diagonal lines
  display_drawLine(ORIGIN, ORIGIN, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_GREEN);
  display_drawLine(ORIGIN, DISPLAY_HEIGHT, DISPLAY_WIDTH, ORIGIN, DISPLAY_GREEN);

  // Draw Circles
  display_drawCircle(QUARTER_WIDTH, HALF_HEIGHT, CIRCLE_RADIUS, DISPLAY_RED);
  display_fillCircle(THREE_QUARTER_WIDTH, HALF_HEIGHT, CIRCLE_RADIUS, DISPLAY_RED);
  
  // Draw Triangles
  display_fillTriangle(TRIANGLE_LEFT_BOUND, UPPER_TRIANGLE_UPPER_BOUND, TRIANGLE_RIGHT_BOUND, UPPER_TRIANGLE_UPPER_BOUND, HALF_WIDTH, UPPER_TRIANGLE_LOWER_BOUND , DISPLAY_YELLOW);
  display_drawTriangle(TRIANGLE_LEFT_BOUND, LOWER_TRIANGLE_LOWER_BOUND, TRIANGLE_RIGHT_BOUND, LOWER_TRIANGLE_LOWER_BOUND, HALF_WIDTH, LOWER_TRIANGLE_UPPER_BOUND, DISPLAY_YELLOW);

  return 0;
}
