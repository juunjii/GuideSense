#include "gps.h"
#include "i2c.h"
#include "printf.h"


// Global Variables
//volatile bool gps_data_ready = false; // Flag to indicate new GPS data is available

// GPS Buffers
volatile uint8_t gpsData[MAX_PACKET_SIZE]; // Buffer for raw GPS data
volatile uint8_t _head = 0, _tail = 0;     // Circular buffer pointers
char gps_sentence[MAX_PACKET_SIZE];        // Buffer for a complete GPS sentence
uint16_t buffer_index = 0;  



// Initialize GPS and peripherals
void GPS_init(void) {
    _head = 0;
    _tail = 0;
    TWI_init();    // Initialize I2C
    USART2_INIT(); // Initialize UART for debugging
}


/**
 * @brief Converts NMEA coordinates to decimal degrees format.
 * 
 * @param coord The NMEA coordinate string (e.g., "12345.678").
 * @param direction The direction indicator ('N', 'S', 'E', or 'W').
 * @return The coordinate in decimal degrees format.
 */
double convert_to_decimal(const char *coord, char direction) {
    double raw = atof(coord);  // Convert the coordinate string to a float
    int degrees = (int)(raw / 100);  // Get the degrees part
    double minutes = raw - (degrees * 100);  // Get the minutes part
    double decimal = degrees + (minutes / 60.0);  // Convert to decimal degrees
    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;  // If South or West, make the value negative
    }
    return decimal;
}

/**
 * @brief Converts a time string from NMEA format (hhmmss.sss) to a 24-hour format (hh:mm:ss.sss).
 * 
 * @param time_str The time string to convert.
 * @return The time string in 24-hour format.
 */
char* convert_to_24hr_format(const char *time_str) {
    static char formatted_time[20];  // Buffer for formatted time string

    int hours, minutes;
    float seconds;

    // Parse the time string into hours, minutes, and seconds
    if (sscanf(time_str, "%2d%2d%f", &hours, &minutes, &seconds) != 3) {
        return "Invalid time format";  // Return an error message if parsing fails
    }

    // Check for valid hour, minute, and second values
    if (hours < 0 || hours >= 24 || minutes < 0 || minutes >= 60 || seconds < 0 || seconds >= 60) {
        return "Invalid time values";  // Return an error message if values are invalid
    }

    // Format the time as a 24-hour string
    snprintf(formatted_time, sizeof(formatted_time), "%02d:%02d:%.3f", hours, minutes, seconds);

    return formatted_time;
}

/**
 * @brief Checks if the user has arrived at the destination based on the current and destination coordinates.
 * 
 * @param curr_lat The current latitude in decimal degrees.
 * @param curr_lon The current longitude in decimal degrees.
 */
void check_arrival(double curr_lat, double curr_lon) {
     // Define destination coordinates (for comparison with current position)
    double dest_lat = 44.973575;
    double dest_lon = -93.218765;
    
    // Debugging: Print current and destination coordinates
    USART2_PRINTF_MOD("Current lat: %.8f, current lon: %.8f\r\n", curr_lat, curr_lon);
    USART2_PRINTF_MOD("Destination lat: %.8f, destination lon: %.8f\r\n", dest_lat, dest_lon);

    // Scale coordinates for fixed-point comparison
    double scaled_curr_lat = curr_lat * SCALE_FACTOR;
    double scaled_curr_lon = curr_lon * SCALE_FACTOR;
    double scaled_dest_lat = dest_lat * SCALE_FACTOR;
    double scaled_dest_lon = dest_lon * SCALE_FACTOR;

    // Debugging: Print scaled coordinates
    USART2_PRINTF_MOD("Scaled current lat: %.8f, current lon: %.8f\r\n", scaled_curr_lat, scaled_curr_lon);
    USART2_PRINTF_MOD("Scaled destination lat: %.8f, destination lon: %.8f\r\n", scaled_dest_lat, scaled_dest_lon);

    // Define thresholds for arrival (scaled format)
    double lat_threshold = 450.0; // 50 meters
    double lon_threshold = 450.0; //  50 meters 

    // Debugging: Print threshold values
    USART2_PRINTF_MOD("Latitude threshold: %.8f\r\n", lat_threshold);
    USART2_PRINTF_MOD("Longitude threshold: %.8f\r\n", lon_threshold);

    // Calculate differences in scaled coordinates
    double lat_diff = fabs(scaled_curr_lat - scaled_dest_lat);
    double lon_diff = fabs(scaled_curr_lon - scaled_dest_lon);

    // Print the differences
    USART2_PRINTF_MOD("Latitude diff: %.8f\r\n", lat_diff);
    USART2_PRINTF_MOD("Longitude diff: %.8f\r\n", lon_diff);

    // If the differences are within thresholds, we have arrived
    if ((lat_diff <= lat_threshold) && (lon_diff <= lon_threshold)) {
        statesActive |= PULSE_ARRIVED;
        USART2_PRINTF_MOD("You have arrived at your destination!\r\n");
        
    } else {
        USART2_PRINTF_MOD("Not yet at the destination. Keep going.\r\n");
    }
}



/**
 * @brief Parses a GPGGA sentence from the GPS data.
 * 
 * This function checks if the sentence starts with "$GNGGA" and, if so, extracts the time, latitude, longitude,
 * and direction information. It then converts the latitude and longitude to decimal format, compares the current
 * position with a predefined destination, and prints the parsed data.
 * 
 * @param sentence The GPGGA sentence to be parsed.
 */
void parse_gngga(const char *sentence) {
    // Check if the sentence starts with "$GNGGA" (GPGGA sentence type)
    if (strncmp(sentence, "$GNGGA", 6) != 0) {
        return; // Not a GPGGA sentence, exit the function
    }
    
    // Create a copy of the sentence to avoid modifying the original sentence
    char copy[100]; 
    strncpy(copy, sentence, sizeof(copy));
    copy[sizeof(copy) - 1] = '\0'; // Null-terminate the string to ensure proper handling

    // Tokenize the sentence to extract relevant fields (time, latitude, longitude, etc.)
    char *token = strtok(copy, ",");
    
    // Parse individual fields from the sentence
    char *time = strtok(NULL, ",");
    char *latitude = strtok(NULL, ",");
    char *lat_dir = strtok(NULL, ",");
    char *longitude = strtok(NULL, ",");
    char *lon_dir = strtok(NULL, ",");

    // Convert latitude and longitude to decimal format
    double lat_decimal = convert_to_decimal(latitude, lat_dir[0]);
    double lon_decimal = convert_to_decimal(longitude, lon_dir[0]);
    
    // Check if the destination is reached based on current coordinates
    check_arrival(lat_decimal, lon_decimal);

    // Print the parsed data (time, latitude, longitude) for debugging
    USART2_PRINTF_MOD("Time: %s\r\n", convert_to_24hr_format(time));
    USART2_PRINTF_MOD("Latitude: %.6f %c\r\n", lat_decimal, lat_dir[0]);
    USART2_PRINTF_MOD("Longitude: %.6f %c\r\n", lon_decimal, lon_dir[0]);
}


// Check and parse GPS sentences
void parse_gps_data(void) {
    if (!gps_data_ready) return; // No new data

    while (_tail != _head) { // Process all available data
        uint8_t incoming = gpsData[_tail++];
        _tail %= MAX_PACKET_SIZE; // Wrap tail index

        if (incoming == '$') { // Start of a new sentence
            buffer_index = 0;
        }

        // Store byte in sentence buffer
        if (buffer_index < MAX_PACKET_SIZE - 1) {
            gps_sentence[buffer_index++] = incoming;

            // Check for end of sentence
            if (incoming == '\n' || incoming == '\r') {
                gps_sentence[buffer_index] = '\0'; // Null-terminate string

                // Process GNGGA sentences
                if (strstr(gps_sentence, "GNGGA") != NULL) {
                    USART2_PRINTF_MOD("\n");
                    USART2_PRINTF_MOD("Found GNGGA sentence: %s\n", gps_sentence);
                    parse_gngga(gps_sentence);
                }

                buffer_index = 0; // Reset for next sentence
            }
        }
    }

    gps_data_ready = false; // Reset data-ready flag
}


