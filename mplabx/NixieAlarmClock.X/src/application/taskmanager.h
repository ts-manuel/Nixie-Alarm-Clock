/* 
 * File:   taskmanager.h
 * Author: ts-manuel
 *
 * 
 */


#ifndef _TASK_H_
#define	_TASK_H_

#include <stdint.h>
#include <stdbool.h>
#include "tasks/buttons.h"


#define _APP_STACK_DEPTH    4


typedef struct
{
    bool initialized;
    void(*Initialize)(void*);
    void(*Update)(void*);
} Task_t;

void APP_Initialize(Task_t* task, void* arg);

void APP_Update(void);

void APP_PushTask(Task_t* task, void* arg);

void APP_PopTask(void);


// Available tasks
extern Task_t task_home;
extern Task_t task_set_alarm;
extern Task_t task_set_time;


#endif	/* _TASK_H_ */

