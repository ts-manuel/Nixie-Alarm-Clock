/* 
 * File:   settings.h
 * Author: ts-manuel
 *
 * Automatically store setting when modified and load on startup
 */

#ifndef _TASKS_SETTINGS_H_
#define	_TASKS_SETTINGS_H_

#include <stdint.h>
#include <stdbool.h>


void SETTINGS_Initialize(void);

void SETTINGS_Update(void);


#endif	/* _TASKS_SETTINGS_H_ */

