/*
 * File:   i2c.c
 * Author: chehj
 *
 * Created on November 13, 2024, 10:29 PM
 */

#include "i2c.h"
#include "printf.h"
#define TWI_LASTCMD_bm (1 << 7)

// PD 1 - yellow
// PD 6 - green 
// PC 1 - red 

void TWI_initPins() {
    // Set TWI pins for SDA and SCL
    PORTA.DIRCLR = PIN2_bm | PIN3_bm;  // Set as inputs (open-drain)
    
    // debugging 
    PORTD.DIRSET = PIN6_bm; // red
    PORTC.DIRSET = PIN1_bm; // green
    
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm; // Enable pull-up for SDA
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm; // Enable pull-up for SCL
    
    
   
}


void TWI_init() {
    
    TWI_initPins();
    
    // Set hold time
    TWI0.CTRLA = TWI_SDAHOLD_50NS_gc;  
    
    // Enable run in debug mode
    TWI0.DBGCTRL = TWI_DBGRUN_bm;  
    
    // Set bus to idle state
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;  
    
    // Set baud rate for 100kHz from a 4MHz oscillator
    TWI0.MBAUD = 10;  
    
    // Enable TWI
    TWI0.MCTRLA = TWI_ENABLE_bm; 
    
}

void print_MSTATUS_bits(uint8_t mstatus) {
    printf("MSTATUS Register: 0x%02X\n", mstatus);

    // Bit 7: RIF - Read Interrupt Flag
    if (mstatus & (1 << 7)) {
        printf("RIF (Read Interrupt Flag): SET (Host byte read operation completed)\n");
    } else {
        printf("RIF (Read Interrupt Flag): CLEAR\n");
    }

    // Bit 6: WIF - Write Interrupt Flag
    if (mstatus & (1 << 6)) {
        printf("WIF (Write Interrupt Flag): SET (Host transmit address/byte write operation completed)\n");
    } else {
        printf("WIF (Write Interrupt Flag): CLEAR\n");
    }

    // Bit 5: CLKHOLD - Clock Hold
    if (mstatus & (1 << 5)) {
        printf("CLKHOLD (Clock Hold): SET (Host holding SCL low, clock stretching)\n");
    } else {
        printf("CLKHOLD (Clock Hold): CLEAR\n");
    }

    // Bit 4: RXACK - Received Acknowledge
    if (mstatus & (1 << 4)) {
        printf("RXACK (Received Acknowledge): NACK received (Client not ready for more data)\n");
    } else {
        printf("RXACK (Received Acknowledge): ACK received (Client ready for more data)\n");
    }

    // Bit 3: ARBLOST - Arbitration Lost
    if (mstatus & (1 << 3)) {
        printf("ARBLOST (Arbitration Lost): SET (Host lost arbitration)\n");
    } else {
        printf("ARBLOST (Arbitration Lost): CLEAR\n");
    }

    // Bit 2: BUSERR - Bus Error
    if (mstatus & (1 << 2)) {
        printf("BUSERR (Bus Error): SET (Illegal bus operation detected)\n");
    } else {
        printf("BUSERR (Bus Error): CLEAR\n");
    }

    // Bits 1:0: BUSSTATE[1:0] - Bus State
    uint8_t bus_state = mstatus & 0x03; // Mask to get bits 1:0
    switch (bus_state) {
        case 0:
            printf("BUSSTATE: Bus is idle\n");
            break;
        case 1:
            printf("BUSSTATE: Address phase\n");
            break;
        case 2:
            printf("BUSSTATE: Data phase\n");
            break;
        case 3:
            printf("BUSSTATE: Stop condition\n");
            break;
        default:
            printf("BUSSTATE: Unknown state\n");
            break;
    }
}


void TWI_write(uint8_t address, uint8_t* data, uint8_t length) {
    // Start a write transaction
    TWI0.MADDR = (address << 1) | 0;
    while (!(TWI0.MSTATUS & TWI_WIF_bm)) {
        if (TWI0.MSTATUS & TWI_RXACK_bm) {
            USART2_PRINTF("Slave NACK\r\n");
            TWI_endWrite();
            return;
        }
    }
    
    USART2_PRINTF("Slave ACK received, starting data write...\r\n");
    
    // Write data bytes
    for (uint8_t i = 0; i < length; i++) {
        TWI0.MDATA = data[i];
        while (!(TWI0.MSTATUS & TWI_WIF_bm)) {
            if (TWI0.MSTATUS & TWI_RXACK_bm) {
                USART2_PRINTF("Data NACK\r\n");
                TWI_endWrite();
                return;
            }
        }
        // Print the successfully written byte
        USART2_PRINTF_MOD("Data[%d] written: 0x%02X (decimal: %d)\r\n", i, data[i], data[i]);
    }

    TWI_endWrite();
    USART2_PRINTF("Write complete\r\n");
}


void TWI_endWrite() {
    // Send NACK and stop condition to end the transaction
    TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;    
}

void TWI_startRead(uint8_t address) {    
    // Send the slave address with the write bit    
    TWI0.MADDR = (address << 1) | 1;
   
    // Wait for the read interrupt flag
    while (!(TWI0.MSTATUS & TWI_RIF_bm)); 
   
}

void readFromTWI(volatile uint8_t* data, uint8_t len) {
    TWI_startRead(GPS_ADDRESS); // GPS_ADDRESS should be defined elsewhere
    USART2_PRINTF_MOD("Reading %d bytes from TWI...\r\n", len);

    for (uint8_t bCount = 0; bCount < len; bCount++) {
        // Wait for the read flag (data ready to be read)
        while (!(TWI0.MSTATUS & TWI_RIF_bm));
       
        // Read the received byte
        data[bCount] = TWI0.MDATA;

        // Debugging: print the received byte in hex and decimal
//        USART2_PRINTF_MOD("Data[%d]: 0x%02X (decimal: %d)\r\n", bCount, data[bCount], data[bCount]);

        // If more bytes to read, send ACK; otherwise, prepare to send NACK
        if (bCount < (len - 1)) {
            TWI0.MCTRLB = TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc; // Send ACK
        } else {
            TWI_endRead(); // Send NACK and stop after the last byte
        }
    }
}


void TWI_endRead() {
    // Send NACK and stop condition to end the transaction
    TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;    
}

