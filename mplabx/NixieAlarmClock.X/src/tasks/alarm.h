/* 
 * File:   alarm.h
 * Author: ts-manuel
 *
 * Compares current time with all set alarms and sets flag if triggered
 */

#ifndef _TASKS_ALARM_H_
#define	_TASKS_ALARM_H_

#include <stdint.h>
#include <stdbool.h>
#include "config.h"


typedef struct
{
    int8_t day;     // Alarm day bitmap 0x00 = alarm slot empty, bit 0 = MO, bit 6 = TU ...
    int8_t hour;    // BCD alarm hour       (high nibble = decimal, low nibble = 0xDU H = )
    int8_t min;     // BCD alarm minutes    (0xDU)
} AlarmSlot_t;

extern AlarmSlot_t alarmSlots[_ALARM_SLOT_COUNT];
extern bool flagTriggerAlarm;

void ALARM_Update(void);

#endif	/* _TASKS_ALARM_H_ */

