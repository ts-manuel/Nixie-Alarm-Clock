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

#include <stdint.h>
#include "system.h"
#include "millis.h"


static volatile uint32_t millis = 0;

/*
 *  Timer 1 ISR fires every 1.0070801 ms
 */
void TMR1_CallBack(void)
{
    // Disable interrupts to increment the value atomically
    __builtin_disable_interrupts();
    millis++;
    __builtin_enable_interrupts();
}


/*
 * Return the elapsed milliseconds
 */
uint32_t TIME_Millis(void)
{
    uint32_t res;
    
    // Disable interrupts to read the value atomically
    __builtin_disable_interrupts();
    res = millis;
    __builtin_enable_interrupts();
   
    return res;
}