/*
 * File:   lidar.h
 * Author: chehj
 *
 * Created on December 1, 2024, 8:50 PM
 */

#ifndef LIDAR_H
#define LIDAR_H

#include <stdint.h>   // For uint8_t, uint16_t
#include "usart.h"    // For usartReadChar()
#include <avr/io.h>

// Constants
#define HEADER 0x59       // LIDAR header byte
#define BUF_SIZE 9        // Buffer size for LIDAR packet

// Function Prototypes
/**
 * @brief Reads data from the LIDAR sensor following its protocol.
 *
 * @param[out] distance Pointer to store the distance value read from the sensor.
 * @return 1 if valid data is received, 0 otherwise.
 */
uint8_t readLidarData(uint16_t *distance);

#endif /* LIDAR_H */
