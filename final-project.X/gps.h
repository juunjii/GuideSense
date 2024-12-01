/* 
 * File:   gps.h
 * Author: chehj
 *
 * Created on November 26, 2024, 7:31 PM
 */

#ifndef GPS_H
#define	GPS_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>

#include <string.h>
#include "i2c.h"
#include <stdlib.h>
#include <string.h>
#include <math.h> // For fabs()
#include "mini-gmp.h"  // Include the GMP library



#define GPS_ADDRESS 0x10 
#define MAX_PACKET_SIZE 255
#define SCALE_FACTOR 1000000

#ifndef DEBUG
#define DEBUG 1  // Set to 1 to enable debugging, 0 to disable
#endif

// Debugging macros for USART2_PRINTF_MOD lines
#if DEBUG
    #define DEBUG_PRINT(msg)         USART2_PRINTF_MOD(msg)
    #define DEBUG_PRINT_INT(val)     USART2_PRINTF_MOD("%d", (val))
    #define DEBUG_PRINT_DOUBLE(val)  USART2_PRINTF_MOD("%f", (val))
    #define DEBUG_PRINT_STRING(str)  USART2_PRINTF_MOD("%s", (str))
#else
    #define DEBUG_PRINT(msg)
    #define DEBUG_PRINT_INT(val)
    #define DEBUG_PRINT_DOUBLE(val)
    #define DEBUG_PRINT_STRING(str)
#endif

// Global buffer and indices for GPS data handling
extern volatile uint8_t gpsData[MAX_PACKET_SIZE];  // Buffer to store GPS data
extern volatile uint8_t _head;                      // Head pointer for writing data to gpsData
extern volatile uint8_t _tail;                      // Tail pointer for reading data from gpsData
extern char gps_buffer[MAX_PACKET_SIZE];            // Buffer to store the GPS sentence
extern uint16_t buffer_index;                       // Current position in the buffer


/**
 * @brief Initializes the GPS module.
 * This function resets the GPS data buffer.
 */
void GPS_begin(void);

/**
 * @brief Checks the GPS module for new data.
 * This function reads bytes from the GPS module via I2C and stores them in the gpsData buffer.
 */
void GPS_check(void);

/**
 * @brief Returns the number of bytes available in the GPS buffer.
 * This function checks the GPS module if new data is available.
 * 
 * @return uint8_t Number of available bytes.
 */
uint8_t GPS_available(void);

/**
 * @brief Reads the next byte from the GPS data buffer.
 * This function retrieves one byte at a time from the buffer.
 * 
 * @return uint8_t The next available byte of GPS data, or 0 if no data is available.
 */
uint8_t GPS_read(void);

/**
 * @brief Validates the checksum of a GPS NMEA sentence.
 * This function calculates the checksum and compares it with the provided checksum in the sentence.
 * 
 * @param sentence The GPS NMEA sentence to validate.
 * @return bool True if the checksum is valid, otherwise false.
 */
bool validate_checksum(const char *sentence);

/**
 * @brief Converts GPS NMEA coordinates to decimal degrees.
 * This function takes a GPS coordinate string and direction (N/S/E/W) and converts it into decimal degrees.
 * 
 * @param coord The GPS coordinate string (e.g., "12345.678").
 * @param direction The direction ('N', 'S', 'E', 'W').
 * @return double The converted coordinate in decimal degrees.
 */
double convert_to_decimal(const char *coord, char direction);

/**
 * @brief Converts GPS time to 24-hour format.
 * This function takes a GPS time string and formats it to a 24-hour clock format.
 * 
 * @param time_str The GPS time string (e.g., "123045.678").
 * @return char* A string with the time in 24-hour format (e.g., "12:30:45.678").
 */
char* convert_to_24hr_format(const char *time_str);

/**
 * @brief Checks if the GPS has arrived at the destination.
 * This function compares the current position to the destination and checks if they are within a threshold.
 * 
 * @param curr_lat The current latitude.
 * @param curr_lon The current longitude.
 */
void check_arrival(double curr_lat, double curr_lon);

/**
 * @brief Parses a GNGGA NMEA sentence.
 * This function extracts the time, latitude, and longitude from a GNGGA sentence and processes the data.
 * 
 * @param sentence The GNGGA NMEA sentence.
 */
void parse_gngga(const char *sentence);

/**
 * @brief Main loop function to handle GPS data.
 * This function continuously checks for available GPS data, parses it, and processes it.
 */
void loop(void);

#endif	/* GPS_H */

