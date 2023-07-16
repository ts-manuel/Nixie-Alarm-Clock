#include "views.h"
#include "tasks/display.h"
#include "tasks/buttons.h"
#include "tasks/alarm.h"
#include "tasks/time.h"
#include "rtcc.h"
#include "hardware/player.h"
#include "logging/logging.h"
#include "hardware/PAM8407.h"
#include "tasks/time.h"


typedef enum {e_VIEW_SET_TIME_HOUR, e_VIEW_SET_TIME_MIN, e_VIEW_SET_TIME_DAY} ViewSetTimeState_t;
const char* viewSetTimeStateStr[] = {"VIEW_SET_TIME_HOUR", "VIEW_SET_TIME_MIN", "VIEW_SET_TIME_DAY"};
static ViewSetTimeState_t state;
static bcdTime_t rtcNewTime;


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


void SetTimeInit(void)
{  
    // Freeze the start time so that it doesn't change while it is modified
    rtcNewTime = rtcTime;
    state = e_VIEW_SET_TIME_HOUR;
    
    LOG_TRACE1("View Init: SetTime - State: %s\n", viewSetTimeStateStr[state]);
}


View_t SetTimeUpdate(void)
{
    ViewSetTimeState_t nextState = state;

    // Handle inputs
    switch(state)
    {
        case e_VIEW_SET_TIME_HOUR:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)        // Increment hours
            {
                LOG_TRACE1("View: SetTime - State: SET_TIME_HOUR - Input: BTN_UP<SHORT_PRESS>\n");
                if (rtcNewTime.tm_hour < 0x23)
                    rtcNewTime.tm_hour = IncValueBCD(rtcNewTime.tm_hour);
                else
                    rtcNewTime.tm_hour = 0;
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS) // Decrement hours
            {
                LOG_TRACE1("View: SetTime - State: SET_TIME_HOUR - Input: BTN_DOWN<SHORT_PRESS>\n");
                if (rtcNewTime.tm_hour > 0)
                    rtcNewTime.tm_hour = DecValueBCD(rtcNewTime.tm_hour);
                else
                    rtcNewTime.tm_hour = 0x23;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("View: SetTime - State: SET_TIME_HOUR - Input: BTN_SET<SHORT_PRESS>\n");
                nextState = e_VIEW_SET_TIME_MIN;
            }
            break;
            
        case e_VIEW_SET_TIME_MIN:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)        // Increment minutes
            {
                LOG_TRACE1("View: SetTime - State: SET_TIME_MIN - Input: BTN_UP<SHORT_PRESS>\n");
                if (rtcNewTime.tm_min < 0x59)
                    rtcNewTime.tm_min = IncValueBCD(rtcNewTime.tm_min);
                else
                    rtcNewTime.tm_min = 0;
                
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS) // Decrement minutes
            {
                LOG_TRACE1("View: SetTime - State: SET_TIME_MIN - Input: BTN_DOWN<SHORT_PRESS>\n");
                if (rtcNewTime.tm_min > 0)
                    rtcNewTime.tm_min = DecValueBCD(rtcNewTime.tm_min);
                else
                    rtcNewTime.tm_min = 0x59;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("View: SetTime - State: SET_TIME_MIN - Input: BTN_SET<SHORT_PRESS>\n");
                nextState = e_VIEW_SET_TIME_DAY;
            }
            break;
            
        case e_VIEW_SET_TIME_DAY:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)        // Increment day
            {
                LOG_TRACE1("View: SetTime - State: SET_TIME_DAY - Input: BTN_UP<SHORT_PRESS>\n");
                if ((++ rtcNewTime.tm_wday) > 7)
                    rtcNewTime.tm_wday = 1;
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS) // Decrement day
            {
                LOG_TRACE1("View: SetTime - State: SET_TIME_DAY - Input: BTN_DOWN<SHORT_PRESS>\n");
                if ((-- rtcNewTime.tm_wday) <= 0)
                    rtcNewTime.tm_wday = 7;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("View: SetTime - State: SET_TIME_DAY - Input: BTN_SET<SHORT_PRESS>\n");
                rtcNewTime.tm_sec = 0;
                RTCC_BCDTimeSet(&rtcNewTime);
                return e_VIEW_HOME;
            }
            break;
            
        default:
            nextState = e_VIEW_SET_TIME_HOUR;
    }
    
    if (state != nextState)
        LOG_TRACE1("View Update: SetTime - State: %s\n", viewSetTimeStateStr[nextState]);
    
    state = nextState;
    
    
    // Update display
    switch(state)
    {
        case e_VIEW_SET_TIME_HOUR:
            display.NEON_MO = (rtcNewTime.tm_wday == 1) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TU = (rtcNewTime.tm_wday == 2) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_WE = (rtcNewTime.tm_wday == 3) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TH = (rtcNewTime.tm_wday == 4) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_FR = (rtcNewTime.tm_wday == 5) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SA = (rtcNewTime.tm_wday == 6) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SU = (rtcNewTime.tm_wday == 7) ? e_SEG_ON : e_SEG_OFF;

            display.NEON_AL1 = (alarmSlots[0].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL2 = (alarmSlots[1].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL3 = (alarmSlots[2].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL4 = (alarmSlots[3].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SET = e_SEG_ON;
            display.NEON_ALM = e_SEG_OFF;
            display.NEON_TME = e_SEG_ON;
            display.NEON_DP  = e_SEG_TGL;

            display.NIXIE_1 = e_SEG_TGL;
            display.NIXIE_2 = e_SEG_TGL;
            display.NIXIE_3 = e_SEG_ON;
            display.NIXIE_4 = e_SEG_ON;
            display.NIXIE_VAL_1 = rtcNewTime.tm_hour >> 4;
            display.NIXIE_VAL_2 = rtcNewTime.tm_hour & 0x0f;
            display.NIXIE_VAL_3 = rtcNewTime.tm_min >> 4;
            display.NIXIE_VAL_4 = rtcNewTime.tm_min & 0x0f;
            break;
            
        case e_VIEW_SET_TIME_MIN:
            display.NEON_MO = (rtcNewTime.tm_wday == 1) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TU = (rtcNewTime.tm_wday == 2) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_WE = (rtcNewTime.tm_wday == 3) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_TH = (rtcNewTime.tm_wday == 4) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_FR = (rtcNewTime.tm_wday == 5) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SA = (rtcNewTime.tm_wday == 6) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SU = (rtcNewTime.tm_wday == 7) ? e_SEG_ON : e_SEG_OFF;

            display.NEON_AL1 = (alarmSlots[0].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL2 = (alarmSlots[1].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL3 = (alarmSlots[2].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL4 = (alarmSlots[3].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SET = e_SEG_ON;
            display.NEON_ALM = e_SEG_OFF;
            display.NEON_TME = e_SEG_ON;
            display.NEON_DP  = e_SEG_TGL;

            display.NIXIE_1 = e_SEG_ON;
            display.NIXIE_2 = e_SEG_ON;
            display.NIXIE_3 = e_SEG_TGL;
            display.NIXIE_4 = e_SEG_TGL;
            display.NIXIE_VAL_1 = rtcNewTime.tm_hour >> 4;
            display.NIXIE_VAL_2 = rtcNewTime.tm_hour & 0x0f;
            display.NIXIE_VAL_3 = rtcNewTime.tm_min >> 4;
            display.NIXIE_VAL_4 = rtcNewTime.tm_min & 0x0f;
            break;
            
        case e_VIEW_SET_TIME_DAY:
            display.NEON_MO = (rtcNewTime.tm_wday == 1) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_TU = (rtcNewTime.tm_wday == 2) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_WE = (rtcNewTime.tm_wday == 3) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_TH = (rtcNewTime.tm_wday == 4) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_FR = (rtcNewTime.tm_wday == 5) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_SA = (rtcNewTime.tm_wday == 6) ? e_SEG_TGL_1 : e_SEG_TGL_0;
            display.NEON_SU = (rtcNewTime.tm_wday == 7) ? e_SEG_TGL_1 : e_SEG_TGL_0;

            display.NEON_AL1 = (alarmSlots[0].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL2 = (alarmSlots[1].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL3 = (alarmSlots[2].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_AL4 = (alarmSlots[3].day != 0) ? e_SEG_ON : e_SEG_OFF;
            display.NEON_SET = e_SEG_ON;
            display.NEON_ALM = e_SEG_OFF;
            display.NEON_TME = e_SEG_ON;
            display.NEON_DP  = e_SEG_TGL;

            display.NIXIE_1 = e_SEG_ON;
            display.NIXIE_2 = e_SEG_ON;
            display.NIXIE_3 = e_SEG_ON;
            display.NIXIE_4 = e_SEG_ON;
            display.NIXIE_VAL_1 = rtcNewTime.tm_hour >> 4;
            display.NIXIE_VAL_2 = rtcNewTime.tm_hour & 0x0f;
            display.NIXIE_VAL_3 = rtcNewTime.tm_min >> 4;
            display.NIXIE_VAL_4 = rtcNewTime.tm_min & 0x0f;
             break;
    }
    
    return e_VIEW_SET_TIME;
}

