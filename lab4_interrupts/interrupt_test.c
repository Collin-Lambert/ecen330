
#include "interrupts.h"
#include "intervalTimer.h"
#include "leds.h"

#define TIMER0_PERIOD 0.1
#define TIMER1_PERIOD 1
#define TIMER2_PERIOD 10

#define TIMER_0 0x0
#define TIMER_1 0x1
#define TIMER_2 0x2

// Interrupt service routine for timer 0. Toggles LED on and off at frequency of
// 10Hz
void timer0_isr() {
  intervalTimer_ackInterrupt(TIMER_0);
  // Toggle LED. If LED is on, turn it off. Vice Versa
  if (leds_read() & (1 << TIMER_0)) {
    leds_write(leds_read() & ~(1 << TIMER_0));
  } else {
    leds_write(leds_read() | (1 << TIMER_0));
  }
}

// Interrupt service routine for timer 1. Toggles LED on and off at frequency of
// 1Hz
void timer1_isr() {
  intervalTimer_ackInterrupt(TIMER_1);
  // Toggle LED. If LED is on, turn it off. Vice Versa
  if (leds_read() & (1 << TIMER_1)) {
    leds_write(leds_read() & ~(1 << TIMER_1));
  } else {
    leds_write(leds_read() | (1 << TIMER_1));
  }
}

// Interrupt service routine for timer 2. Toggles LED on and off at frequency of
// 0.1Hz
void timer2_isr() {
  intervalTimer_ackInterrupt(TIMER_2);
  // Toggle LED. If LED is on, turn it off. Vice Versa
  if (leds_read() & (1 << TIMER_2)) {
    leds_write(leds_read() & ~(1 << TIMER_2));
  } else {
    leds_write(leds_read() | (1 << TIMER_2));
  }
}

/*
This function is a small test application of your interrupt controller.  The
goal is to use the three AXI Interval Timers to generate interrupts at different
rates (10Hz, 1Hz, 0.1Hz), and create interrupt handler functions that change the
LEDs at this rate.  For example, the 1Hz interrupt will flip an LED value each
second, resulting in LED that turns on for 1 second, off for 1 second,
repeatedly.

For each interval timer:
    1. Initialize it as a count down timer with appropriate period.
    2. Enable the timer's interrupt output.  printf("%d\n", irq);
    3. Enable the associated interrupt input on the interrupt controller.
    4. Register an appropriate interrupt handler function (isr_timer0,
isr_timer1, isr_timer2).
    5. Start the timer.

Make sure you call `interrupts_init()` first!
*/
void interrupt_test_run() {
  // Initialize counters as count down.
  intervalTimer_initCountDown(TIMER_0, TIMER0_PERIOD);
  intervalTimer_initCountDown(TIMER_1, TIMER1_PERIOD);
  intervalTimer_initCountDown(TIMER_2, TIMER2_PERIOD);

  // Initialize Interrupt Controller
  interrupts_init();

  // Enable irq inputs on interrupt controller
  interrupts_irq_enable(TIMER_0);
  interrupts_irq_enable(TIMER_1);
  interrupts_irq_enable(TIMER_2);

  // Enable interrupt outputs on timers
  intervalTimer_enableInterrupt(TIMER_0);
  intervalTimer_enableInterrupt(TIMER_1);
  intervalTimer_enableInterrupt(TIMER_2);

  // Register ISRs
  interrupts_register(TIMER_0, timer0_isr);
  interrupts_register(TIMER_1, timer1_isr);
  interrupts_register(TIMER_2, timer2_isr);

  // Start Timers
  intervalTimer_start(TIMER_0);
  intervalTimer_start(TIMER_1);
  intervalTimer_start(TIMER_2);

  // LEDs init
  leds_init();
}
