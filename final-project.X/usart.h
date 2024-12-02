/*
 * File:   usart.h
 * Author: chehj
 *
 * Created on December 1, 2024, 8:45 PM
 */

#ifndef USART_H
#define USART_H

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 3333333
#endif


// Macros
#define USART_BAUD_VALUE(baud_rate) ((F_CPU / (16UL * baud_rate)) - 1)

// Function Prototypes
/**
 * @brief Initializes the USART module with predefined settings.
 */
void usartInit(void);

/**
 * @brief Reads a character from the USART RX buffer.
 * 
 * @return The received character.
 */
char usartReadChar(void);

#endif /* USART_H */
