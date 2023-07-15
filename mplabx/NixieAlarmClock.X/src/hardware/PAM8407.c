

#include "PAM8407.h"
#include "../settings.h"
#include "../delay.h"
#include "../../mcc_generated_files/system.h"
#include "../../mcc_generated_files/pin_manager.h"
#include "logging/logging.h"


static uint8_t attStep = 9;


void PAM8407_Enable(uint8_t volume)
{
    // Enable amplifier
    O_PA_SHUTDOWN_SetHigh();
    
    // Set internal state to default value
    attStep = 9;
    
    PAM8407_VolumeSet(volume);
}
    

void PAM8407_Disable(void)
{
    O_PA_SHUTDOWN_SetLow();
}
    

/**
 * Increases the amplifier volume by toggling the UP pin
 * and updates the internal state of the attenuator.
 */
uint8_t PAM8407_VolumeUp(void)
{
    uint8_t volume;
    
    attStep = attStep > 1 ? attStep - 1 : 1;
    volume = attStep;
    
    //LOG_DEBUG_ARGS("PAM8407_VolumeUp() - Volume: %d\n", volume);
    
    O_PA_UP_SetLow();
    __delay_ms(100);
    O_PA_UP_SetHigh();
    __delay_ms(100);
    
    return volume;
}
    

/**
 * Decreases the amplifier volume by toggling the DOWN pin
 * and updates the internal state of the attenuator.
 */
uint8_t PAM8407_VolumeDown(void)
{
    uint8_t volume;
    
    attStep = attStep < 32 ? attStep + 1 : 32;
    volume = attStep;
    
    //LOG_DEBUG_ARGS("PAM8407_VolumeDown() - Volume: %d\n", volume);
    
    O_PA_DOWN_SetLow();
    __delay_ms(100);
    O_PA_DOWN_SetHigh();
    __delay_ms(100);
    
    return volume;
}


/**
 * Sets the desired volume level by repeatedly calling the UP DOWN functions 
 * 
 * @param step  Desired volume, (1 = max to 32 = min)
 */
uint8_t PAM8407_VolumeSet(uint8_t step)
{
    uint8_t volume;
    
    step = step > 32 ? 32 : step;
    step = step <  1 ?  1 : step;
    
    while(step > attStep)
        volume = PAM8407_VolumeDown();
        
    while(step < attStep)
        volume = PAM8407_VolumeUp();
    
    return volume;
}


/**
 * Returns the internal state of the attenuator
 * 
 * @return  Attenuator value
 */
uint8_t PAM8407_VolumeGet(void)
{
    return attStep;
}