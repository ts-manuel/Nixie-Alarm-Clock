#include "views.h"
#include "tasks/display.h"
#include "tasks/buttons.h"
#include "tasks/alarm.h"
#include "tasks/time.h"
#include "rtcc.h"
#include "utils/logging.h"
#include "hardware/PAM8407.h"


typedef enum {e_VIEW_SET_ALARM_SLOT, e_VIEW_SET_ALARM_HOUR, e_VIEW_SET_ALARM_MIN, e_VIEW_SET_ALARM_DAY} ViewSetAlarmState_t;
const char* viewSetAlarmStateStr[] = {"VIEW_SET_ALARM_SLOT", "VIEW_SET_ALARM_HOUR", "VIEW_SET_ALARM_MIN", "VIEW_SET_ALARM_DAY"};
static ViewSetAlarmState_t state;

static int8_t activeAlarmSot;
static int8_t activeDay;
static AlarmSlot_t alarmSlot;


void SetAlarmInit(void)
{
    activeAlarmSot = 0;
    activeDay = 0;
    state = e_VIEW_SET_ALARM_SLOT;
    
    LOG_TRACE1("View Init: SetAlarm - State: %s\n", viewSetAlarmStateStr[state]);
}


View_t SetAlarmUpdate(void)
{
    ViewSetAlarmState_t nextState = state;

    // Handle inputs
    switch(state)
    {
        case e_VIEW_SET_ALARM_SLOT:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)        // Increment day
            {
                if ((++ activeAlarmSot) >= _ALARM_SLOT_COUNT)
                    activeAlarmSot = 0;
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS) // Decrement day
            {
                if ((-- activeAlarmSot) < 0)
                    activeAlarmSot = _ALARM_SLOT_COUNT - 1;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                alarmSlot = alarmSlots[activeAlarmSot];
                nextState = e_VIEW_SET_ALARM_HOUR;
            }
            break;
            
        case e_VIEW_SET_ALARM_HOUR:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)        // Increment hours
            {
                if ((++ alarmSlot.hour) >= 24)
                    alarmSlot.hour = 0;
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS) // Decrement hours
            {
                if ((-- alarmSlot.hour) < 0)
                    alarmSlot.hour = 23;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                nextState = e_VIEW_SET_ALARM_MIN;
            }
            break;
            
        case e_VIEW_SET_ALARM_MIN:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)        // Increment minutes
            {
                if ((++ alarmSlot.min) >= 60)
                    alarmSlot.min = 0;
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS) // Decrement minutes
            {
                if ((-- alarmSlot.min) < 0)
                    alarmSlot.min = 59;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                nextState = e_VIEW_SET_ALARM_DAY;
            }
            break;
            
        case e_VIEW_SET_ALARM_DAY:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)        // Increment day
            {
                alarmSlot.day |= 1 << activeDay;
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS) // Decrement day
            {
                alarmSlot.day &= ~(1 << activeDay);
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                if ((++ activeDay) >= 7)
                {
                    // Save changes made to the alarm slot and exit task
                    alarmSlots[activeAlarmSot] = alarmSlot;
                    return e_VIEW_HOME;
                }
            }
            break;
            
        default:
            nextState = e_VIEW_SET_ALARM_SLOT;
    }
    
    if (state != nextState)
        LOG_TRACE1("View Update: SetAlarm - State: %s\n", viewSetAlarmStateStr[nextState]);
    
    state = nextState;
    
    
    // Update screen
    switch(state)
    {
        case e_VIEW_SET_ALARM_SLOT:
            display.NEON_MO = (rtcTime.tm_wday == 1) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TU = (rtcTime.tm_wday == 2) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_WE = (rtcTime.tm_wday == 3) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TH = (rtcTime.tm_wday == 4) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_FR = (rtcTime.tm_wday == 5) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SA = (rtcTime.tm_wday == 6) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SU = (rtcTime.tm_wday == 7) ? e_SEG_ON : e_SEG_OFF;

            display.NEON_AL1 = (activeAlarmSot == 0) ? e_SEG_TGL : e_SEG_OFF;
            display.NEON_AL2 = (activeAlarmSot == 1) ? e_SEG_TGL : e_SEG_OFF;
            display.NEON_AL3 = (activeAlarmSot == 2) ? e_SEG_TGL : e_SEG_OFF;
            display.NEON_AL4 = (activeAlarmSot == 3) ? e_SEG_TGL : e_SEG_OFF;
            display.NEON_SET = e_SEG_ON;
            display.NEON_ALM = e_SEG_ON;
            display.NEON_TME = e_SEG_OFF;
            display.NEON_DP  = e_SEG_TGL;

            display.NIXIE_1 = e_SEG_ON;
            display.NIXIE_2 = e_SEG_ON;
            display.NIXIE_3 = e_SEG_ON;
            display.NIXIE_4 = e_SEG_ON;
            display.NIXIE_VAL_1 = rtcTime.tm_hour >> 4;
            display.NIXIE_VAL_2 = rtcTime.tm_hour & 0x0f;
            display.NIXIE_VAL_3 = rtcTime.tm_min >> 4;
            display.NIXIE_VAL_4 = rtcTime.tm_min & 0x0f;
            break;
            
        case e_VIEW_SET_ALARM_HOUR:
            display.NEON_MO = (alarmSlot.day & (1 << 0)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TU = (alarmSlot.day & (1 << 1)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_WE = (alarmSlot.day & (1 << 2)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TH = (alarmSlot.day & (1 << 3)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_FR = (alarmSlot.day & (1 << 4)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SA = (alarmSlot.day & (1 << 5)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SU = (alarmSlot.day & (1 << 6)) ? e_SEG_ON : e_SEG_OFF;

            display.NEON_AL1 = (activeAlarmSot == 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL2 = (activeAlarmSot == 1) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL3 = (activeAlarmSot == 2) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL4 = (activeAlarmSot == 3) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SET = e_SEG_ON;
            display.NEON_ALM = e_SEG_ON;
            display.NEON_TME = e_SEG_OFF;
            display.NEON_DP  = e_SEG_TGL;

            display.NIXIE_1 = e_SEG_TGL;
            display.NIXIE_2 = e_SEG_TGL;
            display.NIXIE_3 = e_SEG_ON;
            display.NIXIE_4 = e_SEG_ON;
            display.NIXIE_VAL_1 = alarmSlot.hour / 10;
            display.NIXIE_VAL_2 = alarmSlot.hour % 10;
            display.NIXIE_VAL_3 = alarmSlot.min / 10;
            display.NIXIE_VAL_4 = alarmSlot.min % 10;
            break;
            
        case e_VIEW_SET_ALARM_MIN:
            display.NEON_MO = (alarmSlot.day & (1 << 0)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TU = (alarmSlot.day & (1 << 1)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_WE = (alarmSlot.day & (1 << 2)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TH = (alarmSlot.day & (1 << 3)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_FR = (alarmSlot.day & (1 << 4)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SA = (alarmSlot.day & (1 << 5)) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SU = (alarmSlot.day & (1 << 6)) ? e_SEG_ON : e_SEG_OFF;

            display.NEON_AL1 = (activeAlarmSot == 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL2 = (activeAlarmSot == 1) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL3 = (activeAlarmSot == 2) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL4 = (activeAlarmSot == 3) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SET = e_SEG_ON;
            display.NEON_ALM = e_SEG_ON;
            display.NEON_TME = e_SEG_OFF;
            display.NEON_DP  = e_SEG_TGL;

            display.NIXIE_1 = e_SEG_ON;
            display.NIXIE_2 = e_SEG_ON;
            display.NIXIE_3 = e_SEG_TGL;
            display.NIXIE_4 = e_SEG_TGL;
            display.NIXIE_VAL_1 = alarmSlot.hour / 10;
            display.NIXIE_VAL_2 = alarmSlot.hour % 10;
            display.NIXIE_VAL_3 = alarmSlot.min / 10;
            display.NIXIE_VAL_4 = alarmSlot.min % 10;
            break;
            
        case e_VIEW_SET_ALARM_DAY:
            display.NEON_MO = (activeDay == 0) ? ((alarmSlot.day & (1 << 0)) ? e_SEG_TGL_1 : e_SEG_TGL_0) : ((alarmSlot.day & (1 << 0)) ? e_SEG_ON : e_SEG_OFF);
            display.NEON_TU = (activeDay == 1) ? ((alarmSlot.day & (1 << 1)) ? e_SEG_TGL_1 : e_SEG_TGL_0) : ((alarmSlot.day & (1 << 1)) ? e_SEG_ON : e_SEG_OFF);
            display.NEON_WE = (activeDay == 2) ? ((alarmSlot.day & (1 << 2)) ? e_SEG_TGL_1 : e_SEG_TGL_0) : ((alarmSlot.day & (1 << 2)) ? e_SEG_ON : e_SEG_OFF);
            display.NEON_TH = (activeDay == 3) ? ((alarmSlot.day & (1 << 3)) ? e_SEG_TGL_1 : e_SEG_TGL_0) : ((alarmSlot.day & (1 << 3)) ? e_SEG_ON : e_SEG_OFF);
            display.NEON_FR = (activeDay == 4) ? ((alarmSlot.day & (1 << 4)) ? e_SEG_TGL_1 : e_SEG_TGL_0) : ((alarmSlot.day & (1 << 4)) ? e_SEG_ON : e_SEG_OFF);
            display.NEON_SA = (activeDay == 5) ? ((alarmSlot.day & (1 << 5)) ? e_SEG_TGL_1 : e_SEG_TGL_0) : ((alarmSlot.day & (1 << 5)) ? e_SEG_ON : e_SEG_OFF);
            display.NEON_SU = (activeDay == 6) ? ((alarmSlot.day & (1 << 6)) ? e_SEG_TGL_1 : e_SEG_TGL_0) : ((alarmSlot.day & (1 << 6)) ? e_SEG_ON : e_SEG_OFF);
            
            display.NEON_AL1 = (activeAlarmSot == 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL2 = (activeAlarmSot == 1) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL3 = (activeAlarmSot == 2) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL4 = (activeAlarmSot == 3) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SET = e_SEG_ON;
            display.NEON_ALM = e_SEG_ON;
            display.NEON_TME = e_SEG_OFF;
            display.NEON_DP  = e_SEG_TGL;

            display.NIXIE_1 = e_SEG_ON;
            display.NIXIE_2 = e_SEG_ON;
            display.NIXIE_3 = e_SEG_ON;
            display.NIXIE_4 = e_SEG_ON;
            display.NIXIE_VAL_1 = alarmSlot.hour / 10;
            display.NIXIE_VAL_2 = alarmSlot.hour % 10;
            display.NIXIE_VAL_3 = alarmSlot.min / 10;
            display.NIXIE_VAL_4 = alarmSlot.min % 10;
            break;
    }
    
    return e_VIEW_SET_ALARM;
}
