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
#define GPS_THRESHOLD 25
#define MAX_PACKET_SIZE 255
#define SCALE_FACTOR 1000000
#define PULSE_LEFT    0x01
#define PULSE_MIDDLE  0x02
#define PULSE_RIGHT   0x04
#define PULSE_CLOSER  0x08
#define PULSE_FURTHER 0x10
#define PULSE_ARRIVED 0x20
#define PULSE_DEST_FARTHER 0x40
#define PULSE_DEST_CLOSER 0x80
#define EARTH_RADIUS 6371000 // Earth's radius in meters

#define RED() PORTD.OUT |= PIN7_bm
#define YELLOW() PORTD.OUT |= PIN5_bm
#define GREEN() PORTA.OUT |= PIN7_bm

// Global buffer and indices for GPS data handling
extern volatile bool gps_data_ready;
extern volatile uint8_t gpsData[MAX_PACKET_SIZE];  // Buffer to store GPS data
extern volatile uint8_t _head;                      // Head pointer for writing data to gpsData
extern volatile uint8_t _tail;                      // Tail pointer for reading data from gpsData
extern char gps_buffer[MAX_PACKET_SIZE];            // Buffer to store the GPS sentence
extern uint16_t buffer_index;                       // Current position in the buffer
extern volatile uint8_t statesActive;



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


/**
 * @brief Converts an angle in degrees to radians.
 *
 * This function takes an angle in degrees and converts it to radians
 * using the formula: radians = degrees * ? / 180.
 *
 * @param degrees The angle in degrees.
 * @return The angle in radians.
 */
double degrees_to_radians(double degrees);

/**
 * @brief Calculates the great-circle distance between two GPS coordinates.
 *
 * This function computes the shortest distance between two points on the 
 * Earth's surface using the Haversine formula. Coordinates are provided 
 * as latitude and longitude in degrees.
 *
 * @param lat1 Latitude of the first point in degrees.
 * @param lon1 Longitude of the first point in degrees.
 * @param lat2 Latitude of the second point in degrees.
 * @param lon2 Longitude of the second point in degrees.
 * @return The distance in meters between the two points.
 */
double calc_distance(double lat1, double lon1, double lat2, double lon2);


#endif	/* GPS_H */

