/* 
 * File:   alarm.h
 * Author: Manuel
 *
 * Created on December 11, 2022, 5:55 PM
 */

#ifndef ALARM_H
#define	ALARM_H

#include <stdint.h>
#include <stdbool.h>
#include "rtcc.h"

#define _AL_SLOT_COUNT  4

typedef struct
{
    int8_t day;     // Alarm day bitmap 0x00 = alarm slot empty, bit 0 = MO, bit 6 = TU ...
    int8_t hour;    // BCD alarm hour       (0xDU H = )
    int8_t min;     // BCD alarm minutes    (0xDU)
    bool   snoozed;
} AlarmSlot_t;


bool ALARM_Check(bcdTime_t* time, AlarmSlot_t* alarms);

#endif	/* ALARM_H */

