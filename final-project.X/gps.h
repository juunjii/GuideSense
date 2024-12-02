/* 
 * File:   gps_interrupt.h
 * Author: chehj
 *
 * Created on December 1, 2024, 11:35 AM
 */

#ifndef GPS_H
#define	GPS_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>

#include <string.h>

#include <stdlib.h>
#include <string.h>
#include <math.h> // For fabs()

#include "i2c.h"


// Constants
#define GPS_ADDRESS 0x10 
#define MAX_PACKET_SIZE 255
#define SCALE_FACTOR 1000000

#define PULSE_ARRIVED 0x20
extern volatile uint8_t statesActive;


// Global buffer and indices for GPS data handling
extern volatile bool gps_data_ready;
extern volatile uint8_t gpsData[MAX_PACKET_SIZE];  // Buffer to store GPS data
extern volatile uint8_t _head;                      // Head pointer for writing data to gpsData
extern volatile uint8_t _tail;                      // Tail pointer for reading data from gpsData
extern char gps_buffer[MAX_PACKET_SIZE];            // Buffer to store the GPS sentence
extern uint16_t buffer_index;                       // Current position in the buffer




/**
 * @brief Initializes GPS and required peripherals (I2C, UART, RTC).
 */
void GPS_init(void);

/**
 * @brief Converts NMEA coordinates to decimal degrees.
 * 
 * @param coord NMEA coordinate string (e.g., "12345.678").
 * @param direction Direction character ('N', 'S', 'E', or 'W').
 * @return Coordinate in decimal degrees.
 */
double convert_to_decimal(const char *coord, char direction);

/**
 * @brief Converts a time string from NMEA format (hhmmss.sss) to 24-hour format.
 * 
 * @param time_str NMEA time string.
 * @return Time string in 24-hour format.
 */
char* convert_to_24hr_format(const char *time_str);

/**
 * @brief Checks if the user has reached the destination.
 * 
 * @param curr_lat Current latitude in decimal degrees.
 * @param curr_lon Current longitude in decimal degrees.
 */
void check_arrival(double curr_lat, double curr_lon);

/**
 * @brief Parses a GPGGA sentence from the GPS data.
 * 
 * @param sentence GPGGA sentence string.
 */
void parse_gngga(const char *sentence);

/**
 * @brief Parses incoming GPS data and processes sentences.
 */
void parse_gps_data(void);


#endif	/* GPS_H */

