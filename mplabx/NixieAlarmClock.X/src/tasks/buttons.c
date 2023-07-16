
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "buttons.h"
#include "system.h"
#include "pin_manager.h"
#include "time/millis.h"


#define _IN_BTN_SET     0
#define _IN_BTN_UP      1
#define _IN_BTN_DOWN    2
#define _IN_BTN_PLAY    3
#define _IN_COUT        4


typedef struct
{
    bool        active;
    BtnState_t  state;              // Current input state
    uint32_t    activationTimeMs;
} Button_t;


static Button_t buttons[_IN_COUT];


/**
 * Read the state of all the inputs and update the struct
 * 
 * @param imputs
 */
static void ReadInputs()
{
    buttons[_IN_BTN_SET].active   = !I_BTN_SET_GetValue();
    buttons[_IN_BTN_UP].active    = !I_BTN_UP_GetValue();
    buttons[_IN_BTN_DOWN].active  = !I_BTN_DOWN_GetValue();
    buttons[_IN_BTN_PLAY].active  = !I_BTN_PLAY_GetValue();
}


/**
 * Initialize buttons with default values
 */
void BTN_Initialize()
{
    buttons[_IN_BTN_SET].active = false;
    buttons[_IN_BTN_SET].state = e_BTN_IDLE;
    buttons[_IN_BTN_SET].activationTimeMs = 0;

    buttons[_IN_BTN_UP].active = false;
    buttons[_IN_BTN_UP].state = e_BTN_IDLE;
    buttons[_IN_BTN_UP].activationTimeMs = 0;
    
    buttons[_IN_BTN_DOWN].active = false;
    buttons[_IN_BTN_DOWN].state = e_BTN_IDLE;
    buttons[_IN_BTN_DOWN].activationTimeMs = 0;
    
    buttons[_IN_BTN_PLAY].active = false;
    buttons[_IN_BTN_PLAY].state = e_BTN_IDLE;
    buttons[_IN_BTN_PLAY].activationTimeMs = 0;
}


/**
 * Read pin states and update input states
 */
void BTN_Update()
{
    uint32_t now = TIME_Millis();
    
    // Read the state of all inputs
    ReadInputs(buttons);
    
    // Update the sate of all inputs
    for (int i = 0; i < _IN_COUT; i++)
    {
        Button_t* st = &buttons[i];
        
        switch (st->state)
        {
            case e_BTN_IDLE:
                if (st->active)
                {
                    st->state = e_BTN_ACTIVE;
                    st->activationTimeMs = now;
                }
                break;
                
            case e_BTN_ACTIVE:
                if (!st->active)
                    st->state = e_BTN_IDLE;
                else if (now - st->activationTimeMs >= _BUTTONS_SHORT_PRESS_MS)
                    st->state = e_BTN_SHORT_PRESS;
                break;
                
            case e_BTN_SHORT_PRESS:
                if (!st->active)
                    st->state = e_BTN_IDLE;
                else
                    st->state = e_BTN_SHORT_HOLD;
                break;
                
            case e_BTN_SHORT_HOLD:
                if (!st->active)
                    st->state = e_BTN_IDLE;
                else if (now - st->activationTimeMs >= _BUTTONS_LONG_PRESS_MS)
                    st->state = e_BTN_LONG_PRESS;
                break;
        
            case e_BTN_LONG_PRESS:
                if (!st->active)
                    st->state = e_BTN_IDLE;
                else
                    st->state = e_BTN_LONG_HOLD;
                break;
   
            case e_BTN_LONG_HOLD:
                if (!st->active)
                    st->state = e_BTN_IDLE;
                break;
                
            default:
                st->state = e_BTN_IDLE;
        }
    }
}


/*
 * Return the button state
 */
BtnState_t BTN_SET_GetState(void)
{
    return buttons[_IN_BTN_SET].state;
}

BtnState_t BTN_UP_GetState(void)
{
    return buttons[_IN_BTN_UP].state;
}

BtnState_t BTN_DOWN_GetState(void)
{
    return buttons[_IN_BTN_DOWN].state;
}

BtnState_t BTN_PLAY_GetState(void)
{
    return buttons[_IN_BTN_PLAY].state;
}