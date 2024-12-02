#ifndef PRINTF_H
#define PRINTF_H

#include <avr/io.h>
#include <string.h>

#define SAMPLES_PER_BIT 16
#define USART2_BAUD_VALUE(BAUD_RATE) (uint16_t)((F_CPU << 6) / (((float)SAMPLES_PER_BIT) * (BAUD_RATE)) + 0.5)

/**
 * @brief Initializes USART2 for communication
 * 
 * This function sets up USART2 on the AVR microcontroller with the following settings:
 * - TX pin (PF0) is set as output, and RX pin (PF1) is set as input.
 * - Baud rate is set to 9600 using a predefined macro.
 * - Transmission is enabled for USART2.
 */
void USART2_INIT(void);

/**
 * @brief Transmits a string over USART2
 * 
 * This function sends each character of the provided string via USART2, one by one.
 * 
 * @param str The string to be transmitted
 */
void USART2_PRINTF(char *str);

/**
 * @brief Transmits a single byte over USART2
 * 
 * This function transmits a single byte of data via USART2.
 * 
 * @param byte The byte to be transmitted
 */
void USART2_PRINT_BYTE(uint8_t byte);

/**
 * @brief Transmits a string from a volatile uint8_t pointer
 * 
 * This function transmits a string from a volatile uint8_t pointer, allowing
 * transmission of volatile data types. It waits for the USART data register
 * to be ready before sending each byte.
 * 
 * @param str Pointer to the string of volatile uint8_t data
 */
void USART2_PRINTF_INT(volatile uint8_t *str);

/**
 * @brief Transmits a string from a volatile unsigned char pointer
 * 
 * Similar to `USART2_PRINTF_INT`, this function transmits a string from a volatile 
 * unsigned char pointer, handling volatile data types in transmission.
 * 
 * @param str Pointer to the string of volatile unsigned char data
 */
void USART2_PRINTF_UCHAR(volatile unsigned char *str);

/**
 * @brief Transmits an unsigned integer over USART2
 * 
 * This function converts the unsigned integer value into a string and transmits
 * each character of the string via USART2.
 * 
 * @param value The unsigned integer to be transmitted
 */
void USART2_PRINTF_UINT(unsigned int value);

/**
 * @brief Transmits a formatted string over USART2
 * 
 * This function works like `printf` but sends the formatted string over USART2.
 * It uses variadic arguments for formatting the string.
 * 
 * @param format The format string, followed by the corresponding arguments
 */
void USART2_PRINTF_MOD(const char *format, ...);


#endif // PRINTF_H
