
#include <stdio.h>
#include "input.h"
#include "system.h"
#include "pin_manager.h"
#include "delay.h"

#include "logging/logging.h"


InState_t inputs[_IN_COUT];


/**
 * Read the state of all the inputs and update the struct
 * 
 * @param imputs
 */
static void ReadInputs()
{
    inputs[_IN_BTN_SET].active   = !I_BTN_SET_GetValue();
    inputs[_IN_BTN_UP].active    = !I_BTN_UP_GetValue();
    inputs[_IN_BTN_DOWN].active  = !I_BTN_DOWN_GetValue();
    inputs[_IN_BTN_PLAY].active  = !I_BTN_PLAY_GetValue();
}


/**
 * Initialize input array with default values
 */
void INPUTS_Initialize()
{
    inputs[_IN_BTN_SET].active = false;
    inputs[_IN_BTN_SET].state = e_INPUT_IDLE;
    inputs[_IN_BTN_SET].activationTimeMs = 0;
    inputs[_IN_BTN_SET].shortPressMs = 100;
    inputs[_IN_BTN_SET].longPressMs = 2000;

    inputs[_IN_BTN_UP].active = false;
    inputs[_IN_BTN_UP].state = e_INPUT_IDLE;
    inputs[_IN_BTN_UP].activationTimeMs = 0;
    inputs[_IN_BTN_UP].shortPressMs = 100;
    inputs[_IN_BTN_UP].longPressMs = 2000;
    
    inputs[_IN_BTN_DOWN].active = false;
    inputs[_IN_BTN_DOWN].state = e_INPUT_IDLE;
    inputs[_IN_BTN_DOWN].activationTimeMs = 0;
    inputs[_IN_BTN_DOWN].shortPressMs = 100;
    inputs[_IN_BTN_DOWN].longPressMs = 2000;
    
    inputs[_IN_BTN_PLAY].active = false;
    inputs[_IN_BTN_PLAY].state = e_INPUT_IDLE;
    inputs[_IN_BTN_PLAY].activationTimeMs = 0;
    inputs[_IN_BTN_PLAY].shortPressMs = 100;
    inputs[_IN_BTN_PLAY].longPressMs = 2000;
}


/**
 * Read pin states and update input states
 */
void INPUTS_Update()
{
    uint32_t now = Millis();
    
    // Read the state of all inputs
    ReadInputs(inputs);
    
    // Update the sate of all inputs
    for (int i = 0; i < _IN_COUT; i++)
    {
        InState_t* st = &inputs[i];
        
        switch (st->state)
        {
            case e_INPUT_IDLE:
                if (st->active)
                {
                    st->state = e_INPUT_ACTIVE;
                    st->activationTimeMs = now;
                }
                break;
                
            case e_INPUT_ACTIVE:
                if (!st->active)
                    st->state = e_INPUT_IDLE;
                else if (now - st->activationTimeMs >= st->shortPressMs)
                    st->state = e_INPUT_SHORT_PRESS;
                break;
                
            case e_INPUT_SHORT_PRESS:
                if (!st->active)
                    st->state = e_INPUT_IDLE;
                else
                    st->state = e_INPUT_SHORT_HOLD;
                break;
                
            case e_INPUT_SHORT_HOLD:
                if (!st->active)
                    st->state = e_INPUT_IDLE;
                else if (now - st->activationTimeMs >= st->longPressMs)
                    st->state = e_INPUT_LONG_PRESS;
                break;
        
            case e_INPUT_LONG_PRESS:
                if (!st->active)
                    st->state = e_INPUT_IDLE;
                else
                    st->state = e_INPUT_LONG_HOLD;
                break;
   
            case e_INPUT_LONG_HOLD:
                if (!st->active)
                    st->state = e_INPUT_IDLE;
                break;
                
            default:
                st->state = e_INPUT_IDLE;
        }
    }
}


/**
 * Returns a pointer to the specified input struct
 * 
 * @param   n   (uint8_t*)      Input number
 * @return      (InState_t*)    pointer to the input struct
 */
InState_t* INPUTS_Get(uint8_t n)
{
    return &inputs[n];
}