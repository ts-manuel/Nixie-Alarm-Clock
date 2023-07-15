

#include "taskmanager.h"
#include "task_args.h"
#include "hardware/display.h"
#include "input/input.h"
#include "settings.h"
#include "alarm.h"
#include "rtcc.h"
#include "hardware/player.h"
#include "logging/logging.h"



static void Initialize(void*);
static void Update(void*);

Task_t task_set_alarm = {false, Initialize, Update};

typedef enum {e_TASK_SET_ALARM_SLOT, e_TASK_SET_ALARM_HOUR, e_TASK_SET_ALARM_MIN, e_TASK_SET_ALARM_DAY} TaskSetAlarmState_t;
static TaskSetAlarmState_t state;

static int8_t activeAlarmSot;
static int8_t activeDay;
static AlarmSlot_t alarmSlot;


static void Initialize(void* arg)
{
    activeAlarmSot = 0;
    activeDay = 0;
    state = e_TASK_SET_ALARM_SLOT;
}



static void Update(void* arg)
{
    Settings_t* settings = (Settings_t*)arg;
    TaskSetAlarmState_t nextState = state;
    bcdTime_t bcd_time;
    
    RTCC_BCDTimeGet(&bcd_time);
    
    
    // Handle inputs
    switch(state)
    {
        case e_TASK_SET_ALARM_SLOT:
            if (INPUT_SHORT_PRESS(_IN_BTN_UP))        // Increment day
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_SLOT - Input: BTN_UP<SHORT_PRESS>\n");
                if ((++ activeAlarmSot) >= _AL_SLOT_COUNT)
                    activeAlarmSot = 0;
            }
            else if (INPUT_SHORT_PRESS(_IN_BTN_DOWN)) // Decrement day
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_SLOT - Input: BTN_UP<SHORT_PRESS>\n");
                if ((-- activeAlarmSot) < 0)
                    activeAlarmSot = _AL_SLOT_COUNT - 1;
            }
            else if (INPUT_SHORT_PRESS(_IN_BTN_SET))
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_SLOT - Input: BTN_SET<SHORT_PRESS>\n");
                alarmSlot = settings->alarmSlots[activeAlarmSot];
                nextState = e_TASK_SET_ALARM_HOUR;
            }
            break;
            
        case e_TASK_SET_ALARM_HOUR:
            if (INPUT_SHORT_PRESS(_IN_BTN_UP))        // Increment hours
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_HOUR - Input: BTN_UP<SHORT_PRESS>\n");
                if ((++ alarmSlot.hour) >= 24)
                    alarmSlot.hour = 0;
            }
            else if (INPUT_SHORT_PRESS(_IN_BTN_DOWN)) // Decrement hours
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_HOUR - Input: BTN_DOWN<SHORT_PRESS>\n");
                if ((-- alarmSlot.hour) < 0)
                    alarmSlot.hour = 23;
            }
            else if (INPUT_SHORT_PRESS(_IN_BTN_SET))
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_HOUR - Input: BTN_SET<SHORT_PRESS>\n");
                nextState = e_TASK_SET_ALARM_MIN;
            }
            break;
            
        case e_TASK_SET_ALARM_MIN:
            if (INPUT_SHORT_PRESS(_IN_BTN_UP))        // Increment minutes
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_MIN - Input: BTN_UP<SHORT_PRESS>\n");
                if ((++ alarmSlot.min) >= 60)
                    alarmSlot.min = 0;
            }
            else if (INPUT_SHORT_PRESS(_IN_BTN_DOWN)) // Decrement minutes
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_MIN - Input: BTN_DOWN<SHORT_PRESS>\n");
                if ((-- alarmSlot.min) < 0)
                    alarmSlot.min = 59;
            }
            else if (INPUT_SHORT_PRESS(_IN_BTN_SET))
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_MIN - Input: BTN_SET<SHORT_PRESS>\n");
                nextState = e_TASK_SET_ALARM_DAY;
            }
            break;
            
        case e_TASK_SET_ALARM_DAY:
            if (INPUT_SHORT_PRESS(_IN_BTN_UP))        // Increment day
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_DAY - Input: BTN_UP<SHORT_PRESS>\n");
                alarmSlot.day |= 1 << activeDay;
            }
            else if (INPUT_SHORT_PRESS(_IN_BTN_DOWN)) // Decrement day
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_DAY - Input: BTN_DOWN<SHORT_PRESS>\n");
                alarmSlot.day &= ~(1 << activeDay);
            }
            else if (INPUT_SHORT_PRESS(_IN_BTN_SET))
            {
                LOG_TRACE1("Task: SetAlarm - State: SET_ALARM_DAY - Input: BTN_SET<SHORT_PRESS>\n");
                if ((++ activeDay) >= 7)
                {
                    // Save changes made to the alarm slot and exit task
                    settings->alarmSlots[activeAlarmSot] = alarmSlot;
                    
                    APP_PopTask();
                }
            }
            break;
            
        default:
            nextState = e_TASK_SET_ALARM_SLOT;
    }
    
    state = nextState;
    
    
 
    // Update screen
    switch(state)
    {
        case e_TASK_SET_ALARM_SLOT:
            display.NEON_MO = (bcd_time.tm_wday == 1) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TU = (bcd_time.tm_wday == 2) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_WE = (bcd_time.tm_wday == 3) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TH = (bcd_time.tm_wday == 4) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_FR = (bcd_time.tm_wday == 5) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SA = (bcd_time.tm_wday == 6) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SU = (bcd_time.tm_wday == 7) ? e_SEG_ON : e_SEG_OFF;

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
            display.NIXIE_VAL_1 = bcd_time.tm_hour >> 4;
            display.NIXIE_VAL_2 = bcd_time.tm_hour & 0x0f;
            display.NIXIE_VAL_3 = bcd_time.tm_min >> 4;
            display.NIXIE_VAL_4 = bcd_time.tm_min & 0x0f;
            break;
            
        case e_TASK_SET_ALARM_HOUR:
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
            
        case e_TASK_SET_ALARM_MIN:
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
            
        case e_TASK_SET_ALARM_DAY:
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
}