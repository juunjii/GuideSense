/*
 * File:   usart.c
 * Author: chehj
 *
 * Description:
 * Implementation of USART (Universal Synchronous Asynchronous Receiver Transmitter) functions.
 * Includes USART initialization and a function for reading characters from USART.
 *
 * Created on December 1, 2024, 8:45 PM
 */

#include "usart.h"

/**
 * @brief Initializes the USART1 module for communication.
 * Configures the UART pins, sets baud rate, enables RX and TX, and configures
 * other settings like data bits, parity, stop bits, and asynchronous mode.
 */
void usartInit() {
    // Configure UART pins:
    // RX (PIN1) as input
    PORTC.DIR &= ~PIN1_bm;
    // TX (PIN0) as output
    PORTC.DIR |= PIN0_bm;
    
    // Initialize the USART baud rate to 115200
    // USART_BAUD_VALUE macro calculates the correct value based on the desired baud rate
    USART1.BAUD = USART_BAUD_VALUE(115200);
    
    // Enable RX (Receiver) and TX (Transmitter), set RX mode to normal
    USART1.CTRLB = USART_TXEN_bm | USART_RXEN_bm | USART_RXMODE_NORMAL_gc;
    
    // Configure USART frame format:
    // - 8 data bits
    // - No parity
    // - 1 stop bit
    // - Asynchronous mode
    USART1.CTRLC = USART_CHSIZE_8BIT_gc | 
                   USART_PMODE_DISABLED_gc |  // No parity
                   USART_SBMODE_1BIT_gc |    // 1 stop bit
                   USART_CMODE_ASYNCHRONOUS_gc;  // Asynchronous mode

    // Enable debug run mode, allowing the USART to run during debugging
    USART1.DBGCTRL = USART_DBGRUN_bm;
}

/**
 * @brief Reads a character from the USART1 data register.
 * Waits until data is received and then returns the received character.
 * 
 * @return Received character from USART.
 */
char usartReadChar() {
    // Wait until data is available in the USART RX buffer
    while (!(USART1.STATUS & USART_RXCIF_bm));  // Wait for RXC (Receive Complete) flag
    
    // Return the received character from the data register
    return USART1.RXDATAL;
}
