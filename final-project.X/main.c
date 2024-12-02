
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include "RTC_Operations.h"
#include "motor.h"
#include "lidar.h"
#include "gps.h"
#include <util/delay.h>
#define DISTANCE_THRESHOLD 5

volatile int secondCounter = 0;

#define PULSE_LEFT 0x01
#define PULSE_MIDDLE 0x02
#define PULSE_RIGHT 0x04
#define PULSE_CLOSER 0x08
#define PULSE_FURTHER 0x10
#define PULSE_ARRIVED 0x20

volatile uint8_t statesActive = 0;
volatile uint8_t pulseCounter = 0;
volatile bool gps_data_ready = false; // Flag to indicate new GPS data is available

ISR(RTC_CNT_vect) {
    secondCounter++;
    if (statesActive & PULSE_LEFT){
        pulseLeft();
        //USART2_PRINTF_MOD("PULSING LEFT\r\n");
        
    }
    
    if (statesActive & PULSE_CLOSER){
        pulseCloser();
        //USART2_PRINTF_MOD("PULSING RIGHT\r\n");
        
    }
    
    if (statesActive & PULSE_FURTHER){
        pulseFurther();
        //USART2_PRINTF_MOD("PULSING FURTHER\r\n");
        
    }
    
    if (statesActive & PULSE_ARRIVED){
        pulseArrived();
        //USART2_PRINTF_MOD("PULSING ARRIVED\r\n");
    }
    if (secondCounter == 2){
        secondCounter = 0;
        pulseCounter++;
    }
    
    
    if (pulseCounter > 6){
        pulseCounter = 0;
    }
    
    for (uint8_t x = 0; x < MAX_PACKET_SIZE; x++) {
        if (x % 32 == 0) {
            requestByte(32); // Request 32 bytes from the GPS module
        }
        uint8_t incoming = read(); // Read incoming byte from GPS

        if (incoming != 0x0A) {  // Ignore line breaks
            gpsData[_head++] = incoming; // Store the incoming byte in the gpsData buffer
            _head %= MAX_PACKET_SIZE;  // Wrap head pointer if it exceeds MAX_PACKET_SIZE
        }
    }
    
    gps_data_ready = true; // Set flag to indicate new data is available
    RTC.INTFLAGS = RTC_OVF_bm;
    
}
int main() {
    uint16_t distance;
    uint16_t prev_distance;
    
    // Initialize UART
    usartInit();
    GPS_init(); // Initialize GPS and peripherals
    RTC_init();

    _delay_ms(5000);
    
    // Configure LED pin (PA4) as output
    PORTA.DIR |= PIN4_bm;
    PORTA.DIR |= PIN5_bm;
    PORTA.DIR |= PIN6_bm;
    
    sei();
    
    while (1) {
        
        // Try to read valid LIDAR data
        if (statesActive & PULSE_ARRIVED)
        {
            
        } else {
            if (readLidarData(&distance)) {
            // Update LED based on distance threshold
            if (distance > DISTANCE_THRESHOLD) {
                USART2_PRINTF_MOD("Distance: %.8f\r\n", distance);
                if (distance > prev_distance){
                    //USART2_PRINTF_MOD("Getting FURTHER TO AN OBJECT\r\n");
                    statesActive |= PULSE_FURTHER;
                    statesActive &= ~PULSE_CLOSER;
                } else if (distance < prev_distance) {
                    //USART2_PRINTF_MOD("Getting CLOSER from an Object\r\n");
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
            parse_gps_data(); // Parse GPS sentences in the main loop
        }
        
        
    }
    
    return 0;
}