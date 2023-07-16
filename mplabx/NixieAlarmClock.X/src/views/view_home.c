#include "views.h"
#include "tasks/display.h"
#include "tasks/buttons.h"
#include "tasks/alarm.h"
#include "tasks/time.h"
#include "hardware/player.h"
#include "logging/logging.h"
#include "hardware/PAM8407.h"


typedef enum {e_VIEW_HOME_IDLE, e_VIEW_HOME_SET_ALARM, e_VIEW_HOME_SET_TIME} ViewHomeState_t;
const char* viewHomeStateStr[] = {"VIEW_HOME_IDLE", "VIEW_HOME_SET_ALARM", "VIEW_HOME_SET_TIME"};
static ViewHomeState_t state;


void HomeInit(void)
{
    state = e_VIEW_HOME_IDLE;
    
    LOG_TRACE1("View Init: Home - State: %s\n", viewHomeStateStr[state]);
}


View_t HomeUpdate(void)
{
    ViewHomeState_t nextState = state;

    
    // Handle inputs
    switch (state)
    {
        case e_VIEW_HOME_IDLE:
            if (BTN_SET_GetState() == e_BTN_LONG_PRESS)
            {
                PLAYER_Stop();                      // Stop any songs playing
                nextState = e_VIEW_HOME_SET_ALARM;  // Change sate
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                if (PLAYER_GetState() == e_PLAYER_BUSY)
                    PLAYER_Stop();
            }
            else if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)
            {
                /*if (PLAYER_GetState() == e_PLAYER_BUSY)
                {
                    settings->volume = PAM8407_VolumeUp();
                }*/
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS)
            {
                /*if (PLAYER_GetState() == e_PLAYER_BUSY)
                {
                    settings->volume = PAM8407_VolumeDown();
                }*/
            }
            else if (BTN_PLAY_GetState() == e_BTN_SHORT_PRESS) // Start playing a song or change song
            {
                if (PLAYER_GetState() == e_PLAYER_BUSY)
                    PLAYER_Stop();

                //PLAYER_Play(settings->volume);
            }
            else if (flagTriggerAlarm)  // Start playing a song if not already
            {
                /*if (PLAYER_GetState() == e_PLAYER_IDLE)
                    PLAYER_Play(settings->volume);*/
                
                // Clear alarm trigger flag
                flagTriggerAlarm = false;
            }
            break;
            
        case e_VIEW_HOME_SET_ALARM:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)
            {
                nextState = e_VIEW_HOME_SET_TIME;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                return e_VIEW_SET_ALARM;
            }
            break;
            
        case e_VIEW_HOME_SET_TIME:
            if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS)
            {
                nextState = e_VIEW_HOME_SET_ALARM;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                return e_VIEW_SET_TIME;
            }
            break;
            
        default:
            nextState = e_VIEW_HOME_IDLE;
    }
    
    if (state != nextState)
        LOG_TRACE1("View Update: Home - State: %s\n", viewHomeStateStr[nextState]);
        
    state = nextState;
    
    
    // Update screen
    display.NEON_MO = (rtcTime.tm_wday == 1) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_TU = (rtcTime.tm_wday == 2) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_WE = (rtcTime.tm_wday == 3) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_TH = (rtcTime.tm_wday == 4) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_FR = (rtcTime.tm_wday == 5) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_SA = (rtcTime.tm_wday == 6) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_SU = (rtcTime.tm_wday == 7) ? e_SEG_ON : e_SEG_OFF;

    display.NEON_AL1 = (alarmSlots[0].day != 0) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_AL2 = (alarmSlots[1].day != 0) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_AL3 = (alarmSlots[2].day != 0) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_AL4 = (alarmSlots[3].day != 0) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_SET = (state != e_VIEW_HOME_IDLE)       ? e_SEG_ON : e_SEG_OFF;
    display.NEON_ALM = (state == e_VIEW_HOME_SET_ALARM)  ? e_SEG_TGL : e_SEG_OFF;
    display.NEON_TME = (state == e_VIEW_HOME_SET_TIME)   ? e_SEG_TGL : e_SEG_OFF;
    display.NEON_DP  = e_SEG_TGL;

    display.NIXIE_1 = e_SEG_ON;
    display.NIXIE_2 = e_SEG_ON;
    display.NIXIE_3 = e_SEG_ON;
    display.NIXIE_4 = e_SEG_ON;
    display.NIXIE_VAL_1 = rtcTime.tm_hour >> 4;
    display.NIXIE_VAL_2 = rtcTime.tm_hour & 0x0f;
    display.NIXIE_VAL_3 = rtcTime.tm_min >> 4;
    display.NIXIE_VAL_4 = rtcTime.tm_min & 0x0f;
    
    return e_VIEW_HOME;
}