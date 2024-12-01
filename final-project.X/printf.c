/* Tutorial Based On:
 *      https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ApplicationNotes/ApplicationNotes/TB3216-Getting-Started-with-USART-DS90003216.pdf
 * 
 * MicroUSB USART Peripheral:
 *      + TX Pin: PF0
 *      + RX Pin: PF1 
 */

#define F_CPU 3333333 // Define the clock speed as 3.33 MHz
#include <avr/io.h>   // Include AVR I/O register definitions
#include <util/delay.h>  // Include delay functions
#include "printf.h"    // Include custom printf functionality
#include <stdio.h>  // Include standard I/O functions (for vsnprintf)
#include <stdarg.h> // Include standard macros for handling variadic functions (va_list, va_start, va_end)
#include <string.h> // Include string functions (e.g., strlen)


int main(void);
void USART2_INIT(void);
void USART2_PRINTF(char *str);

/**
 * @brief Initialize USART2 for communication
 * 
 * Configures USART2 for transmission using the MicroUSB pins (PF0 for TX, PF1 for RX)
 * and sets the baud rate to 9600.
 */
void USART2_INIT(void)
{
    /* Set TX pin (PF0) as output and RX pin (PF1) as input */
    PORTF.DIRSET = PIN0_bm;   // Set PF0 (TX) as output
    PORTF.DIRCLR = PIN1_bm;   // Set PF1 (RX) as input
    
    /* Set the BAUD rate for 9600 baud using a predefined macro from the tutorial */
    USART2.BAUD = (uint16_t)USART2_BAUD_VALUE(9600);
    
    /* Enable USART2 transmission */
    USART2.CTRLB |= USART_TXEN_bm;  // Enable transmitter
}

/**
 * @brief Transmit a string over USART2
 * 
 * @param str The string to be transmitted
 */
void USART2_PRINTF(char *str)
{
    for (size_t i = 0; i < strlen(str); i++)  // Loop through each character of the string
    {
        while (!(USART2.STATUS & USART_DREIF_bm));  // Wait for the data register to be ready
        USART2.TXDATAL = str[i];  // Send the character to the TX register
    }
}

/**
 * @brief Transmit a single byte over USART2
 * 
 * @param byte The byte to be transmitted
 */
void USART2_PRINT_BYTE(uint8_t byte) {
    // Wait until the data register is ready for new data
    while (!(USART2.STATUS & USART_DREIF_bm));      
    USART2.TXDATAL = byte; // Send the single byte
}

/**
 * @brief Transmit a string from a volatile uint8_t pointer
 * 
 * This version of the function is designed to handle volatile uint8_t arrays.
 * 
 * @param str Pointer to a volatile uint8_t array
 */
void USART2_PRINTF_INT(volatile uint8_t *str)
{
    for (size_t i = 0; i < strlen((char *)str); i++)  // Cast to char* for compatibility with strlen
    {
        while (!(USART2.STATUS & USART_DREIF_bm));  // Wait for the data register to be empty
        USART2.TXDATAL = str[i];  // Transmit the byte
    }
}

/**
 * @brief Transmit a string from a volatile unsigned char pointer
 * 
 * Similar to USART2_PRINTF_INT but specifically for volatile unsigned char arrays.
 * 
 * @param str Pointer to a volatile unsigned char array
 */
void USART2_PRINTF_UCHAR(volatile unsigned char *str)
{
    for (size_t i = 0; i < strlen((char *)str); i++)  // Cast to char* for compatibility with strlen
    {
        while (!(USART2.STATUS & USART_DREIF_bm));   // Wait for the data register to be empty
        USART2.TXDATAL = str[i];  // Send the character
    }
}

/**
 * @brief Transmit an unsigned integer over USART2
 * 
 * Converts the unsigned integer to a string and transmits it character by character.
 * 
 * @param value The unsigned integer value to be transmitted
 */
void USART2_PRINTF_UINT(unsigned int value)
{
    char buffer[12];  // Buffer to hold the string representation of the number
    snprintf(buffer, sizeof(buffer), "%u", value);  // Convert the unsigned int to a string

    for (size_t i = 0; i < strlen(buffer); i++)  // Loop through the string representation
    {
        while (!(USART2.STATUS & USART_DREIF_bm));  // Wait for the data register to be empty
        USART2.TXDATAL = buffer[i];  // Transmit the character
    }
}

/**
 * @brief Transmit a formatted string over USART2 using variadic arguments
 * 
 * This function works like printf, formatting the string with variable arguments before transmitting.
 * 
 * @param format The format string (similar to printf)
 */
void USART2_PRINTF_MOD(const char *format, ...) {
    char buffer[128];  // Buffer to hold the formatted string
    va_list args;  // Declare a variable argument list

    va_start(args, format);  // Start processing the variadic arguments
    vsnprintf(buffer, sizeof(buffer), format, args);  // Format the string
    va_end(args);  // End variadic argument processing

    // Send each character in the formatted string
    for (size_t i = 0; i < strlen(buffer); i++) {
        while (!(USART2.STATUS & USART_DREIF_bm));  // Wait for the data register to be empty
        USART2.TXDATAL = buffer[i];  // Transmit the character
    }
}
