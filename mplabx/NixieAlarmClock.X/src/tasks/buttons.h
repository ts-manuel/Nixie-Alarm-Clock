/* 
 * File:   input.h
 * Author: ts-manuel
 *
 * FSM to handle inputs in a more structured way.
 * The input can be in one of the following states, based upon how long the
 * input remains "active":
 *  IDLE:           input in it's default state
 *  ACTIVE:         input is just become active
 *  SHORT_PRESS:    input is active for = shortPressMs
 *  SHORT_HOLD:     input is active for > shortPressMs and < longPressMs
 *  LONG_PRESS:     input is active for = longPressMs
 *  LONG_HOLD:      input is active for > longPressMs
 */

#ifndef _TASKS_BUTTONS_H_
#define	_TASKS_BUTTONS_H_

typedef enum
{
    e_BTN_IDLE,           // Input in the non active state
    e_BTN_ACTIVE,         // Input transition to the active state
    e_BTN_SHORT_PRESS,    // Input is in the active state for >= shortPressMs (flag valid for only one cycle)
    e_BTN_SHORT_HOLD,     // Input is in the active state for >= shortPressMs
    e_BTN_LONG_PRESS,     // Input is in the active state for >= longPressMs (flag valid for only one cycle)
    e_BTN_LONG_HOLD       // Input is in the active state for >= longPressMs (ex. button held down)
} BtnState_t;


/**
 * Initialize buttons with default values
 */
void BTN_Initialize();


/**
 * Read pin states and update input states
 */
void BTN_Update();


/*
 * Return the button state
 */
BtnState_t BTN_SET_GetState(void);
BtnState_t BTN_UP_GetState(void);
BtnState_t BTN_DOWN_GetState(void);
BtnState_t BTN_PLAY_GetState(void);



#endif	/* _TASKS_BUTTONS_H_ */

