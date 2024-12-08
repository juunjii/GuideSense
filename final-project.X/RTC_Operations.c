#include <avr/io.h> // Include AVR I/O definitions
#include "RTC_Operations.h" // Include custom RTC operation header file

/**
 * @brief Initialize the Real-Time Counter (RTC).
 * Configures the RTC module with specified settings, including clock source,
 * prescaler, interrupt setup, and enabling it to run in standby and debug modes.
 */
void RTC_init(void)
{
    // Wait for all RTC registers to synchronize before making changes
    while (RTC.STATUS > 0)
    {
        ; // Empty loop to ensure synchronization is complete
    }
    
    // Set the period and compare values for the RTC
    RTC.PER = RTC_PERIOD; // Set the RTC period from a predefined constant
    RTC.CMP = RTC_CMP;    // Set the RTC compare value from a predefined constant
    
    // Configure RTC control register
    RTC.CTRLA = RTC_PRESCALER_DIV1_gc // Use a prescaler value of 1 (no division)
                 | RTC_RTCEN_bm       // Enable the RTC module
                 | RTC_RUNSTDBY_bm;   // Allow RTC to run in standby mode
    
    // Select the internal 32.768 kHz clock as the RTC clock source
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
    
    // Configure RTC to continue running during debugging
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    
    // Enable the Overflow Interrupt to trigger an interrupt on RTC overflow
    RTC.INTCTRL |= RTC_OVF_bm;
}
