/* 
 * File:   display.h
 * Author: ts-manuel
 *
 * Display refresh task
 */

#include <stdbool.h>
#include <stdint.h>


#ifndef _TASKS_DISPLAY_H_
#define	_TASKS_DISPLAY_H_


typedef enum {e_SEG_OFF = 0, e_SEG_ON, e_SEG_TGL, e_SEG_TGL_0, e_SEG_TGL_1} e_SegState_t;

typedef struct {
    e_SegState_t NEON_MO;
    e_SegState_t NEON_TU;
    e_SegState_t NEON_WE;
    e_SegState_t NEON_TH;
    e_SegState_t NEON_FR;
    e_SegState_t NEON_SA;
    e_SegState_t NEON_SU;
    
    e_SegState_t NEON_AL4;
    e_SegState_t NEON_AL3;
    e_SegState_t NEON_AL2;
    e_SegState_t NEON_AL1;
    e_SegState_t NEON_SET;
    e_SegState_t NEON_ALM;
    e_SegState_t NEON_TME;
    e_SegState_t NEON_DP;
    
    e_SegState_t NIXIE_1;
    e_SegState_t NIXIE_2;
    e_SegState_t NIXIE_3;
    e_SegState_t NIXIE_4;
    uint8_t NIXIE_VAL_1;
    uint8_t NIXIE_VAL_2;
    uint8_t NIXIE_VAL_3;
    uint8_t NIXIE_VAL_4;
} DispBuffer_t;

extern DispBuffer_t display;

/**
 * Initialization of the display
 * 
 */
void DISPLAY_Initialize(void);

/**
 * Called in the main loop updates the state of the display
 * 
 */
void DISPLAY_Update(void);

/**
 * Turns the display switching regulator on or off
 * 
 * bool b:      true = on, false = off
 */
void DISPLAY_SetPower(bool b);


#endif	/* _TASKS_DISPLAY_H_ */

