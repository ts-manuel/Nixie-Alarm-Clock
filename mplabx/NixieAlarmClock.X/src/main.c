/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.1
        Device            :  PIC24FJ64GA705
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.70
        MPLAB 	          :  MPLAB X v5.50
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "system.h"
#include "rtcc.h"
#include "delay.h"
#include "input/input.h"
#include "hardware/display.h"
#include "hardware/player.h"
#include "hardware/PAM8407.h"
#include "hardware/PAM8407.h"
#include "application/taskmanager.h"
#include "application/task_args.h"
#include "logging/logging.h"


#define _FW_VER_MAJ 0
#define _FW_VER_MIN 1


/*
                         Main application
 */
int main(void)
{
    Settings_t settings;
    bcdTime_t bcd_time;
    
    // initialize the device
    SYSTEM_Initialize();
    MillisInitialize();
    
    U1BRG = 0x174;  // Set UART1 to 9600 code-configurator doesn't support 7.168 MHz as clock frequency
    
    // Load settings
    SETTINGS_Load(&settings);
    
    // Inputs initialization
    INPUTS_Initialize();
    
    // Display init
    DISPLAY_Initialize();
    DISPLAY_SetPower(true);
    
    // Task manager initialization
    APP_Initialize(&task_home, (void*)&settings);
    
    // Read clock
    RTCC_BCDTimeGet(&bcd_time);
    
    __delay_ms(500);
    LOG_INFO("\n\n");
    LOG_INFO("--------------------------------------\n");
    LOG_INFO("COLD START FW Ver. %d.%d\n", _FW_VER_MAJ, _FW_VER_MIN);
    LOG_INFO("Time: %d%d:%d%d Day: %d\n", bcd_time.tm_hour >> 4, bcd_time.tm_hour & 0x0f,
            bcd_time.tm_min >> 4, bcd_time.tm_min & 0x0f, bcd_time.tm_wday);
    LOG_INFO("\n");
    LOG_INFO("Settings:\n");
    LOG_INFO("  Volume: %d\n", settings.volume);
    AlarmSlot_t* al;
    for (uint8_t i = 0; i < _AL_SLOT_COUNT; i++)
    {
        al = &settings.alarmSlots[i];
        LOG_INFO("  Alarm slot [%d] Time: %d%d:%d%d Day: %02X\n", i, al->hour >> 4, al->hour & 0x0f, al->min >> 4, al->min & 0x0f, al->day);
    }
    LOG_INFO("--------------------------------------\n");
    
    
    while (1)
    {
        static uint32_t lastMillis = 0;
        uint32_t millis = Millis();
        
        // Heartbeat LED
        if (millis - lastMillis > 500)
        {
            lastMillis = millis;
            O_LED_Toggle();
        }
        
        // Read input states
        INPUTS_Update();
        
        // Update tasks
        APP_Update();
        
        // Update display
        DISPLAY_Update();
    }

    return 1;
}






/**
 End of File
*/

