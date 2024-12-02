/*
 * File:   lidar.c
 * Author: chehj
 *
 * Created on December 1, 2024, 8:50 PM
 */

#include "lidar.h"


// Read data from LIDAR sensor following its protocol
// Returns 1 if valid data received, 0 otherwise
uint8_t readLidarData(uint16_t *distance) {
    
    char data[9];
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
    for (int i = 2; i < 9; i++) {
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


