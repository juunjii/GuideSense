/*
 * File:   usart.c
 * Author: chehj
 *
 * Created on December 1, 2024, 8:45 PM
 */


#include "usart.h"

void usartInit() {
    // Configure UART pins
    PORTC.DIR &= ~PIN1_bm;  // RX as input
    PORTC.DIR |= PIN0_bm;   // TX as output
    
    // Initialize UART with proper settings first
    USART1.BAUD = USART_BAUD_VALUE(115200);
    
    // Enable RX and TX
    USART1.CTRLB = USART_TXEN_bm | USART_RXEN_bm | USART_RXMODE_NORMAL_gc;
    
    // 8 data bits, no parity, 1 stop bit, asynchronous mode
    USART1.CTRLC = USART_CHSIZE_8BIT_gc | 
                   USART_PMODE_DISABLED_gc |
                   USART_SBMODE_1BIT_gc |
                   USART_CMODE_ASYNCHRONOUS_gc;
                   
    // Enable debug run
    USART1.DBGCTRL = USART_DBGRUN_bm;
}

char usartReadChar() {
    // Wait for data to be received
    while (!(USART1.STATUS & USART_RXCIF_bm));
    return USART1.RXDATAL;
}
