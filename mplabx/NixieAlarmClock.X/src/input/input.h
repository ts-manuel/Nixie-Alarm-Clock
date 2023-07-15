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

#ifndef INPUTS_H
#define	INPUTS_H

#include <stdint.h>
#include <stdbool.h>


#define _IN_BTN_SET     0
#define _IN_BTN_UP      1
#define _IN_BTN_DOWN    2
#define _IN_BTN_PLAY    3
#define _IN_COUT        4
    

typedef enum
{
    e_INPUT_IDLE,           // Input in the non active state
    e_INPUT_ACTIVE,         // Input transition to the active state
    e_INPUT_SHORT_PRESS,    // Input is in the active state for >= shortPressMs (flag valid for only one cycle)
    e_INPUT_SHORT_HOLD,     // Input is in the active state for >= shortPressMs
    e_INPUT_LONG_PRESS,     // Input is in the active state for >= longPressMs (flag valid for only one cycle)
    e_INPUT_LONG_HOLD       // Input is in the active state for >= longPressMs (ex. button held down)
} InputState_t;


typedef struct
{
    bool active;
    InputState_t state;         // Current input state
    uint32_t activationTimeMs;
    uint32_t shortPressMs;      // Time in ms after witch to register a short press
    uint32_t longPressMs;       // Time in ms after witch to register a long press
} InState_t;


extern InState_t inputs[_IN_COUT];
    

#define INPUT_ACTIVE(x)         (inputs[(x)].state == e_INPUT_ACTIVE)
#define INPUT_SHORT_PRESS(x)    (inputs[(x)].state == e_INPUT_SHORT_PRESS)
#define INPUT_LONG_PRESS(x)     (inputs[(x)].state == e_INPUT_LONG_PRESS)
#define INPUT_LONG_HOLD(x)      (inputs[(x)].state == e_INPUT_LONG_HOLD)
    

/**
 * Initialize input array with default values
 */
void INPUTS_Initialize();
    

/**
 * Read pin states and update input states
 */
void INPUTS_Update();


/**
 * Returns a pointer to the specified input struct
 * 
 * @param   n   (uint8_t*)      Input number
 * @return      (InState_t*)    pointer to the input struct
 */
InState_t* INPUTS_Get(uint8_t n);


#endif	/* INPUTS_H */

