/* 
 * File:   time.h
 * Author: ts-manuel
 *
 * Read current time fro RTC
 */

#include "rtcc.h"
#include "time.h"

void TIME_Update(void)
{
    bcdTime_t time;
    
    if (RTCC_BCDTimeGet(&time))
        rtcTime = time;
}