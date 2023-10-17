
#include "intervalTimer.h"
#include "xil_io.h"
#include "xparameters.h"

#define TCSR0_OFFSET 0x00 // Control status register
#define TLR0_OFFSET 0x04  // Load register
#define TCR0_OFFSET 0x08  // Timer/counter register
#define TCSR1_OFFSET 0x10 // Control status register
#define TLR1_OFFSET 0x14  // Load register
#define TCR1_OFFSET 0x18  // Timer/counter register

#define ZEROS_32_BIT 0x00000000

// address map of TCSR0
// bit 31-12 reserved
// bit 11 CASC  <- 1 enable cascade, 0 disable cascade
// bit 10 ENALL - UNUSED
// bit  9 PWMA0 - UNUSED
// bit  8 T0INT < - interupt status, write 1 to acknowledge interrupt
// bit  7 ENT0  <- 1 enable timer, 0 disable timer
// bit  6 ENIT0 <- 1 enable interrupts, 0 disable interrupts
// bit  5 LOAD0 <- 1 load timer with value in TLR0, 0 no load
// bit  4 ARHT0 <- 1 autoreload, 0 hold value
// bit  3 CAPT0 - UNUSED
// bit  2 GENT0 - UNUSED
// bit  1 UDT0  <-  1 counter acts as down counter, 0 counter acts as up counter
// bit  0 MDT0 - UNUSED

#define CASC 11
#define T0INT 8
#define ENT 7
#define ENIT 6
#define LOAD 5
#define ARHT 4
#define UDT 1

#define CLK_HZ 100000000.0
#define UPPER_COUNTER_SHIFT_VAL 32

// Returns 32 bit value of register given register number
// and an address offset value
static uint32_t readRegister(uint8_t registerNum, uint32_t offset) {

  // Read and return value from specific register given offset
  switch (registerNum) {
  case INTERVAL_TIMER_0:
    return Xil_In32(XPAR_AXI_TIMER_0_BASEADDR + offset);
    break;
  case INTERVAL_TIMER_1:
    return Xil_In32(XPAR_AXI_TIMER_1_BASEADDR + offset);
    break;
  case INTERVAL_TIMER_2:
    return Xil_In32(XPAR_AXI_TIMER_2_BASEADDR + offset);
    break;
  default:
    break;
  }
}

// Writes value to register given register number and an address offset value
static void writeRegister(uint8_t registerNum, uint32_t offset,
                          uint32_t value) {

  // Write value to specific register given offset and value
  switch (registerNum) {
  case INTERVAL_TIMER_0:
    Xil_Out32(XPAR_AXI_TIMER_0_BASEADDR + offset, value);
    break;
  case INTERVAL_TIMER_1:
    Xil_Out32(XPAR_AXI_TIMER_1_BASEADDR + offset, value);
    break;
  case INTERVAL_TIMER_2:
    Xil_Out32(XPAR_AXI_TIMER_2_BASEADDR + offset, value);
    break;
  default:
    break;
  }
}

// Writes value to a specific bit in a register given a register num, bit num,
// offset, and value
static void writeBitOfRegister(uint8_t registerNum, uint8_t bitNum,
                               uint32_t offset, bool value) {
  uint32_t bitMask = (1 << bitNum);
  uint32_t regValue = readRegister(registerNum, offset);

  // Determines whether to write 1 or 0 to specified bit
  if (value) {
    writeRegister(registerNum, offset,
                  (regValue | bitMask)); // Write 1 to specified bit
  } else {
    writeRegister(registerNum, offset,
                  (regValue & ~bitMask)); // Write 0 to specified bit
  }
}

// This function is called whenever you want to reload the Counter values
// from the load registers.  For a count-up timer, this will reset the
// timer to zero.  For a count-down timer, this will reset the timer to
// its initial count-down value.  The load registers should have already
// been set in the appropriate `init` function.
void intervalTimer_reload(uint32_t timerNumber) {
  // Load timer 0
  // Load value of TLR into TCR
  writeBitOfRegister(timerNumber, LOAD, TCSR0_OFFSET, 1);
  writeBitOfRegister(timerNumber, LOAD, TCSR0_OFFSET, 0); // Stop loading

  // Load timer 1
  // Load value of TLR into TCR
  writeBitOfRegister(timerNumber, LOAD, TCSR1_OFFSET, 1);
  writeBitOfRegister(timerNumber, LOAD, TCSR1_OFFSET, 0); // Stop loading
}

// Initializes a timer to function as an incrementing timer
// Timer will also be configured to be in cascade mode which takes advantage of
// both counter registers This allows the timer to count up to a 64 bit number
void intervalTimer_initCountUp(uint32_t timerNumber) {
  // write zeros to TCSR registers
  writeRegister(timerNumber, TCSR0_OFFSET, ZEROS_32_BIT);
  writeRegister(timerNumber, TCSR1_OFFSET, ZEROS_32_BIT);

  // write 1 to TCSR0 CASC bit
  writeBitOfRegister(timerNumber, CASC, TCSR0_OFFSET, 1); // Enable cascading

  // write 0 to TCSR0 UDT bit
  writeBitOfRegister(timerNumber, UDT, TCSR0_OFFSET,
                     0); // Configure as up counter

  // load zeros into load register counter 0
  // When timer is reset this is the value it will reset to
  writeRegister(timerNumber, TLR0_OFFSET,
                ZEROS_32_BIT); // Fill load register with zeros

  // load zeros into load register of counter 1
  // When timer is reset this is the value it will reset to
  writeRegister(timerNumber, TLR1_OFFSET,
                ZEROS_32_BIT); // Fill load register with zeros

  // Move load register values into counter registers
  intervalTimer_reload(timerNumber);
}

// Initializes a timer to function as an decrementing timer with a given period
// of 'period' Timer will also be configured to be in cascade mode which takes
// advantage of both counter registers This allows the timer to count up to a 64
// bit number
void intervalTimer_initCountDown(uint32_t timerNumber, double period) {
  // write zeros to TCSR registers
  writeRegister(timerNumber, TCSR0_OFFSET, ZEROS_32_BIT);
  writeRegister(timerNumber, TCSR1_OFFSET, ZEROS_32_BIT);

  // write 1 to TCSR0 CASC bit
  writeBitOfRegister(timerNumber, CASC, TCSR0_OFFSET, 1); // Enable cascading

  // write 0 to TCSR0 UDT bit
  writeBitOfRegister(timerNumber, UDT, TCSR0_OFFSET,
                     1); // Configure as down counter

  // write 1 to TCSR0 ARHT bit
  writeBitOfRegister(timerNumber, ARHT, TCSR0_OFFSET,
                     1); // Configure to auto reload when reaching zero

  uint64_t counterVal =
      CLK_HZ *
      period; // Convert 'period' which is given in seconds, to clock cycles

  // load zeros into load register counter 1
  writeRegister(
      timerNumber, TLR1_OFFSET,
      (counterVal >> UPPER_COUNTER_SHIFT_VAL)); // Fill load register with
                                                // correct value for period

  // load zeros into load register of counter 0
  writeRegister(timerNumber, TLR0_OFFSET,
                counterVal); // Fill load register with correct value for period

  // Move load register values into counter registers
  intervalTimer_reload(timerNumber);
}

// This function ascertains how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
// In cascade mode it is necessary to read the upper and lower 32-bit registers,
// concatenate them into a 64-bit counter value, and then perform the conversion
// to a double seconds value.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  uint64_t upperRegValue = readRegister(timerNumber, TCR1_OFFSET);
  uint64_t lowerRegValue = readRegister(timerNumber, TCR0_OFFSET);

  uint64_t totalRegValue =
      (upperRegValue << UPPER_COUNTER_SHIFT_VAL) | lowerRegValue;
  return (totalRegValue / CLK_HZ);
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber) {
  writeBitOfRegister(timerNumber, ENT, TCSR0_OFFSET, 1);
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber) {
  writeBitOfRegister(timerNumber, ENT, TCSR0_OFFSET, 0);
}

// Enable the interrupt output of the given timer.
void intervalTimer_enableInterrupt(uint8_t timerNumber) {
  writeBitOfRegister(timerNumber, ENIT, TCSR0_OFFSET, 1);
}

// Disable the interrupt output of the given timer.
void intervalTimer_disableInterrupt(uint8_t timerNumber) {
  writeBitOfRegister(timerNumber, ENIT, TCSR0_OFFSET, 0);
}

// Acknowledge the rollover to clear the interrupt output.
void intervalTimer_ackInterrupt(uint8_t timerNumber) {
  writeBitOfRegister(timerNumber, T0INT, TCSR0_OFFSET, 1); // Clears interrupt
}
