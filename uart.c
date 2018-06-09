#include "stm8s.h"

void tx_string(char str[]) { 
	char index = 0;
	while (str[index]) { // wait until uart tx buffer empty while 
		 // Write Character at Str index to tx buffer 
		while (!(UART1_SR && UART_SR_TXE));
		//next Character index
		UART1_DR = str[index]; 
		index++; 
	}
}

// void uartRead_isr(void) __interrupt(TIM4_ISR) {
    // PD_ODR ^= (1 << OUTPUT_PIN);
    // TIM4_SR &= ~(1 << UIF);
// }

/**
 * send UART interrupt
 * https://github.com/rumpeltux/stm8s-sdcc-examples/blob/master/serial_interrupt.c
*/
// void uart1_isr(void) __interrupt(UART1_TXC_vector) {
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

// // listen UART interrupt
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
	CLK_SWR = 0XE1; // switch to internal oscillator 16 000 000 Hz
	CLK_DIVR = 0x00; // Set the frequency to 16 MHz
	CLK_PCKENR1 = 0xFF; // Enable peripherals clk = 16MHz 0XFF; 
	// enable clock to all peripherals 
	UART1_CR2 = UART_CR2_TEN; // tx enable 
	// 1 STOP bit 
	UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2); //9600 baud - see stm8s reference manual section 22.3.4, 
	//page 331 of 462 
	UART1_BRR2 = 0X03; 
	UART1_BRR1 = 0X68; //9600 baud
	
	// send a string repeatedly
	while (1) { //never ending loop 
		tx_string("Hello!\n"); //call tx_string
		for (i=0;i<0x48000;i++); // wait loop 
	} 
}
