
#include "settings.h"
#include "alarm.h"
#include "rtcc.h"


/**
 * Compares the current time with every alarm slot if a match is fount the
 * function returns true to signal that is time to ring the alarm.
 * After a match is fount the snozzed flag inside the alarm slot is set so
 * that the alarm is not re-triggered if the snooze button is pressed in the
 * same minute.
 * 
 * @param time      (bcdTime_t*)    Current time in BCD format
 * @param alarms    (AlarmSlot_t*)  Alarm slots
 * @return          (bool)          True if it is time to ring the alarm
 */
bool ALARM_Check(bcdTime_t* time, AlarmSlot_t* alarms)
{
    bool play = false;
    
    for (uint8_t i = 0; i < _AL_SLOT_COUNT; i++)
    {
        AlarmSlot_t* alarm = &alarms[i];
        
        if ((alarm->day & (1 << (time->tm_wday - 1))) && alarm->hour == time->tm_hour && alarm->min == time->tm_min)
        {
            if (alarm->snoozed == false)
            {
                play = true;
                alarm->snoozed = true;
            }
        }
        else
        {
            alarm->snoozed = false;
        }
    }
    
    return play;
}