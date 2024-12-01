///*
// * File:   gps.c
// * Author: chehj
// *
// * Created on November 13, 2024, 11:30 PM
// */
//
//#include "gps.h"
//#include "i2c.h"
//#include "printf.h"
//#include <stdio.h>  
//#include <math.h>  
//#include <stdlib.h>  
//
//
//// Global buffer and index variables
//volatile uint8_t gpsData[MAX_PACKET_SIZE];  // Buffer to store GPS data
//volatile uint8_t _head = 0;                 // Head pointer to track writing position in gpsData
//volatile uint8_t _tail = 0;                 // Tail pointer to track reading position in gpsData
//char gps_buffer[MAX_PACKET_SIZE];           // Buffer to store the GPS sentence
//uint16_t buffer_index = 0;                  // Current position in the buffer
//
///**
// * @brief Resets the head and tail pointers for GPS data processing.
// * This function initializes the buffers and prepares the GPS system for data collection.
// */
//void GPS_begin() {
//    _head = 0;
//    _tail = 0;
//}
//
///**
// * @brief Checks and reads data from the GPS module.
// * It fetches 32 bytes of data from the GPS module at a time and stores them in the gpsData buffer.
// */
//void GPS_check() {
////    USART2_PRINTF("Checking for new data!!....\n");
//    for (uint8_t x = 0; x < MAX_PACKET_SIZE; x++) {
//        if (x % 32 == 0) {
//            requestByte(32); // Request 32 bytes from the GPS module
//        }
//        uint8_t incoming = read(); // Read incoming byte from GPS
//
//        if (incoming != 0x0A) {  // Ignore line breaks
//            gpsData[_head++] = incoming; // Store the incoming byte in the gpsData buffer
//            _head %= MAX_PACKET_SIZE;  // Wrap head pointer if it exceeds MAX_PACKET_SIZE
//        }
//    }
////    USART2_PRINTF("Exit GPS_check!!!!!!");
//}
//
//
///**
// * @brief Returns the number of available bytes that can be read from the gpsData buffer.
// * 
// * @return The number of bytes available for reading.
// */
//uint8_t GPS_available() {
//    if (_tail == _head) {  // If tail equals head, there is no new data
////        USART2_PRINTF("Buffer is empty....\n");
//        GPS_check(); // Check for new data from the GPS module
//    }
//
//    // Return the number of new bytes available
//    if (_head > _tail) return (_head - _tail);
//    if (_tail > _head) return (MAX_PACKET_SIZE - _tail + _head);
//
//    return 0; // No data available
//}
//
///**
// * @brief Reads the next available byte from the gpsData buffer.
// * 
// * @return The next byte in the buffer, or 0 if no byte is available.
// */
//uint8_t GPS_read() {
////    USART2_PRINTF("Return the next available byte in gps array...\n");
//
//    if (_tail != _head) {  // If there's data available
//        uint8_t datum = gpsData[_tail++];  // Get the byte at the tail
//        if (_tail == MAX_PACKET_SIZE) _tail = 0; // Wrap the tail pointer
//        return datum;
//    }
//    return 0; // No new data available
//}
//
//
//
///**
// * @brief Validates the checksum of a GPS NMEA sentence.
// * 
// * @param sentence The NMEA sentence to validate.
// * @return true if the checksum is valid, false otherwise.
// */
//bool validate_checksum(const char *sentence) {
//    const char *checksum_ptr = strchr(sentence, '*');  // Find the checksum delimiter '*'
//    if (checksum_ptr == NULL) {
//        DEBUG_PRINT("No checksum found.\n");
//        return false;
//    }
//
//    unsigned char checksum = 0;
//    for (const char *ptr = sentence + 1; ptr < checksum_ptr; ptr++) {
//        checksum ^= *ptr;  // XOR each character in the sentence to compute the checksum
//    }
//
//    unsigned int provided_checksum;
//    if (sscanf(checksum_ptr + 1, "%2X", &provided_checksum) != 1) {
//        DEBUG_PRINT("Checksum extraction failed.\n");
//        return false;
//    }
//    
//     // Check the sizes of the variables
//    USART2_PRINTF_MOD("Size of checksum: %zu bytes\n", sizeof(checksum));  // Expected output: 1 byte
//    USART2_PRINTF_MOD("Size of provided_checksum_char: %zu bytes\n", sizeof(provided_checksum));  // Expected output: 1 byte
//
//    unsigned char provided_checksum_char = (unsigned char)provided_checksum;
//
//    // Return true if the calculated checksum matches the provided checksum
//    return checksum == provided_checksum_char;
//}
//
//
//
///**
// * @brief Converts NMEA coordinates to decimal degrees format.
// * 
// * @param coord The NMEA coordinate string (e.g., "12345.678").
// * @param direction The direction indicator ('N', 'S', 'E', or 'W').
// * @return The coordinate in decimal degrees format.
// */
//double convert_to_decimal(const char *coord, char direction) {
//    double raw = atof(coord);  // Convert the coordinate string to a float
//    int degrees = (int)(raw / 100);  // Get the degrees part
//    double minutes = raw - (degrees * 100);  // Get the minutes part
//    double decimal = degrees + (minutes / 60.0);  // Convert to decimal degrees
//    if (direction == 'S' || direction == 'W') {
//        decimal = -decimal;  // If South or West, make the value negative
//    }
//    return decimal;
//}
//
///**
// * @brief Converts a time string from NMEA format (hhmmss.sss) to a 24-hour format (hh:mm:ss.sss).
// * 
// * @param time_str The time string to convert.
// * @return The time string in 24-hour format.
// */
//char* convert_to_24hr_format(const char *time_str) {
//    static char formatted_time[20];  // Buffer for formatted time string
//
//    int hours, minutes;
//    float seconds;
//
//    // Parse the time string into hours, minutes, and seconds
//    if (sscanf(time_str, "%2d%2d%f", &hours, &minutes, &seconds) != 3) {
//        return "Invalid time format";  // Return an error message if parsing fails
//    }
//
//    // Check for valid hour, minute, and second values
//    if (hours < 0 || hours >= 24 || minutes < 0 || minutes >= 60 || seconds < 0 || seconds >= 60) {
//        return "Invalid time values";  // Return an error message if values are invalid
//    }
//
//    // Format the time as a 24-hour string
//    snprintf(formatted_time, sizeof(formatted_time), "%02d:%02d:%.3f", hours, minutes, seconds);
//
//    return formatted_time;
//}
//
///**
// * @brief Checks if the user has arrived at the destination based on the current and destination coordinates.
// * 
// * @param curr_lat The current latitude in decimal degrees.
// * @param curr_lon The current longitude in decimal degrees.
// */
//void check_arrival(double curr_lat, double curr_lon) {
//     // Define destination coordinates (for comparison with current position)
//    double dest_lat = 44.973575;
//    double dest_lon = -93.218765;
//    
//    // Debugging: Print current and destination coordinates
//    USART2_PRINTF_MOD("Current lat: %.8f, current lon: %.8f\r\n", curr_lat, curr_lon);
//    USART2_PRINTF_MOD("Destination lat: %.8f, destination lon: %.8f\r\n", dest_lat, dest_lon);
//
//    // Scale coordinates for fixed-point comparison
//    double scaled_curr_lat = curr_lat * SCALE_FACTOR;
//    double scaled_curr_lon = curr_lon * SCALE_FACTOR;
//    double scaled_dest_lat = dest_lat * SCALE_FACTOR;
//    double scaled_dest_lon = dest_lon * SCALE_FACTOR;
//
//    // Debugging: Print scaled coordinates
//    USART2_PRINTF_MOD("Scaled current lat: %.8f, current lon: %.8f\r\n", scaled_curr_lat, scaled_curr_lon);
//    USART2_PRINTF_MOD("Scaled destination lat: %.8f, destination lon: %.8f\r\n", scaled_dest_lat, scaled_dest_lon);
//
//    // Define thresholds for arrival (scaled format)
//    double lat_threshold = 1400.0;
//    double lon_threshold = 10.0;
//
//    // Debugging: Print threshold values
//    USART2_PRINTF_MOD("Latitude threshold: %.8f\r\n", lat_threshold);
//    USART2_PRINTF_MOD("Longitude threshold: %.8f\r\n", lon_threshold);
//
//    // Calculate differences in scaled coordinates
//    double lat_diff = fabs(scaled_curr_lat - scaled_dest_lat);
//    double lon_diff = fabs(scaled_curr_lon - scaled_dest_lon);
//
//    // Print the differences
//    USART2_PRINTF_MOD("Latitude diff: %.8f\r\n", lat_diff);
//    USART2_PRINTF_MOD("Longitude diff: %.8f\r\n", lon_diff);
//
//    // If the differences are within thresholds, we have arrived
//    if (lat_diff <= lat_threshold || lon_diff <= lon_threshold) {
//        USART2_PRINTF_MOD("You have arrived at your destination!\r\n");
//    } else {
//        USART2_PRINTF_MOD("Not yet at the destination. Keep going.\r\n");
//    }
//}
//
//
//
///**
// * @brief Parses a GPGGA sentence from the GPS data.
// * 
// * This function checks if the sentence starts with "$GNGGA" and, if so, extracts the time, latitude, longitude,
// * and direction information. It then converts the latitude and longitude to decimal format, compares the current
// * position with a predefined destination, and prints the parsed data.
// * 
// * @param sentence The GPGGA sentence to be parsed.
// */
//void parse_gngga(const char *sentence) {
//    // Check if the sentence starts with "$GNGGA" (GPGGA sentence type)
//    if (strncmp(sentence, "$GNGGA", 6) != 0) {
//        return; // Not a GPGGA sentence, exit the function
//    }
//    
//    // Create a copy of the sentence to avoid modifying the original sentence
//    char copy[100]; 
//    strncpy(copy, sentence, sizeof(copy));
//    copy[sizeof(copy) - 1] = '\0'; // Null-terminate the string to ensure proper handling
//
//    // Tokenize the sentence to extract relevant fields (time, latitude, longitude, etc.)
//    char *token = strtok(copy, ",");
//    
//    // Parse individual fields from the sentence
//    char *time = strtok(NULL, ",");
//    char *latitude = strtok(NULL, ",");
//    char *lat_dir = strtok(NULL, ",");
//    char *longitude = strtok(NULL, ",");
//    char *lon_dir = strtok(NULL, ",");
//
//    // Convert latitude and longitude to decimal format
//    double lat_decimal = convert_to_decimal(latitude, lat_dir[0]);
//    double lon_decimal = convert_to_decimal(longitude, lon_dir[0]);
//    
//    // Check if the destination is reached based on current coordinates
//    check_arrival(lat_decimal, lon_decimal);
//
//    // Print the parsed data (time, latitude, longitude) for debugging
//    USART2_PRINTF_MOD("Time: %s\r\n", convert_to_24hr_format(time));
//    USART2_PRINTF_MOD("Latitude: %.6f %c\r\n", lat_decimal, lat_dir[0]);
//    USART2_PRINTF_MOD("Longitude: %.6f %c\r\n", lon_decimal, lon_dir[0]);
//}
//
//
///**
// * @brief Main loop function to read and parse GPS data.
// * 
// * The function continuously checks for available GPS data, reads the incoming bytes, and stores them in a buffer.
// * Once a complete sentence is received, it checks for the presence of a GNGGA sentence and processes it.
// * If a GNGGA sentence is found, it calls the `parse_gngga` function to extract and display the information.
// */
//void loop() {
//    // Check if GPS data is available
//    if (GPS_available() == 0) {
//        USART2_PRINTF("No data available....\n");
//        return;
//    }
//
//    // Process incoming GPS data until no data is left
//    while (GPS_available() != 0) {
//        uint8_t incoming = GPS_read(); // Read the latest byte from the GPS module
//
//        // If the byte is '$', break sentences onto new lines for clarity
//        if (incoming == '$') {
//            USART2_PRINTF("\n");
//        }
//        //        USART2_PRINT_BYTE(incoming);
//        
//
//        // Store the incoming byte in the buffer if there is space
//        if (buffer_index < MAX_PACKET_SIZE - 1) { 
//            gps_buffer[buffer_index++] = incoming;
//
//            // Check for the end of a sentence (new line or carriage return)    
//            if (incoming == '\n' || incoming == '\r') {
//                gps_buffer[buffer_index] = '\0'; // Null-terminate the string to process it
//                 
//                //                if (!validate_checksum(gps_buffer)) {
////                    USART2_PRINTF("Invalid checksum\n");
////                }
//                
//                // Check if the sentence contains a valid GNGGA sentence and process it
//                if ((strstr(gps_buffer, "GNGGA") != NULL)) {
//                    USART2_PRINTF("Found GNGGA sentence: ");
//                    USART2_PRINTF_MOD("%s\n", gps_buffer);
//                    parse_gngga(gps_buffer); // Parse the GNGGA sentence
//                }
//                
//                //        if (strstr(gps_buffer, "GNRMC") != NULL) {
////            // Print the current sentence (for debugging purposes)
////            USART2_PRINTF("Found GNRMC sentence: ");
////            USART2_PRINTF_MOD("%s\n", gps_buffer);
////         
////
////        }
//
//                // Reset buffer index for the next sentence
//                buffer_index = 0;
//            }
//        } else {
//            // Handle buffer overflow (reset buffer if it exceeds maximum size)
//            USART2_PRINTF("Buffer overflow, resetting...\n");
//            buffer_index = 0;
//        }
//    }
//}
//
//
//
//int main(void) {
//    
//    GPS_begin();   // Initialize GPS module
//    TWI_init();    // Initialize I2C (TWI) communication
//    USART2_INIT(); // Initialize USART2 for serial communication
//    loop();        // Enter the main loop to process GPS data
//    
////    while (1) {}
//}
