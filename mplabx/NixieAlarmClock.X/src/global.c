/* 
 * File:   global.c
 * Author: ts-manuel
 *
 * This file contains the declaration for all global variables used to transfer
 * information between modules
 */

#include "config.h"
#include "tasks/display.h"
#include "tasks/alarm.h"

DispBuffer_t display;

AlarmSlot_t alarmSlots[_ALARM_SLOT_COUNT];
bool flagTriggerAlarm;