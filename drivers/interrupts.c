#include "interrupts.h"
#include "armInterrupts.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

#define MER_INIT 0x03

#define TIMER_0 0x0
#define TIMER_1 0x1
#define TIMER_2 0x2

#define ISR 0x00 // Interrupt Status Register
#define IPR 0x04 // Interrupt Pending Register
#define IER 0x08 // Interrupt Enable Register
#define IAR 0x0C // Interrupt Acknowledge Register
#define SIE 0x10 // Set Interrupt Enable
#define CIE 0x14 // Clear Interrupt Enable
#define MER 0x1C // Master Enable Register

#define NUM_INTERRUPT_INPUTS 3

// Global isr fuction pointers
static void (*isrFcnPtrs[NUM_INTERRUPT_INPUTS])() = {NULL};

// Writes value to interrupt controller register given an address offset and a
// value
static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + offset, value);
}

// Reads value from interrupt controller register given an address offset and a
// value
static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_AXI_INTC_0_BASEADDR + offset);
}

// Interrupt request handler
static void interrupts_isr() {

  // Loop through each interrupt input
  for (uint32_t i = 0; i < NUM_INTERRUPT_INPUTS; ++i) {

    // Check if there is an interrupt pending
    if (readRegister(IPR) & (1 << i)) {

      // Check to make sure there is a callback function registered
      if (isrFcnPtrs[i] != NULL) {

        // Execute callback function
        isrFcnPtrs[i]();
      }

      // Acknowledge interrupt [turns off corresponding bit in IPR]
      writeRegister(IAR, (1 << i));
    }
  }
}

// Initialize interrupt hardware
// This function:
// 1. Configures AXI INTC registers to:
//  - Enable interrupt output (see Master Enable Register)
//  - Disable all interrupt input lines.
// 2. Enables the Interrupt system on the ARM processor, and registers an ISR
// handler function. This is done by calling:
//  - armInterrupts_init()
//  - armInterrupts_setupIntc(isr_fcn_ptr)
//  - armInterrupts_enable()
void interrupts_init() {
  // Write 0x3 to master enable register
  writeRegister(MER, MER_INIT);

  // Disable all interrupt input lines on the AXI controller
  interrupts_irq_disable(TIMER_0);
  interrupts_irq_disable(TIMER_1);
  interrupts_irq_disable(TIMER_2);

  // Enable the interrupt system on the ARM processor
  armInterrupts_init();
  armInterrupts_setupIntc(interrupts_isr);
  armInterrupts_enable();
}

// Enable single input interrupt line, given by irq number.
void interrupts_irq_enable(uint8_t irq) { writeRegister(SIE, 1 << irq); }

// Disable single input interrupt line, given by irq number.
void interrupts_irq_disable(uint8_t irq) { writeRegister(CIE, 1 << irq); }

// Register a callback function (fcn is a function pointer to this callback
// function) for a given interrupt input number (irq).  When this interrupt
// input is active, fcn will be called.
void interrupts_register(uint8_t irq, void (*fcn)()) { isrFcnPtrs[irq] = fcn; }