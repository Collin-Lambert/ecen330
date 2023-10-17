#include "buttons.h"
#include <assert.h>
#include "xparameters.h"
#include "xil_io.h"

// Offset values for TRI and DATA registers
#define BUTTONS_TRI_OFFSET 0x4
#define BUTTONS_DATA_OFFSET 0x0

#define BUTTONS_INPUT_ENABLE 0xF


// Returns 32 bit value for the button register given an address offset value
static uint32_t readRegister(uint32_t offset) {
   return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

// Writes value to button register given an address offset value
static void writeRegister(uint32_t offset, uint32_t value) {
   Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}

// Initializes the button driver software and hardware.
void buttons_init() {
    // Base address + 0x4 addresses the TRI register.
    // To configure as input write 1s to TRI register
    writeRegister(BUTTONS_TRI_OFFSET, BUTTONS_INPUT_ENABLE);

    // Read TRI register to verify number written is present and that hardware is present
    assert(readRegister(BUTTONS_TRI_OFFSET) == BUTTONS_INPUT_ENABLE); 
}

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
uint8_t buttons_read() {
    return readRegister(BUTTONS_DATA_OFFSET);
}