

#include "taskmanager.h"
#include "task_args.h"
#include "hardware/display.h"
#include "tasks/buttons.h"
#include "settings.h"
#include "alarm.h"
#include "rtcc.h"
#include "hardware/player.h"
#include "logging/logging.h"
#include "hardware/PAM8407.h"



static void Initialize(void*);
static void Update(void*);

Task_t task_home = {false, Initialize, Update};

typedef enum {e_TASK_HOME_IDLE, e_TASK_HOME_SET_ALARM, e_TASK_HOME_SET_TIME} TaskHomeState_t;
static TaskHomeState_t state;


static void Initialize(void* arg)
{
    LOG_TRACE2("Task Home -> Initialize()\n");
    
    
    state = e_TASK_HOME_IDLE;
}



static void Update(void* arg)
{
    Settings_t* settings = (Settings_t*)arg;
    TaskHomeState_t nextState = state;
    bcdTime_t bcd_time;
    
    RTCC_BCDTimeGet(&bcd_time);
    
    // Check alarm
    bool ringAlarm = ALARM_Check(&bcd_time, settings->alarmSlots);
    

    // Handle inputs
    switch (state)
    {
        case e_TASK_HOME_IDLE:
            if (BTN_SET_GetState() == e_BTN_LONG_PRESS)
            {
                LOG_TRACE1("Task: Home - State: IDLE - Input: BTN_SET<LONG_PRESS>\n");
                PLAYER_Stop();                      // Stop any songs playing
                nextState = e_TASK_HOME_SET_ALARM;  // Change sate
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("Task: Home - State: IDLE - Input: BTN_SET<SHORT_PRESS>\n");
                if (PLAYER_GetState() == e_PLAYER_BUSY)
                    PLAYER_Stop();
            }
            else if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("Task: Home - State: IDLE - Input: BTN_UP<SHORT_PRESS>\n");
                if (PLAYER_GetState() == e_PLAYER_BUSY)
                {
                    settings->volume = PAM8407_VolumeUp();
                }
            }
            else if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("Task: Home - State: IDLE - Input: BTN_DOWN<SHORT_PRESS>\n");
                if (PLAYER_GetState() == e_PLAYER_BUSY)
                {
                    settings->volume = PAM8407_VolumeDown();
                }
            }
            else if (BTN_PLAY_GetState() == e_BTN_SHORT_PRESS) // Start playing a song or change song
            {
                LOG_TRACE1("Task: Home - State: IDLE - Input: BTN_PLAY<SHORT_PRESS>\n");
                if (PLAYER_GetState() == e_PLAYER_BUSY)
                    PLAYER_Stop();

                PLAYER_Play(settings->volume);
            }
            else if (ringAlarm)  // Start playing a song if not already
            {
                LOG_TRACE1("Task: Home - State: IDLE - Action: ringAlarm\n");
                if (PLAYER_GetState() == e_PLAYER_IDLE)
                    PLAYER_Play(settings->volume);
            }
            break;
            
        case e_TASK_HOME_SET_ALARM:
            if (BTN_UP_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("Task: Home - State: SET_ALARM - Input: BTN_UP<SHORT_PRESS>\n");
                nextState = e_TASK_HOME_SET_TIME;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("Task: Home - State: SET_ALARM - Input: BTN_SET<SHORT_PRESS>\n");
                APP_PushTask(&task_set_alarm, arg); // Set alarm
                nextState = e_TASK_HOME_IDLE;
            }
            break;
            
        case e_TASK_HOME_SET_TIME:
            if (BTN_DOWN_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("Task: Home - State: SET_TIME - Input: BTN_DOWN<SHORT_PRESS>\n");
                nextState = e_TASK_HOME_SET_ALARM;
            }
            else if (BTN_SET_GetState() == e_BTN_SHORT_PRESS)
            {
                LOG_TRACE1("Task: Home - State: SET_ALARM - Input: BTN_SET<SHORT_PRESS>\n");
                APP_PushTask(&task_set_time, arg); // Set time
                nextState = e_TASK_HOME_IDLE;
            }
            break;
            
        default:
            nextState = e_TASK_HOME_IDLE;
    }
    
    state = nextState;
    
    
    // Update screen
    display.NEON_MO = (bcd_time.tm_wday == 1) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_TU = (bcd_time.tm_wday == 2) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_WE = (bcd_time.tm_wday == 3) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_TH = (bcd_time.tm_wday == 4) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_FR = (bcd_time.tm_wday == 5) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_SA = (bcd_time.tm_wday == 6) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_SU = (bcd_time.tm_wday == 7) ? e_SEG_ON : e_SEG_OFF;

    display.NEON_AL1 = (settings->alarmSlots[0].day != 0) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_AL2 = (settings->alarmSlots[1].day != 0) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_AL3 = (settings->alarmSlots[2].day != 0) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_AL4 = (settings->alarmSlots[3].day != 0) ? e_SEG_ON : e_SEG_OFF;
    display.NEON_SET = (state != e_TASK_HOME_IDLE)       ? e_SEG_ON : e_SEG_OFF;
    display.NEON_ALM = (state == e_TASK_HOME_SET_ALARM)  ? e_SEG_TGL : e_SEG_OFF;
    display.NEON_TME = (state == e_TASK_HOME_SET_TIME)   ? e_SEG_TGL : e_SEG_OFF;
    display.NEON_DP  = e_SEG_TGL;

    display.NIXIE_1 = e_SEG_ON;
    display.NIXIE_2 = e_SEG_ON;
    display.NIXIE_3 = e_SEG_ON;
    display.NIXIE_4 = e_SEG_ON;
    display.NIXIE_VAL_1 = bcd_time.tm_hour >> 4;
    display.NIXIE_VAL_2 = bcd_time.tm_hour & 0x0f;
    display.NIXIE_VAL_3 = bcd_time.tm_min >> 4;
    display.NIXIE_VAL_4 = bcd_time.tm_min & 0x0f;
}