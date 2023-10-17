#include "gpioTest.h"
#include "display.h"
#include "buttons.h"
#include "switches.h"
#include "leds.h"


#define UPPER_BOUND 0
#define LEFT_BOUND 0
#define BOX_WIDTH (DISPLAY_WIDTH / 4)
#define BOX_HEIGHT (DISPLAY_HEIGHT / 2)
#define BTN_3_2_EDGE (DISPLAY_WIDTH / 4)
#define BTN_2_1_EDGE (DISPLAY_WIDTH / 2)
#define BTN_1_0_EDGE (3 * (DISPLAY_WIDTH / 4))
#define RIGHT_BOUND DISPLAY_WIDTH

#define CURSOR_HEIGHT (DISPLAY_HEIGHT / 4) - 10
#define TEXT_SIZE 2
#define TEXT_0_COORD BTN_1_0_EDGE + 17
#define TEXT_1_COORD BTN_2_1_EDGE + 17
#define TEXT_2_COORD BTN_3_2_EDGE + 17
#define TEXT_3_COORD LEFT_BOUND + 17

#define BUTTON_0_COLOR 0xffcc // Yellowish
#define BUTTON_1_COLOR 0x7ec5 // Green
#define BUTTON_2_COLOR 0xfcc4 // Oragne
#define BUTTON_3_COLOR 0x4cba // Blue

#define ALL_SWITCHES_ON 0x0F
#define ALL_BUTTONS_ON 0x0F

// Draw button box on screen
void drawButtonBoxes(int buttonsPressed) {
    // if buttons_pressed
    if (buttonsPressed & BUTTONS_BTN3_MASK) {
        display_fillRect(LEFT_BOUND, UPPER_BOUND, BOX_WIDTH, BOX_HEIGHT, BUTTON_3_COLOR);
        display_setCursor(TEXT_3_COORD, CURSOR_HEIGHT);
        display_print("BTN3");
    }
    else {
        display_fillRect(LEFT_BOUND, UPPER_BOUND, BOX_WIDTH, BOX_HEIGHT, DISPLAY_BLACK);
    }
    if (buttonsPressed & BUTTONS_BTN2_MASK) {
        display_fillRect(BTN_3_2_EDGE, UPPER_BOUND, BOX_WIDTH, BOX_HEIGHT, BUTTON_2_COLOR);
        display_setCursor(TEXT_2_COORD, CURSOR_HEIGHT);
        display_print("BTN2");
    }
    else {
        display_fillRect(BTN_3_2_EDGE, UPPER_BOUND, BOX_WIDTH, BOX_HEIGHT, DISPLAY_BLACK);
    }
    if (buttonsPressed & BUTTONS_BTN1_MASK) {
        display_fillRect(BTN_2_1_EDGE, UPPER_BOUND, BOX_WIDTH, BOX_HEIGHT, BUTTON_1_COLOR);
        display_setCursor(TEXT_1_COORD, CURSOR_HEIGHT);
        display_print("BTN1");
    }
    else {
        display_fillRect(BTN_2_1_EDGE, UPPER_BOUND, BOX_WIDTH, BOX_HEIGHT, DISPLAY_BLACK);
    }
    if (buttonsPressed & BUTTONS_BTN0_MASK) {
        display_fillRect(BTN_1_0_EDGE, UPPER_BOUND, BOX_WIDTH, BOX_HEIGHT, BUTTON_0_COLOR);
        display_setCursor(TEXT_0_COORD, CURSOR_HEIGHT);
        display_print("BTN0");
    }
    else {
        display_fillRect(BTN_1_0_EDGE, UPPER_BOUND, BOX_WIDTH, BOX_HEIGHT, DISPLAY_BLACK);
    }
}


// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test will run until all 4 pushbuttons are
// simultaneously pressed.
void gpioTest_buttons() {
    buttons_init();
    display_init();
    display_fillScreen(DISPLAY_BLACK); // Blank the screen.
    display_setTextSize(TEXT_SIZE);
    display_setTextColor(DISPLAY_WHITE);

    uint8_t buttons = buttons_read();
    uint8_t buttons_prev = buttons_read();

    // Loop until all buttons are pressed simultaneously
    while (buttons != ALL_BUTTONS_ON) {
        buttons = buttons_read();

        // Checks to see if any button has changed state
        // Only draw to the screen when a change is detected
        if (buttons != buttons_prev) {
            drawButtonBoxes(buttons);
        }

        buttons_prev = buttons;
    }
}

// Runs a test of the switches. As you slide the switches, LEDs directly above
// the switches will illuminate. The test will run until all switches are slid
// upwards. When all 4 slide switches are slid upward, this function will
// return.
void gpioTest_switches() {
    switches_init();
    leds_init();

    uint8_t switches = switches_read();

    // Loop until all switches are slid upward
    while (switches != ALL_SWITCHES_ON) {
        switches = switches_read();

        leds_write(switches);
    }
    // Clear LEDs
    leds_write(0x0);
}