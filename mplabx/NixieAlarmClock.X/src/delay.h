/* 
 * File:   delay.h
 * Author: Manuel
 *
 * Created on October 31, 2022, 9:21 AM
 */

#ifndef DELAY_H
#define	DELAY_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

void __delay_us(int x);

void __delay_ms(int x);

void MillisInitialize(void);

uint32_t Millis(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DELAY_H */

