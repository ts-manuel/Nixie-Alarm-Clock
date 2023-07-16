/* 
 * File:   time.h
 * Author: ts-manuel
 *
 * Read current time fro RTC
 */

#ifndef _TASK_TIME_H_
#define	_TASK_TIME_H_

#include "rtcc.h"

extern bcdTime_t rtcTime;

void TIME_Update(void);

#endif	/* _TASK_TIME_H_ */

