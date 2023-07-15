

#include "taskmanager.h"
#include "task_args.h"
#include "hardware/display.h"
#include "tasks/buttons.h"
#include "settings.h"
#include "alarm.h"
#include "rtcc.h"
#include "hardware/player.h"
#include "logging/logging.h"



static void Initialize(void*);
static void Update(void*);

Task_t task_set_time = {false, Initialize, Update};

typedef enum {e_TASK_SET_TIME_HOUR, e_TASK_SET_TIME_MIN, e_TASK_SET_TIME_DAY} TaskSetTimeState_t;
static TaskSetTimeState_t state;
static bcdTime_t currentTimeBCD;


static uint8_t IncValueBCD(uint8_t val)
{
    uint8_t un = val & 0x0f;
    uint8_t dc = val >> 4;
    
    
    
    if (un == 9)
    {
        un = 0;
        dc ++;
    }
    else
        un ++;
    
    return (dc << 4) | un;
}

static uint8_t DecValueBCD(uint8_t val)
{
    uint8_t un = val & 0x0f;
    uint8_t dc = val >> 4;
    
    if (un == 0)
    {
        un = 9;
        dc --;
    }
    else
        un --;
    
    return (dc << 4) | un;
}


static void Initialize(void* arg)
{  
    RTCC_BCDTimeGet(&currentTimeBCD);
    state = e_TASK_SET_TIME_HOUR;
}



static void Update(void* arg)
{
    Settings_t* settings = (Settings_t*)arg;
    TaskSetTimeState_t nextState = state;

    
    // Handle inputs
    switch(state)
    {
        case e_TASK_SET_TIME_HOUR:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)        // Increment hours
            {
                LOG_TRACE1("Task: SetTime - State: SET_TIME_HOUR - Input: BTN_UP<SHORT_PRESS>\n");
                if (currentTimeBCD.tm_hour < 0x23)
                    currentTimeBCD.tm_hour = IncValueBCD(currentTimeBCD.tm_hour);
                else
                    currentTimeBCD.tm_hour = 0;
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS) // Decrement hours
            {
                LOG_TRACE1("Task: SetTime - State: SET_TIME_HOUR - Input: BTN_DOWN<SHORT_PRESS>\n");
                if (currentTimeBCD.tm_hour > 0)
                    currentTimeBCD.tm_hour = DecValueBCD(currentTimeBCD.tm_hour);
                else
                    currentTimeBCD.tm_hour = 0x23;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("Task: SetTime - State: SET_TIME_HOUR - Input: BTN_SET<SHORT_PRESS>\n");
                nextState = e_TASK_SET_TIME_MIN;
            }
            break;
            
        case e_TASK_SET_TIME_MIN:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)        // Increment minutes
            {
                LOG_TRACE1("Task: SetTime - State: SET_TIME_MIN - Input: BTN_UP<SHORT_PRESS>\n");
                if (currentTimeBCD.tm_min < 0x59)
                    currentTimeBCD.tm_min = IncValueBCD(currentTimeBCD.tm_min);
                else
                    currentTimeBCD.tm_min = 0;
                
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS) // Decrement minutes
            {
                LOG_TRACE1("Task: SetTime - State: SET_TIME_MIN - Input: BTN_DOWN<SHORT_PRESS>\n");
                if (currentTimeBCD.tm_min > 0)
                    currentTimeBCD.tm_min = DecValueBCD(currentTimeBCD.tm_min);
                else
                    currentTimeBCD.tm_min = 0x59;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("Task: SetTime - State: SET_TIME_MIN - Input: BTN_SET<SHORT_PRESS>\n");
                nextState = e_TASK_SET_TIME_DAY;
            }
            break;
            
        case e_TASK_SET_TIME_DAY:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)        // Increment day
            {
                LOG_TRACE1("Task: SetTime - State: SET_TIME_DAY - Input: BTN_UP<SHORT_PRESS>\n");
                if ((++ currentTimeBCD.tm_wday) > 7)
                    currentTimeBCD.tm_wday = 1;
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS) // Decrement day
            {
                LOG_TRACE1("Task: SetTime - State: SET_TIME_DAY - Input: BTN_DOWN<SHORT_PRESS>\n");
                if ((-- currentTimeBCD.tm_wday) <= 0)
                    currentTimeBCD.tm_wday = 7;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("Task: SetTime - State: SET_TIME_DAY - Input: BTN_SET<SHORT_PRESS>\n");
                currentTimeBCD.tm_sec = 0;
                RTCC_BCDTimeSet(&currentTimeBCD);
                APP_PopTask();
                nextState = e_TASK_SET_TIME_HOUR;
            }
            break;
            
        default:
            nextState = e_TASK_SET_TIME_HOUR;
    }
    
    state = nextState;
    
    
    
    // Update display
    switch(state)
    {
        case e_TASK_SET_TIME_HOUR:
            display.NEON_MO = (currentTimeBCD.tm_wday == 1) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TU = (currentTimeBCD.tm_wday == 2) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_WE = (currentTimeBCD.tm_wday == 3) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TH = (currentTimeBCD.tm_wday == 4) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_FR = (currentTimeBCD.tm_wday == 5) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SA = (currentTimeBCD.tm_wday == 6) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SU = (currentTimeBCD.tm_wday == 7) ? e_SEG_ON : e_SEG_OFF;

            display.NEON_AL1 = (settings->alarmSlots[0].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL2 = (settings->alarmSlots[1].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL3 = (settings->alarmSlots[2].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL4 = (settings->alarmSlots[3].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SET = e_SEG_ON;
            display.NEON_ALM = e_SEG_OFF;
            display.NEON_TME = e_SEG_ON;
            display.NEON_DP  = e_SEG_TGL;

            display.NIXIE_1 = e_SEG_TGL;
            display.NIXIE_2 = e_SEG_TGL;
            display.NIXIE_3 = e_SEG_ON;
            display.NIXIE_4 = e_SEG_ON;
            display.NIXIE_VAL_1 = currentTimeBCD.tm_hour >> 4;
            display.NIXIE_VAL_2 = currentTimeBCD.tm_hour & 0x0f;
            display.NIXIE_VAL_3 = currentTimeBCD.tm_min >> 4;
            display.NIXIE_VAL_4 = currentTimeBCD.tm_min & 0x0f;
            break;
            
        case e_TASK_SET_TIME_MIN:
            display.NEON_MO = (currentTimeBCD.tm_wday == 1) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TU = (currentTimeBCD.tm_wday == 2) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_WE = (currentTimeBCD.tm_wday == 3) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TH = (currentTimeBCD.tm_wday == 4) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_FR = (currentTimeBCD.tm_wday == 5) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SA = (currentTimeBCD.tm_wday == 6) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SU = (currentTimeBCD.tm_wday == 7) ? e_SEG_ON : e_SEG_OFF;

            display.NEON_AL1 = (settings->alarmSlots[0].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL2 = (settings->alarmSlots[1].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL3 = (settings->alarmSlots[2].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL4 = (settings->alarmSlots[3].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SET = e_SEG_ON;
            display.NEON_ALM = e_SEG_OFF;
            display.NEON_TME = e_SEG_ON;
            display.NEON_DP  = e_SEG_TGL;

            display.NIXIE_1 = e_SEG_ON;
            display.NIXIE_2 = e_SEG_ON;
            display.NIXIE_3 = e_SEG_TGL;
            display.NIXIE_4 = e_SEG_TGL;
            display.NIXIE_VAL_1 = currentTimeBCD.tm_hour >> 4;
            display.NIXIE_VAL_2 = currentTimeBCD.tm_hour & 0x0f;
            display.NIXIE_VAL_3 = currentTimeBCD.tm_min >> 4;
            display.NIXIE_VAL_4 = currentTimeBCD.tm_min & 0x0f;
            break;
            
        case e_TASK_SET_TIME_DAY:
            display.NEON_MO = (currentTimeBCD.tm_wday == 1) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_TU = (currentTimeBCD.tm_wday == 2) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_WE = (currentTimeBCD.tm_wday == 3) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_TH = (currentTimeBCD.tm_wday == 4) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_FR = (currentTimeBCD.tm_wday == 5) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_SA = (currentTimeBCD.tm_wday == 6) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_SU = (currentTimeBCD.tm_wday == 7) ? e_SEG_TGL_1 : e_SEG_TGL_0;

            display.NEON_AL1 = (settings->alarmSlots[0].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL2 = (settings->alarmSlots[1].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL3 = (settings->alarmSlots[2].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL4 = (settings->alarmSlots[3].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SET = e_SEG_ON;
            display.NEON_ALM = e_SEG_OFF;
            display.NEON_TME = e_SEG_ON;
            display.NEON_DP  = e_SEG_TGL;

            display.NIXIE_1 = e_SEG_ON;
            display.NIXIE_2 = e_SEG_ON;
            display.NIXIE_3 = e_SEG_ON;
            display.NIXIE_4 = e_SEG_ON;
            display.NIXIE_VAL_1 = currentTimeBCD.tm_hour >> 4;
            display.NIXIE_VAL_2 = currentTimeBCD.tm_hour & 0x0f;
            display.NIXIE_VAL_3 = currentTimeBCD.tm_min >> 4;
            display.NIXIE_VAL_4 = currentTimeBCD.tm_min & 0x0f;
             break;
    }
}
