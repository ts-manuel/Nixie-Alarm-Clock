
#include "taskmanager.h"
#include "logging/logging.h"


typedef struct
{
    Task_t* task;
    void*   arg;
} AppStackEntry_t;

static AppStackEntry_t AppStack[_APP_STACK_DEPTH];
static uint8_t stackPointer = 0;
static AppStackEntry_t* stackTop = &AppStack[0];


/**
 * Initialize task manager with the first task
 * 
 * @param task
 * @param arg
 */
void APP_Initialize(Task_t* task, void* arg)
{
    stackPointer = 0;
    stackTop = &AppStack[0];
    APP_PushTask(task, arg);
}



/**
 * 
 * 
 * @param inputs
 */
void APP_Update(void)
{
    if (stackPointer == 0)
        return;
    
    // Initialize task
    if (!stackTop->task->initialized)
    {
        stackTop->task->Initialize(stackTop->arg);
        stackTop->task->initialized = true;
    }
    
    // Update task
    stackTop->task->Update(stackTop->arg);
}


/**
 * Push task to the stack
 * 
 * @param task
 * @param arg
 */
void APP_PushTask(Task_t* task, void* arg)
{
    if (stackPointer < _APP_STACK_DEPTH)
    {
        AppStack[stackPointer].task = task;
        AppStack[stackPointer].task->initialized = false;
        AppStack[stackPointer].arg  = arg;
        stackTop = &AppStack[stackPointer];
        stackPointer ++;
    }
    else
    {
        LOG_ERROR("Application stack overflow\n");
    }
}


/**
 * Pop task from the stack
 * 
 */
void APP_PopTask(void)
{
    if (stackPointer > 0)
    {
        stackPointer --;
        
        if (stackPointer > 0){
            stackTop = &AppStack[stackPointer - 1];
        }
    }
    else
    {
        LOG_ERROR("Application stack underflow\n");
    }
}