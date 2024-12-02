/* 
 * File:   RTC_Operations.h
 * Author: chris
 *
 * Created on October 21, 2024, 5:50 PM
 */

#ifndef RTC_OPERATIONS_H
#define	RTC_OPERATIONS_H

#include <avr/io.h>

#ifndef RTC_PERIOD
#define RTC_PERIOD 16383
#endif

#ifndef RTC_CMP
#define RTC_CMP 32766
#endif

//Functions
void RTC_init(void);

#endif	/* RTC_OPERATIONS_H */

