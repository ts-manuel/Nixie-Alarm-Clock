

#include "settings.h"
#include <stdio.h>
#include <string.h>


/**
 * Load settings from eeprom
 * 
 */
void SETTINGS_Load(Settings_t* settings)
{
    settings->volume = 30;
    
    memset(settings->alarmSlots, 0, sizeof(AlarmSlot_t) * 4);
    settings->alarmSlots[0].day   = 0b00000001;
    settings->alarmSlots[0].hour  = 0x06;
    settings->alarmSlots[0].min   = 0x22;
}


/**
 * Store settings to eeprom
 * 
 */
void SETTING_Store(Settings_t* settings)
{
    
}