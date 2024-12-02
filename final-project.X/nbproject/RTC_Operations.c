#include <avr/io.h>
#include "RTC_Operations.h"

void RTC_init(void)
{
    
    /* Initialize RTC: */
    while (RTC.STATUS > 0)
    {
        ; /* Wait for all registers to synchronize */
    }
    
    /* Set period for PIT */
    RTC.PER = RTC_PERIOD;
    RTC.CMP = RTC_CMP;
    
    RTC.CTRLA = RTC_PRESCALER_DIV1_gc 
        | RTC_RTCEN_bm /* Enable: enabled */
        | RTC_RUNSTDBY_bm; /* Run In Standby: enabled */
    
    /* Select the 32.768 kHz clock source */
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
    
    /* Run in debug mode */
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    
    /* Enable Overflow Interrupt */
     RTC.INTCTRL |= RTC_OVF_bm;
    
    
}
