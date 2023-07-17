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
#include "tmr1.h"
#include "pin_manager.h"
#include "utils/millis.h"
#include "utils/delay.h"
#include "utils/logging.h"
#include "tasks/display.h"
#include "tasks/buttons.h"
#include "tasks/alarm.h"
#include "tasks/time.h"
#include "tasks/player.h"
#include "tasks/settings.h"
#include "views/views.h"
#include "hardware/PAM8407.h"


#define _FW_VER_MAJ 0
#define _FW_VER_MIN 2


/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    // Set UART1 baud rate, code-configurator doesn't support 7.168 MHz as clock frequency
    //U1BRG = 0x174; // 9600
    U1BRG = 186;    // 19200
    TIME_delay_ms(1);
    printf("\n\n");
    
    SETTINGS_Initialize();
    BTN_Initialize();
    VIEWS_Initialize();
    DISPLAY_Initialize();
    DISPLAY_SetPower(true);
    PLAYER_Initialize();
    
    // Read RTC time
    TIME_Update();

    
    LOG_INFO("\n\n");
    LOG_INFO("--------------------------------------\n");
    LOG_INFO("COLD START FW Ver. %d.%d\n", _FW_VER_MAJ, _FW_VER_MIN);
    LOG_INFO("Time: %d%d:%d%d Day: %d\n", rtcTime.tm_hour >> 4, rtcTime.tm_hour & 0x0f,
            rtcTime.tm_min >> 4, rtcTime.tm_min & 0x0f, rtcTime.tm_wday);
    LOG_INFO("\n");
    LOG_INFO("Settings:\n");
    LOG_INFO("  Volume: %d\n", playerVolume);
    AlarmSlot_t* al;
    for (uint8_t i = 0; i < _ALARM_SLOT_COUNT; i++)
    {
        al = &alarmSlots[i];
        LOG_INFO("  Alarm slot [%d] Time: %d%d:%d%d Day: %02X\n", i, al->hour >> 4, al->hour & 0x0f, al->min >> 4, al->min & 0x0f, al->day);
    }
    LOG_INFO("--------------------------------------\n");
    
    
    while (1)
    {
        static uint32_t lastMillis = 0;
        uint32_t millis = TIME_Millis();
        
        // Heartbeat LED
        if (millis - lastMillis > 500)
        {
            lastMillis = millis;
            O_LED_Toggle();
        }
        
        // Update tasks
        ALARM_Update();
        BTN_Update();
        DISPLAY_Update();
        TIME_Update();
        PLAYER_Update();
        SETTINGS_Update();
        
        // Update views
        VIEWS_Update();
    }

    return 1;
}






/**
 End of File
*/

