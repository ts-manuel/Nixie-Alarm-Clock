/* 
 * File:   settings.h
 * Author: ts-manuel
 *
 * Automatically store setting when modified and load on startup
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "memory/flash.h"
#include "config.h"
#include "utils/millis.h"
#include "utils/logging.h"
#include "tasks/alarm.h"
#include "tasks/player.h"
#include "settings.h"


typedef struct 
{
    AlarmSlot_t alarmSlots[_ALARM_SLOT_COUNT];
    int8_t playerVolume;
} Settings_t;


// Reserve last page of flash for storing settings
#define _FLASH_STORAGE_ADDR 0xa700
uint16_t __attribute__((space(prog), address(_FLASH_STORAGE_ADDR), noload, keep, unused)) flashStorage[FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS];

uint16_t  __attribute__((aligned(4))) rowBuffer[FLASH_WRITE_ROW_SIZE_IN_INSTRUCTIONS];


static bool CompareSettings(Settings_t* s1, Settings_t* s2);
static uint8_t ComputeChecksum(Settings_t* s);
static void CopySettings(Settings_t* source, Settings_t* dest);
static void GetSettings(Settings_t* s);
static void SetSettings(Settings_t* s);
static void StoreSettings(Settings_t* s);


/**
 * Restore settings from eeprom
 */
void SETTINGS_Initialize(void)
{
    Settings_t settings;
    uint8_t* sPtr = (uint8_t*)(&settings);
    uint16_t i;
    uint8_t checksum = 0;
    
    LOG_TRACE1("SETTINGS_Init(): reading flash\n");
    
    for (i = 0; i < sizeof(Settings_t); i++)
    {
        sPtr[i] = (uint8_t)FLASH_ReadWord16((uint32_t)(_FLASH_STORAGE_ADDR + i*2));
        checksum += sPtr[i];
        
        LOG_TRACE0("SETTINGS_Init(): flash[%d] = 0x%02x\n", i, sPtr[i]);
    }
    
    checksum += (uint8_t)FLASH_ReadWord16((uint32_t)(_FLASH_STORAGE_ADDR + i*2));
    LOG_TRACE0("SETTINGS_Init(): flash[%d] = 0x%02x\n", i, checksum);
    
    if (checksum == 0)
    {
        LOG_INFO("SETTINGS_Init(): loading settings\n");
        
        SetSettings(&settings);
    }
    else
    {
        LOG_WARNING("SETTINGS_Init(): checksum error, loading default settings\n");
        
        // Set default volume
        playerVolume = 11;
        
        // Clear alarm slots
        memset(alarmSlots, 0, sizeof(AlarmSlot_t) * _ALARM_SLOT_COUNT);
    }
}


/**
 * Store settings only if changed
 */
void SETTINGS_Update(void)
{
    static Settings_t prevSettings;
    static bool prevSettingsValid = false;
    static uint32_t lastMillis = 0;
    uint32_t millis = TIME_Millis();
    Settings_t settings;
    
    if (millis - lastMillis > _SETTINGS_CHECK_INTERVAL_MS)
    {
        lastMillis = millis;
        
        GetSettings(&settings);
        
        if (prevSettingsValid)
        {
            if (!CompareSettings(&settings, &prevSettings))
            {
                LOG_INFO("SETTINGS_Update(): Writing settings to flash\n");
                StoreSettings(&settings);
            }
        }
        
        CopySettings(&settings, &prevSettings);
        prevSettingsValid = true;
    }
}


static bool CompareSettings(Settings_t* s1, Settings_t* s2)
{
    uint8_t* _s1 = (uint8_t*)s1;
    uint8_t* _s2 = (uint8_t*)s2;
    
    for (uint16_t i = 0; i < sizeof(Settings_t); i++)
    {
        LOG_TRACE0("Compare settings: s1[%d] = 0x%02x, s2[%d] = 0x%02x,\n", i, _s1[i], i, _s2[i]);
        
        if (_s1[i] != _s2[i])
            return false;
    }
        
    return true;
}


static uint8_t ComputeChecksum(Settings_t* s)
{
    uint8_t checksum = 0;
    uint8_t* sPtr = (uint8_t*)s;
    
    for (uint16_t i = 0; i < sizeof(Settings_t); i++)
    {
        checksum += sPtr[i];
    }
    
    return -checksum;
}


static void CopySettings(Settings_t* source, Settings_t* dest)
{
    for (uint16_t i = 0; i < _ALARM_SLOT_COUNT; i++)
        dest->alarmSlots[i] = source->alarmSlots[i];
    dest->playerVolume = source->playerVolume;
}


static void GetSettings(Settings_t* s)
{
    for (uint16_t i = 0; i < _ALARM_SLOT_COUNT; i++)
        s->alarmSlots[i] = alarmSlots[i];
    s->playerVolume = playerVolume;
}


static void SetSettings(Settings_t* s)
{
    for (uint16_t i = 0; i < _ALARM_SLOT_COUNT; i++)
        alarmSlots[i] = s->alarmSlots[i];
    playerVolume = s->playerVolume;
}


static void StoreSettings(Settings_t* s)
{
    uint8_t* sPtr = (uint8_t*)s;
    uint16_t i;
    
    // Erase flash memory page
    FLASH_ErasePage((uint32_t)_FLASH_STORAGE_ADDR);
    
    // Load buffer with data
    for (i = 0; i < sizeof(Settings_t); i++)
    {
        LOG_TRACE0("SETTINGS StoreSettings(): flash[%d] = 0x%02x\n", i, sPtr[i]);
        rowBuffer[i] = sPtr[i];
    }
    
    // Add checksum
    rowBuffer[i] = ComputeChecksum(s);
    LOG_TRACE0("SETTINGS StoreSettings(): flash[%d] = 0x%02x\n", i, rowBuffer[i]);
    
    // Unlock flash
    FLASH_Unlock(FLASH_UNLOCK_KEY);
    
    // Write row buffer to flash
    if(!FLASH_WriteRow16((uint32_t)_FLASH_STORAGE_ADDR, rowBuffer))
        LOG_WARNING("SETTINGS StoreSettings(): WriteRow16 failed\n");
}