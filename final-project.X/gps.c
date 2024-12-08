#include "gps.h"
#include "i2c.h"
#include "printf.h"


// GPS Buffers
volatile uint8_t gpsData[MAX_PACKET_SIZE]; // Buffer for raw GPS data
volatile uint8_t _head = 0, _tail = 0;     // Circular buffer pointers
char gps_sentence[MAX_PACKET_SIZE];        // Buffer for a complete GPS sentence
uint16_t buffer_index = 0;  // Index to track gps_sentence
double previous_distance = -1.0; // To calculate difference in updated distance 


// Initialize GPS and peripherals
void GPS_init(void) {
    _head = 0;
    _tail = 0;
    TWI_init();    // Initialize I2C
    USART2_INIT(); // Initialize UART for debugging
}


/**
 * @brief Converts NMEA coordinates from degree minutes to decimal degrees format.
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
        return "Invalid time format";  
    }

    // Check for valid hour, minute, and second values
    if (hours < 0 || hours >= 24 || minutes < 0 || minutes >= 60 || seconds < 0 || seconds >= 60) {
        return "Invalid time values";  
    }

    // Format the time as a 24-hour string
    snprintf(formatted_time, sizeof(formatted_time), "%02d:%02d:%.3f", hours, minutes, seconds);

    return formatted_time;
}


// Function to convert degrees to radians
double degrees_to_radians(double degrees) {
    return degrees * M_PI / 180.0;
}

// Function to calculate the distance between two GPS coordinates
double calc_distance(double lat1, double lon1, double lat2, double lon2) {
    // Convert latitude and longitude from degrees to radians
    double lat1_rad = degrees_to_radians(lat1);
    double lon1_rad = degrees_to_radians(lon1);
    double lat2_rad = degrees_to_radians(lat2);
    double lon2_rad = degrees_to_radians(lon2);

    // Differences in coordinates
    double delta_lat = lat2_rad - lat1_rad;
    double delta_lon = lon2_rad - lon1_rad;

    // Haversine formula
    double a = sin(delta_lat / 2) * sin(delta_lat / 2) +
               cos(lat1_rad) * cos(lat2_rad) *
               sin(delta_lon / 2) * sin(delta_lon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    // Distance in meters
    return EARTH_RADIUS * c;
}

/**
 * @brief Checks if the current location is near the destination and prints status updates.
 * 
 * This function compares the current GPS coordinates with a predefined destination's coordinates.
 * It prints the current status, whether the user is approaching, arrived, or still away from the destination.
 * 
 * @param curr_lat Current latitude in decimal degrees.
 * @param curr_lon Current longitude in decimal degrees.
 */
void check_arrival(double curr_lat, double curr_lon) {
    // Define destination coordinates 
    // Curret destination: Platonic Figure by UMN ME Building
    double dest_lat = 44.974796;
    double dest_lon = -93.233444;
    
    // Calculate the distance to the destination
    double distance = calc_distance(curr_lat, curr_lon, dest_lat, dest_lon);

    // Print current and destination coordinates
    USART2_PRINTF("----------------------------------------------\r\n");
    USART2_PRINTF("-------------Destination Location-------------\r\n");
    USART2_PRINTF("----------------------------------------------\r\n");
    USART2_PRINTF_MOD("%.8f, %.8f\r\n", dest_lat, dest_lon);
    USART2_PRINTF("-----------------------------------------------\r\n");
    USART2_PRINTF("-----------Distance From Destination-----------\r\n");
    USART2_PRINTF("-----------------------------------------------\r\n");
    USART2_PRINTF_MOD("%.8f m\r\n", distance);
    if (!(statesActive & PULSE_ARRIVED )) {
    // Check if the user is getting closer to the destination
    if (previous_distance > 0 && distance < previous_distance) {
        statesActive |= PULSE_DEST_CLOSER;
        statesActive &= ~PULSE_DEST_FARTHER;
        USART2_PRINTF("You're getting closer to your destination.\r\n");
    } else if (previous_distance > 0) {
        statesActive |= PULSE_DEST_FARTHER;
        statesActive &= ~PULSE_DEST_CLOSER;
        USART2_PRINTF("You're moving away from the destination.\r\n");
    }
    }

    // Print arrival status
    USART2_PRINTF("-----------------------------------------------\r\n");
    USART2_PRINTF("--------------------STATUS---------------------\r\n");
    USART2_PRINTF("-----------------------------------------------\r\n");

    // Define arrival thresholds (e.g., 50 meters)
    if (distance <= GPS_THRESHOLD) {
        statesActive |= PULSE_ARRIVED;
        statesActive &= ~PULSE_DEST_FARTHER;
        statesActive &= ~PULSE_DEST_CLOSER;
        USART2_PRINTF("You have arrived at your destination!\r\n");
    } else {
        USART2_PRINTF("Not yet at the destination. Keep going.\r\n");
        statesActive &= ~PULSE_ARRIVED;
    }

    // Update the previous distance for the next comparison
    previous_distance = distance;

    USART2_PRINTF("\r\n");
    USART2_PRINTF("===================================================\r\n");
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
    // Check if the sentence starts with "$GNGGA" 
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
                    parse_gngga(gps_sentence);
                }

                buffer_index = 0; // Reset for next sentence
            }
        }
    }

    gps_data_ready = false; // Reset data-ready flag
}


