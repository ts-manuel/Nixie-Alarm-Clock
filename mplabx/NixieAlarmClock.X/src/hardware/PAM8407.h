/* 
 * File:   PAM8407.h
 * Author: ts-manuel
 * 
 * Volume settable in 32 steps.
 * When exiting the shutdown state the default volume is set to 12dB (value 23).
 * 31: +24        23: +12      15: +0      7: -14
 * 30: +22.5      22: +10.36   14: -1.5    6: -16
 * 29: +21        21: +9       13: -3      5: -18
 * 28: +19.5      20: +7.5     12: -4.5    4: -20
 * 27: +18        19: +6       11: -6      3: -22
 * 26: +16.5      18: +4.5     10: -8      2: -24
 * 25: +15        17: +3        9: -10     1: -26
 * 24: +13.5      16: +1.5      8: -12     0: -80
 * 
 */

#ifndef _HARDWARE_PAM8407_H_
#define	_HARDWARE_PAM8407_H_

#include <stdint.h>

void PAM8407_Enable(void);

void PAM8407_Disable(void);

void PAM8407_SetVolume(int8_t vol);

uint8_t PAM8407_GetVolume(void);

#endif	/* _HARDWARE_PAM8407_H_ */

