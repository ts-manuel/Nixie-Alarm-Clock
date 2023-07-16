/* 
 * File:   alarm.h
 * Author: ts-manuel
 *
 * Compares current time with all set alarms and sets flag if triggered
 */

#include "rtcc.h"
#include "alarm.h"
#include "utils/logging.h"

static bcdTime_t lastTriggerTime;
static bool checkLastTriggerTime = false;


void ALARM_Update(void)
{
    bcdTime_t time;
    
    // Read time from RTC
    if (RTCC_BCDTimeGet(&time) == false)
        return;
    
    // Don't trigger if already triggered for this minute
    if (checkLastTriggerTime)
    {
        if (lastTriggerTime.tm_wday == time.tm_wday &&
                lastTriggerTime.tm_hour == time.tm_hour &&
                lastTriggerTime.tm_min == time.tm_min)
            return;
    }
    
    checkLastTriggerTime = false;
    
    for (uint8_t i = 0; i < _ALARM_SLOT_COUNT; i++)
    {
        AlarmSlot_t* alarm = &alarmSlots[i];
        
        if ((alarm->day & (1 << (time.tm_wday - 1))) &&
                alarm->hour == time.tm_hour &&
                alarm->min == time.tm_min)
        {
            LOG_TRACE1("ALARM_Update() - Alarm %d triggered\n", (int)i);
            flagTriggerAlarm = true;
            lastTriggerTime = time;
            checkLastTriggerTime = true;
        }
    }
}