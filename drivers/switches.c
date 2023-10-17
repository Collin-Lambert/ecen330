#include "switches.h"
#include <assert.h>
#include "xparameters.h"
#include "xil_io.h"

// Offset values for TRI and DATA registers
#define SWITCHES_TRI_OFFSET 0x4
#define SWITCHES_DATA_OFFSET 0x0

#define SWITCHES_INPUT_ENABLE 0xF

// Returns 32 bit value for the button register given an address offset value
static uint32_t readRegister(uint32_t offset) {
   return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

// Writes value to button register given an address offset value
static void writeRegister(uint32_t offset, uint32_t value) {
   Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);
}

// Initializes the SWITCHES driver software and hardware.
void switches_init() {
    // Base address + 0x4 addresses the TRI register.
    // To configure as input write 1s to TRI register
    writeRegister(SWITCHES_TRI_OFFSET, SWITCHES_INPUT_ENABLE);

    // Read TRI register to verify number written is present and that hardware is present
    assert(readRegister(SWITCHES_TRI_OFFSET) == SWITCHES_INPUT_ENABLE); 
}

// Returns the current value of all 4 switches as the lower 4 bits of the
// returned value. bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
uint8_t switches_read() {
    return readRegister(SWITCHES_DATA_OFFSET);
}