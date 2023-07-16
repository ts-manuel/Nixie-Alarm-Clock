/* 
 * File:   millis.h
 * Author: ts-manuel
 *
 * Keeps track of the time elapsed in milliseconds from power on.
 * 
 * The milliseconds are stored as a 32 bit variable and will overflow
 * every 49 days 17 h and 3 minutes.
 * 
 * The interrupt from timer 1 is used to increment time, it fires every 1.0070801 ms
 * so the millis value will drift away from the RTC.
 */

#ifndef _TIME_MILLIS_H_
#define	_TIME_MILLIS_H_

uint32_t TIME_Millis(void);

#endif	/* _TIME_MILLIS_H_ */

