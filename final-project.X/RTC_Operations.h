/* 
 * File:   RTC_Operations.h
 * Author: chris
 *
 * Description:
 * Header file for Real-Time Counter (RTC) operations.
 * It defines constants and declares the initialization function for the RTC module.
 *
 * Created on October 21, 2024, 5:50 PM
 */

#ifndef RTC_OPERATIONS_H
#define RTC_OPERATIONS_H

#include <avr/io.h> // Include AVR I/O definitions to access registers and constants

// Default RTC period value
// Defines the maximum count value before the RTC overflows. 
// This determines the overflow interval based on the clock frequency.
#ifndef RTC_PERIOD
#define RTC_PERIOD 16383
#endif

// Default RTC compare value
// Used to trigger a specific event when the counter reaches this value.
// Should be less than or equal to RTC_PERIOD.
#ifndef RTC_CMP
#define RTC_CMP 32766
#endif

// Function Prototypes

/**
 * @brief Initializes the RTC module.
 * Configures the RTC with specific period, compare, and control settings, enabling
 * interrupts and selecting the clock source.
 */
void RTC_init(void);

#endif /* RTC_OPERATIONS_H */
