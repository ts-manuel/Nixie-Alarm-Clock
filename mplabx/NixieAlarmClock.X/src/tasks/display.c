/* 
 * File:   display.h
 * Author: ts-manuel
 *
 * Display refresh task
 */

#include "system.h"
#include "pin_manager.h"
#include "config.h"
#include "utils/delay.h"
#include "utils/millis.h"
#include "display.h"


typedef struct {
    uint8_t NEON_MO;
    uint8_t NEON_TU;
    uint8_t NEON_WE;
    uint8_t NEON_TH;
    uint8_t NEON_FR;
    uint8_t NEON_SA;
    uint8_t NEON_SU;
    
    uint8_t NEON_AL4;
    uint8_t NEON_AL3;
    uint8_t NEON_AL2;
    uint8_t NEON_AL1;
    uint8_t NEON_SET;
    uint8_t NEON_ALM;
    uint8_t NEON_TME;
    uint8_t NEON_DP;
    
    uint8_t HV_EN;
    uint8_t NIXIE_1;
    uint8_t NIXIE_2;
    uint8_t NIXIE_3;
    uint8_t NIXIE_4;
} DispState_t;


static void ShiftDisplayState(DispState_t* d);
static void ShiftData(uint8_t* data, uint8_t len);

static DispState_t Disp;


/**
 * Initialization of the display
 * 
 */
void DISPLAY_Initialize(void)
{
    display.NEON_MO = e_SEG_ON;
    display.NEON_TU = e_SEG_OFF;
    display.NEON_WE = e_SEG_OFF;
    display.NEON_TH = e_SEG_OFF;
    display.NEON_FR = e_SEG_OFF;
    display.NEON_SA = e_SEG_OFF;
    display.NEON_SU = e_SEG_OFF;
    
    display.NEON_AL4 = e_SEG_OFF;
    display.NEON_AL3 = e_SEG_OFF;
    display.NEON_AL2 = e_SEG_OFF;
    display.NEON_AL1 = e_SEG_OFF;
    display.NEON_SET = e_SEG_OFF;
    display.NEON_ALM = e_SEG_OFF;
    display.NEON_TME = e_SEG_OFF;
    display.NEON_DP = e_SEG_TGL;
    
    display.NIXIE_1 = e_SEG_ON;
    display.NIXIE_2 = e_SEG_ON;
    display.NIXIE_3 = e_SEG_ON;
    display.NIXIE_4 = e_SEG_ON;
    display.NIXIE_VAL_1 = 0;
    display.NIXIE_VAL_2 = 0;
    display.NIXIE_VAL_3 = 0;
    display.NIXIE_VAL_4 = 0;
}


/*
 * Called in the main loop updates the state of the display
 * 
 */
void DISPLAY_Update(void)
{
    static uint32_t LastTogleMs = 0;
    //static uint8_t toggleValue = 0;
    uint32_t millis = TIME_Millis();
    uint8_t state[e_SEG_TGL_1 + 1];
    
    if (millis - LastTogleMs > _DISPLAY_TOGGLE_PERIOD_ms)
    {
        LastTogleMs = millis;
    }
    
    state[e_SEG_OFF]    = 0;
    state[e_SEG_ON]     = 1;
    state[e_SEG_TGL]    = (millis - LastTogleMs) < _DISPLAY_TOGGLE_PERIOD_ms / 2;
    state[e_SEG_TGL_0]  = (millis - LastTogleMs) < _DISPLAY_TOGGLE_PERIOD_ms / 8;
    state[e_SEG_TGL_1]  = (millis - LastTogleMs) < _DISPLAY_TOGGLE_PERIOD_ms - _DISPLAY_TOGGLE_PERIOD_ms / 8;

    
    Disp.NEON_MO = state[display.NEON_MO];
    Disp.NEON_TU = state[display.NEON_TU];
    Disp.NEON_WE = state[display.NEON_WE];
    Disp.NEON_TH = state[display.NEON_TH];
    Disp.NEON_FR = state[display.NEON_FR];
    Disp.NEON_SA = state[display.NEON_SA];
    Disp.NEON_SU = state[display.NEON_SU];
    
    Disp.NEON_AL4 = state[display.NEON_AL4];
    Disp.NEON_AL3 = state[display.NEON_AL3];
    Disp.NEON_AL2 = state[display.NEON_AL2];
    Disp.NEON_AL1 = state[display.NEON_AL1];
    Disp.NEON_SET = state[display.NEON_SET];
    Disp.NEON_ALM = state[display.NEON_ALM];
    Disp.NEON_TME = state[display.NEON_TME];
    Disp.NEON_DP  = state[display.NEON_DP];
    
    Disp.NIXIE_1 = state[display.NIXIE_1] ? display.NIXIE_VAL_1 : 10;
    Disp.NIXIE_2 = state[display.NIXIE_2] ? display.NIXIE_VAL_2 : 10;
    Disp.NIXIE_3 = state[display.NIXIE_3] ? display.NIXIE_VAL_3 : 10;
    Disp.NIXIE_4 = state[display.NIXIE_4] ? display.NIXIE_VAL_4 : 10;
    
    ShiftDisplayState(&Disp);
}


/* 
 * Turns the display switching regulator on or off
 * 
 * bool b:      true = on, false = off
 */
void DISPLAY_SetPower(bool b)
{
    Disp.HV_EN = b;
}


/* 
 * Translates the display state to a binary representation
 * and sends it to the display shift registers.
 * 
 * DispState_t* d: Data to be translated
 */
static void ShiftDisplayState(DispState_t* d)
{
    uint8_t buff[5];
    
    uint8_t plate1 = (d->NIXIE_1 % 2) ? 0 : 1;
    uint8_t plate2 = (d->NIXIE_2 % 2) ? 0 : 1;
    uint8_t plate3 = (d->NIXIE_3 % 2) ? 0 : 1;
    uint8_t plate4 = (d->NIXIE_4 % 2) ? 0 : 1;
    
    buff[4] = plate4
            | (d->NEON_MO << 1)
            | (d->NEON_TU << 2)
            | (d->NEON_WE << 3)
            | (d->NEON_TH << 4)
            | (d->NEON_FR << 5)
            | (d->NEON_SA << 6)
            | (d->NEON_SU << 7);
    
    buff[3] = d->HV_EN
            | (d->NEON_AL4 << 1)
            | (d->NEON_AL3 << 2)
            | (d->NEON_AL2 << 3)
            | (d->NEON_AL1 << 4)
            | (d->NEON_SET << 5)
            | (d->NEON_ALM << 6)
            | (d->NEON_TME << 7);
    
    buff[2] = plate3
            | (((d->NIXIE_3 == 8 || d->NIXIE_3 == 9) ? 1 : 0) << 1)
            | (((d->NIXIE_3 == 6 || d->NIXIE_3 == 7) ? 1 : 0) << 2)
            | (((d->NIXIE_4 == 6 || d->NIXIE_4 == 7) ? 1 : 0) << 3)
            | (((d->NIXIE_4 == 0 || d->NIXIE_4 == 1) ? 1 : 0) << 4)
            | (((d->NIXIE_4 == 8 || d->NIXIE_4 == 9) ? 1 : 0) << 5)
            | (((d->NIXIE_4 == 2 || d->NIXIE_4 == 3) ? 1 : 0) << 6)
            | (((d->NIXIE_4 == 4 || d->NIXIE_4 == 5) ? 1 : 0) << 7);
    
    buff[1] = plate2
            | (((d->NIXIE_2 == 0 || d->NIXIE_2 == 1) ? 1 : 0) << 1)
            | (((d->NIXIE_2 == 6 || d->NIXIE_2 == 7) ? 1 : 0) << 2)
            | (((d->NIXIE_2 == 8 || d->NIXIE_2 == 9) ? 1 : 0) << 3)
            | (((d->NEON_DP) ? 1 : 0) << 4)
            | (((d->NIXIE_3 == 2 || d->NIXIE_3 == 3) ? 1 : 0) << 5)
            | (((d->NIXIE_3 == 0 || d->NIXIE_3 == 1) ? 1 : 0) << 6)
            | (((d->NIXIE_3 == 4 || d->NIXIE_3 == 5) ? 1 : 0) << 7);
    
    buff[0] = plate1
            | (((d->NIXIE_1 == 2 || d->NIXIE_1 == 3) ? 1 : 0) << 1)
            | (((d->NIXIE_1 == 0 || d->NIXIE_1 == 1) ? 1 : 0) << 2)
            | (((d->NIXIE_1 == 4 || d->NIXIE_1 == 5) ? 1 : 0) << 3)
            | (((d->NIXIE_1 == 6 || d->NIXIE_1 == 7) ? 1 : 0) << 4)
            | (((d->NIXIE_1 == 8 || d->NIXIE_1 == 9) ? 1 : 0) << 5)
            | (((d->NIXIE_2 == 2 || d->NIXIE_2 == 3) ? 1 : 0) << 6)
            | (((d->NIXIE_2 == 4 || d->NIXIE_2 == 5) ? 1 : 0) << 7);
    
    ShiftData(buff, sizeof(buff));
}


/* 
 * Shifts data to the display
 * 
 * uint8_t* data:   Pointer to the data
 * uint8_t len:     Number of bytes to send
 */
static void ShiftData(uint8_t* data, uint8_t len)
{
    O_DSP_LATCH_SetLow();
    O_DSP_CLK_SetLow();
    TIME_delay_us(_DISPLAY_HALF_CLK_us);
    
    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (data[i] & (0x80 >> j))
                O_DSP_DATA_SetHigh();
            else
                O_DSP_DATA_SetLow();
            
            TIME_delay_us(_DISPLAY_HALF_CLK_us);
            O_DSP_CLK_SetHigh();
            TIME_delay_us(_DISPLAY_HALF_CLK_us);
            O_DSP_CLK_SetLow();
        }
    }
    
    TIME_delay_us(_DISPLAY_HALF_CLK_us);
    O_DSP_LATCH_SetHigh();
    TIME_delay_us(_DISPLAY_HALF_CLK_us);
    O_DSP_LATCH_SetLow();
    
    O_DSP_DATA_SetLow();
    O_DSP_CLK_SetLow();
}