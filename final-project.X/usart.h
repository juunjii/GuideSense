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

#define SAMPLES_PER_BIT 16
#define USART_BAUD_VALUE(BAUD_RATE) (uint16_t) ((F_CPU << 6) / (((float) SAMPLES_PER_BIT) * (BAUD_RATE)) + 0.5)
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
