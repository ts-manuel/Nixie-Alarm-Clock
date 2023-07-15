/* 
 * File:   PAM8407.h
 * Author: ts-manuel
 * 
 * Volume settable in 32 steps.
 * When exiting the shutdown state the default volume is set to 12dB (step 9).
 * 1: +24        9 : +12      17: +0      25: -14
 * 2: +22.5      10: +10.36   18: -1.5    26: -16
 * 3: +21        11: +9       19: -3      27: -18
 * 4: +19.5      12: +7.5     20: -4.5    28: -20
 * 5: +18        13: +6       21: -6      29: -22
 * 6: +16.5      14: +4.5     22: -8      30: -24
 * 7: +15        15: +3       23: -10     31: -26
 * 8: +13.5      16: +1.5     24: -12     32: -80
 * 
 *
 * Created on December 3, 2022, 4:50 PM
 */

#ifndef PAM8407_H
#define	PAM8407_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif
   
    void PAM8407_Enable(uint8_t volume);
    
    void PAM8407_Disable(void);
    
    uint8_t PAM8407_VolumeUp(void);
    
    uint8_t PAM8407_VolumeDown(void);
    
    uint8_t PAM8407_VolumeSet(uint8_t step);
    
    uint8_t PAM8407_VolumeGet(void);

#ifdef	__cplusplus
}
#endif

#endif	/* PAM8407_H */

