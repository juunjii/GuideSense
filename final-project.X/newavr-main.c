#ifndef F_CPU
#define F_CPU 3333333
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#include <avr/cpufunc.h>
#include <avr/interrupt.h>
//#include <stdio.h>


#include "RTC_Operations.h"
#include "USART_init.h"
#include "Pulse.h"

#define BUF_SIZE 9
#define HEADER 0x59
#define DISTANCE_THRESHOLD 5
#define LEFT_MOTOR PIN4_bm
#define MIDDLE_MOTOR PIN5_bm
#define RIGHT_MOTOR PIN6_bm


volatile int secondCounter = 0;


#define PULSE_LEFT 0x01
#define PULSE_MIDDLE 0x02
#define PULSE_RIGHT 0x04
#define PULSE_CLOSER 0x08
#define PULSE_FURTHER 0x10
#define STATE_5 0x20
#define STATE_6 0x40
#define STATE_7 0x80
volatile uint8_t statesActive = 0;
volatile uint8_t pulseCounter = 0;
//
//#define RTC_PERIOD 16383



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

// Read data from LIDAR sensor following its protocol
// Returns 1 if valid data received, 0 otherwise
uint8_t readLidarData(uint16_t *distance) {
    char data[BUF_SIZE];
    uint8_t check;
    
    // Wait for first header byte (0x59)
    data[0] = usartReadChar();
    if (data[0] != HEADER) {
        return 0;
    }
    
    // Wait for second header byte (0x59)
    data[1] = usartReadChar();
    if (data[1] != HEADER) {
        return 0;
    }
    
    // Read remaining 7 bytes
    for (int i = 2; i < BUF_SIZE; i++) {
        data[i] = usartReadChar();
    }
    
    // Calculate checksum (sum of first 8 bytes)
    check = 0;
    for (int i = 0; i < 8; i++) {
        check += data[i];
    }
    
    // Verify checksum
    if (data[8] != (check & 0xFF)) {
        return 0;
    }
    
    // Extract distance value (bytes 2-3, little endian)
    *distance = data[2] + data[3] * 256;
    return 1;
}




void initMotors(){
    PORTA.DIR |= LEFT_MOTOR; //left
    PORTA.DIR |= MIDDLE_MOTOR; //middle
    PORTA.DIR |= RIGHT_MOTOR; //right
}

void clearMotors(){
    PORTA.OUT &= ~LEFT_MOTOR;
    PORTA.OUT &= ~MIDDLE_MOTOR;
    PORTA.OUT &= ~RIGHT_MOTOR;
}

void pulseLeft(){
    if (pulseCounter < 3){
            if (secondCounter == 1){
                PORTA.OUT &= ~LEFT_MOTOR;
                PORTA.OUT |= MIDDLE_MOTOR;
                PORTA.OUT &= ~RIGHT_MOTOR;
            } else if (secondCounter == 2){
                PORTA.OUT |= LEFT_MOTOR;
                PORTA.OUT &= ~MIDDLE_MOTOR;
                PORTA.OUT &= ~RIGHT_MOTOR;
            }
//            pulseCounter++;
        } else {
            pulseCounter = 0;
            statesActive &= ~PULSE_LEFT;
            clearMotors();
        }
}

void pulseMiddle(){
    if (pulseCounter < 3){
            if (secondCounter == 1){
                PORTA.OUT &= ~LEFT_MOTOR;
                PORTA.OUT |= MIDDLE_MOTOR;
                PORTA.OUT &= ~RIGHT_MOTOR;
            } else if (secondCounter == 2){
                PORTA.OUT &= ~LEFT_MOTOR;
                PORTA.OUT &= ~MIDDLE_MOTOR;
                PORTA.OUT &= ~RIGHT_MOTOR;
//                pulseCounter++;
            }
            
        } else {
            pulseCounter = 0;
            statesActive &= ~PULSE_LEFT;
            clearMotors();
        }
}

void pulseRight(){
    if (pulseCounter < 3){
            if (secondCounter == 1){
                PORTA.OUT &= ~LEFT_MOTOR;
                PORTA.OUT |= MIDDLE_MOTOR;
                PORTA.OUT &= ~RIGHT_MOTOR;
            } else if (secondCounter == 2){
                PORTA.OUT &= ~LEFT_MOTOR;
                PORTA.OUT &= ~MIDDLE_MOTOR;
                PORTA.OUT |= RIGHT_MOTOR;
//                pulseCounter++;
            }
            
        } else {
            pulseCounter = 0;
            statesActive &= ~PULSE_LEFT;
            clearMotors();
        }
}

void pulseCloser(){
    if (pulseCounter < 3){
        PORTA.OUT |= LEFT_MOTOR;
        PORTA.OUT &= ~MIDDLE_MOTOR;
        PORTA.OUT |= RIGHT_MOTOR;

    } else if (pulseCounter == 3){
        PORTA.OUT &= ~LEFT_MOTOR;
        PORTA.OUT |= MIDDLE_MOTOR;
        PORTA.OUT &= ~RIGHT_MOTOR;
            
    }else {
        pulseCounter = 0;
            statesActive &= ~PULSE_LEFT;
            clearMotors();
        }
}

void pulseFurther(){
    if (pulseCounter < 3){
        PORTA.OUT &= ~LEFT_MOTOR;
        PORTA.OUT |= MIDDLE_MOTOR;
        PORTA.OUT &= ~RIGHT_MOTOR;

    } else if (pulseCounter == 3){
        PORTA.OUT |= LEFT_MOTOR;
        PORTA.OUT &= ~MIDDLE_MOTOR;
        PORTA.OUT |= RIGHT_MOTOR; 
    }else {
        pulseCounter = 0;
        statesActive &= ~PULSE_LEFT;
        clearMotors();
    }
}


ISR(RTC_CNT_vect) {

    secondCounter++;
    if (statesActive & PULSE_LEFT){
        pulseLeft();
        
    }
    
    if (statesActive & PULSE_CLOSER){
        pulseCloser();
        
    }
    
    if (statesActive & PULSE_FURTHER){
        pulseFurther();
        
    }
    if (secondCounter == 2){
        secondCounter = 0;
    }
    pulseCounter++;
    if (pulseCounter > 6){
        pulseCounter = 0;
    }
    RTC.INTFLAGS = RTC_OVF_bm;
    
}
int main() {
    uint16_t distance;
    uint16_t prev_distance;
    
    // Initialize UART
    usartInit();
    
    // Configure LED pin (PA4) as output
    PORTA.DIR |= PIN4_bm;
    PORTA.DIR |= PIN5_bm;
    PORTA.DIR |= PIN6_bm;
    RTC_init();
    sei();
    while (1) {
        
//        PORTA.OUT |= LEFT_MOTOR;
        // Try to read valid LIDAR data
        if (readLidarData(&distance)) {
            // Update LED based on distance threshold
            if (distance > DISTANCE_THRESHOLD) {
                if (distance > prev_distance){
                    statesActive |= PULSE_FURTHER;
                    statesActive &= ~PULSE_CLOSER;
                } else if (distance < prev_distance) {
                    statesActive |= PULSE_CLOSER;
                    statesActive &= ~PULSE_FURTHER;
                }

                 
            } else {
                statesActive &= ~PULSE_CLOSER;
                statesActive &= ~PULSE_FURTHER;
                clearMotors();
                pulseCounter = 0;

            }
            
            prev_distance = distance;
        }
        
    }
    
    return 0;
}