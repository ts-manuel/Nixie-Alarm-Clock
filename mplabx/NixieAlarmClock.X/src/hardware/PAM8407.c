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

#include <stdint.h>
#include <stdbool.h>
#include "PAM8407.h"
#include "time/delay.h"
#include "system.h"
#include "pin_manager.h"

#define _PAM8407_VOM_PULSE_MS   100

static void VolumeUp(void);
static void VolumeDown(void);

static bool enabled = false;
static uint8_t volume = 23;


void PAM8407_Enable(void)
{
    // Enable amplifier
    O_PA_SHUTDOWN_SetLow();
    TIME_delay_us(1);
    O_PA_SHUTDOWN_SetHigh();
    
    // Set internal state to default value
    volume = 23;
    
    enabled = true;
}
    

void PAM8407_Disable(void)
{
    O_PA_SHUTDOWN_SetLow();
    
    enabled = false;
}
    

/**
 * Sets the desired volume level by repeatedly calling the UP DOWN functions 
 * 
 * @param vol   Desired volume, (0 = min to 31 = max)
 */
void PAM8407_SetVolume(int8_t vol)
{
    // Clamp input volume
    vol = vol > 31 ? 31 : vol;
    vol = vol <  0 ?  0 : vol;
    
    // Change volume
    while(vol > volume)
        VolumeDown();
        
    while(vol < volume)
        VolumeUp();
}


/**
 * Returns the volume setting
 * 
 * @return  Volume
 */
uint8_t PAM8407_GetVolume(void)
{
    return volume;
}


/**
 * Increases the amplifier volume by toggling the UP pin
 * and updates the internal state of the attenuator.
 */
static void VolumeUp(void)
{
    O_PA_UP_SetLow();
    TIME_delay_ms(_PAM8407_VOM_PULSE_MS);
    O_PA_UP_SetHigh();
    TIME_delay_ms(_PAM8407_VOM_PULSE_MS);
    
    volume ++;
}
    

/**
 * Decreases the amplifier volume by toggling the DOWN pin
 * and updates the internal state of the attenuator.
 */
static void VolumeDown(void)
{
    O_PA_DOWN_SetLow();
    TIME_delay_ms(_PAM8407_VOM_PULSE_MS);
    O_PA_DOWN_SetHigh();
    TIME_delay_ms(_PAM8407_VOM_PULSE_MS);
    
    volume --;
}