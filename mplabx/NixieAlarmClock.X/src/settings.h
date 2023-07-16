/* 
 * File:   settings.h
 * Author: ts-manuel
 *
 * Functions to load and save the settings
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

#include <stdint.h>
#include <stdbool.h>
#include "tasks/alarm.h"


typedef struct
{
    uint8_t     volume;                     // Amplifier volume (0 to 32)
} Settings_t;



void SETTINGS_Load(Settings_t* settings);


void SETTING_Store(Settings_t* settings);


#endif	/* SETTINGS_H */

