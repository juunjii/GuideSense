/* 
 * File:   i2c.h
 * Author: chehj
 *
 * Created on November 26, 2024, 7:31 PM
 */

#ifndef I2C_H
#define	I2C_H

#define MAX_PACKET_SIZE 255
#define GPS_ADDRESS 0x10 

// Debugging macros
#define _DEBUG_RED() (PORTD.OUT |= PIN6_bm)
#define _DEBUG_GREEN() (PORTC.OUT |= PIN1_bm)
#define _PRINT_MSTATUS() (USART2_PRINTF_MOD("MSTATUS: 0x%02X\n", TWI0.MSTATUS))
#define _PRINT_RECEIVED_BYTES() (USART2_PRINTF_MOD("Data[%d]: 0x%02X (decimal: %d)\r\n", bCount, data[bCount], data[bCount]))
#define F_CPU 3333333

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h> 




/**
 * @brief Initialize the I2C (TWI) pins for SDA and SCL.
 * This sets up the pins as inputs and enables the pull-up resistors.
 */
void TWI_initPins(void);

/**
 * @brief Initialize the I2C (TWI) interface.
 * This configures the baud rate, holds time, debug control, and enables the TWI interface.
 */
void TWI_init(void);

/**
 * @brief Start a read operation from the specified slave address.
 * This sends the slave address with the read bit set (address << 1 | 1).
 * 
 * @param address The I2C slave address to read from.
 */
void TWI_startRead(uint8_t address);

/**
 * @brief Read data from the I2C bus.
 * This reads a specific number of bytes from the I2C bus.
 * 
 * @param data Pointer to the buffer where the received data will be stored.
 * @param len The number of bytes to read.
 * @return The number of bytes successfully read.
 */
uint8_t readFromTWI(volatile uint8_t* data, uint8_t len);

/**
 * @brief Request a specific number of bytes from the GPS (or other I2C device).
 * This function reads the requested number of bytes and stores them in the buffer.
 * 
 * @param quantity The number of bytes to request from the GPS.
 * @return The number of bytes successfully read.
 */
uint8_t requestByte(uint8_t quantity);

/**
 * @brief Read one byte at a time from the internal buffer.
 * This function returns each successive byte from the buffer on each call.
 * 
 * @return The next byte in the buffer, or -1 if no more bytes are available.
 */
uint8_t read(void);

/**
 * @brief End the read operation and send a NACK with the stop condition.
 * This function sends the NACK signal for the last byte and stops the communication.
 */
void TWI_endRead(void);

// Global Variables

/**
 * @brief Circular buffer to store received data.
 * This buffer temporarily holds the data received from I2C.
 */
extern uint8_t rxBuffer[32];

/**
 * @brief Index for the next byte to read from the rxBuffer.
 */
extern uint8_t rxBufferIndex;

/**
 * @brief Length of the received data in the rxBuffer.
 */
extern uint8_t rxBufferLength;

#endif	/* I2C_H */

