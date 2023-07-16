/* 
 * File:   views.h
 * Author: ts-manuel
 *
 * 
 */

#include <stdint.h>
#include <stdbool.h>
#include "utils/logging.h"
#include "views.h"


void HomeInit(void);
View_t HomeUpdate(void);
void SetTimeInit(void);
View_t SetTimeUpdate(void);
void SetAlarmInit(void);
View_t SetAlarmUpdate(void);


const char* viewsStr[] = {"VIEW_HOME", "VIEW_SET_TIME", "VIEW_SET_ALARM"};
static View_t currentView;


void VIEWS_Initialize(void)
{
    currentView = e_VIEW_HOME;
    
    LOG_TRACE1("VIEWS_Initialize() - currentView: e_VIEW_HOME\n");
    //LOG_TRACE1("VIEWS_Initialize() - currentView: %s\n", viewsStr[currentView]);
    
    HomeInit();
}


void VIEWS_Update(void)
{
    View_t nextView = e_VIEW_HOME;
    
    
    // Call update function on current view
    switch (currentView)
    {
        case e_VIEW_HOME:
            nextView = HomeUpdate();
            break;
        case e_VIEW_SET_TIME:
            nextView = SetTimeUpdate();
            break;
        case e_VIEW_SET_ALARM:
            nextView = SetAlarmUpdate();
            break;
    }
    
    // Call init function if the view has changed
    if (nextView != currentView)
    {
        LOG_TRACE1("VIEWS_Update() - currentView: %s\n", viewsStr[nextView]);
        
        switch (nextView)
        {
            case e_VIEW_HOME:
                HomeInit();
                break;
            case e_VIEW_SET_TIME:
                SetTimeInit();
                break;
            case e_VIEW_SET_ALARM:
                SetAlarmInit();
                break;
        }
    }
    
    currentView = nextView;
}