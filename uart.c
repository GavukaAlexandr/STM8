#include "stm8s.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define wfi()                                                                  \
  { __asm__("wfi\n"); } /* Wait For Interrupt */
#define rim()                                                                  \
  { __asm__("rim"); } /* Enable interrupts */

#define CLRBIT(REG, VALUE) REG &= ~VALUE
#define SETBIT(REG, VALUE) REG |= ~VALUE

char *buffer;
int buffer_position = 0;
int buffer_size = 0;

/**
 * send UART interrupt
 * https://github.com/rumpeltux/stm8s-sdcc-examples/blob/master/serial_interrupt.c
*/
// #define UART1_T_TXE_vector 17

void uart1_isr(void) __interrupt(UART1_TXC_vector) {
  // The vector is used for multiple purposes, check that TXE is indeed set.
  if (UART1_SR & UART_SR_TXE) {
    if (buffer_position < buffer_size)
      UART1_DR = buffer[buffer_position++];
    else {
      // Transfer complete. Disable interrupts.
      // The main loop detects this condition.
      CLRBIT(UART1_CR2, UART_CR2_TIEN);
    }
  }
}

void putstring(char *s) {
  buffer = s;
  buffer_position = 0;
  buffer_size = strlen(s);
  // Enables the interrupt, which is called immediately if TXE is already set.
  SETBIT(UART1_CR2, UART_CR2_TIEN);
}

// listen UART interrupt
// void uart1_isr(void) __interrupt(UART1_RXF_vector) {
//   // The vector is used for multiple purposes, check that TXE is indeed set.
//   if (UART1_SR & UART_SR_TXE) {
//     if (buffer_position < buffer_size)
//       UART1_DR = buffer[buffer_position++];
//     else {
//       // Transfer complete. Disable interrupts.
//       // The main loop detects this condition.
//       CLRBIT(UART1_CR2, UART_CR2_TIEN);
//     }
//   }
// }


int main() { 
	unsigned long i = 0;
	CLK_DIVR = 0XE1; // switch to internal oscillator 16 000 000 Hz
	CLK_DIVR = 0x00; // Set the frequency to 16 MHz
	CLK_PCKENR1 = 0xFF; // Enable peripherals clk = 16MHz 0XFF; 
	// enable clock to all peripherals 
	UART1_CR2 = UART_CR2_TEN; // tx enable 
	// 1 STOP bit 
	UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2); //9600 baud - see stm8s reference manual section 22.3.4, 
	//page 331 of 462 
	UART1_BRR2 = 0X03; 
	UART1_BRR1 = 0X68; //9600 baud
	
	rim();

	  for (;;) {
    putstring("Hello world!\n");
    do {
      wfi(); // Puts the processor in wait mode until an interrupt is received.
    } while (UART1_CR2 & UART_CR2_TIEN); // When the transmission is complete, the interrupt handler clears this flag.

    for (i = 0; i < 147456; i++); // Sleep
  }
}
