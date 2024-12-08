/*
 * File:   i2c.c
 * Author: chehj
 *
 * Created on November 13, 2024, 10:29 PM
 */

#include "i2c.h"    // Include the header file for I2C functions and definitions
#include "printf.h" // Include the header for printf functionality


// Circular buffer to store received data from I2C
uint8_t rxBuffer[32];          // Buffer to hold up to 32 bytes
uint8_t rxBufferIndex = 0;      // Index to track the next byte to read from the buffer
uint8_t rxBufferLength = 0;     // Length of data currently stored in the buffer


/**
 * @brief Initialize the I2C (TWI) pins for SDA and SCL.
 * This sets the SDA and SCL pins as inputs and enables pull-up resistors.
 */
void TWI_initPins() {
    // Set pins for SDA and SCL as input (open-drain)
    PORTA.DIRCLR = PIN2_bm | PIN3_bm;  // Set pins 2 and 3 as inputs

    // Enable pull-up resistors for SDA and SCL
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm; // Pull-up for SDA
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm; // Pull-up for SCL
}


/**
 * @brief Initialize the TWI (I2C) interface.
 * This function configures the baud rate, hold time, debug mode, and bus state.
 */
void TWI_init() {
    
    // Initialize the TWI pins
    TWI_initPins();
    
    // Set the SDA hold time to 50ns
    TWI0.CTRLA = TWI_SDAHOLD_50NS_gc;  
    
    // Enable the debug mode for TWI (I2C) interface
    TWI0.DBGCTRL = TWI_DBGRUN_bm;  
    
    // Set the TWI bus to idle state
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;  
    
    // Set the baud rate for 100kHz using a 4MHz system clock (MBAUD = 10)
    TWI0.MBAUD = 10;  
    
    // Enable the TWI interface
    TWI0.MCTRLA = TWI_ENABLE_bm; 
}


/**
 * @brief Start a read operation on the TWI bus.
 * This function sends the slave address with the read bit set.
 * 
 * @param address The I2C slave address to read from.
 */
void TWI_startRead(uint8_t address) {    
    // Send the slave address with the read bit (1) set
    TWI0.MADDR = (address << 1) | 1;
   
    // Wait for the read interrupt flag, indicating that data is ready
    while (!(TWI0.MSTATUS & TWI_RIF_bm)); 
}


/**
 * @brief Read a specified number of bytes from the TWI bus.
 * 
 * @param data Pointer to a buffer where the received data will be stored.
 * @param len The number of bytes to read from the bus.
 * @return The number of bytes successfully read.
 */
uint8_t readFromTWI(volatile uint8_t* data, uint8_t len) {
    // Start the read operation for the given slave address
    TWI_startRead(GPS_ADDRESS);

    uint8_t bytesRead = 0;
  
    // Loop to read the specified number of bytes
    for (uint8_t bCount = 0; bCount < len; bCount++) {

        // Wait for the read flag (data is ready to be read)
        while (!(TWI0.MSTATUS & TWI_RIF_bm));
      
        // Read the received byte from the data register
        data[bCount] = TWI0.MDATA;
        bytesRead++;
        
        // If more bytes need to be read, send ACK (acknowledgment)
        if (bCount < (len - 1)) {
            TWI0.MCTRLB = TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc; // Send ACK for more data
        } else {
            // For the last byte, send NACK and end the read
            TWI_endRead();
        }
    }
     
    return bytesRead; // Return the number of bytes read
}


/**
 * @brief Request a specific number of bytes from the GPS (or other device).
 * This function stores the received bytes in the global rxBuffer.
 * 
 * @param quantity The number of bytes to request from the device.
 * @return The number of bytes successfully read.
 */
uint8_t requestByte(uint8_t quantity) {
    // Read the specified number of bytes and store in rxBuffer
    uint8_t read = readFromTWI(rxBuffer, quantity);
    
    // Reset the buffer index and update the buffer length
    rxBufferIndex = 0;
    rxBufferLength = read;

    return read; // Return the number of bytes read
}


/**
 * @brief Read one byte at a time from the rxBuffer.
 * This function returns each successive byte stored in the buffer.
 * 
 * @return The next byte in the buffer, or -1 if there are no more bytes to read.
 */
uint8_t read() {
    int value = -1;
  
    // If there are bytes left in the buffer, return the next byte
    if(rxBufferIndex < rxBufferLength) {
        value = rxBuffer[rxBufferIndex];
        ++rxBufferIndex; // Increment the buffer index for the next read
    }
 
    return value; // Return the byte, or -1 if no more bytes
}


/**
 * @brief End the read operation and send a NACK with the stop condition.
 * This function signals the end of the read transaction on the TWI bus.
 */
void TWI_endRead() {
    // Send NACK and stop condition to end the transaction
    TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;    
}
