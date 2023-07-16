/* 
 * File:   logging.h
 * Author: ts-manuel
 *
 * Macros to log stuff with colors and different logging levels
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "utils/millis.h"
#include "logging.h"

#define ESC "\e"    // Linux esc

/*
 * Color definitions
 * Escape sequence to change graphic mode
 * ESC + '[' + GM_X + ';' + GM_X + ... + GM_X + CL_X + 'm'
 */
#if (_LOG_USE_COLORS == true)

// Color 16
#define CL_BLK        ESC"[30m"
#define CL_RED        ESC"[31m"
#define CL_GRN        ESC"[32m"
#define CL_YEL        ESC"[33m"
#define CL_BLU        ESC"[34m"
#define CL_MAG        ESC"[35m"
#define CL_CYN        ESC"[36m"
#define CL_WHT        ESC"[37m"
#define CL_DEFAULT    ESC"[39m"
#define CL_BG_BLK     ESC"[40m"
#define CL_BG_RED     ESC"[41m"
#define CL_BG_GRN     ESC"[42m"
#define CL_BG_YEL     ESC"[43m"
#define CL_BG_BLU     ESC"[44m"
#define CL_BG_MAG     ESC"[45m"
#define CL_BG_CYN     ESC"[46m"
#define CL_BG_WHT     ESC"[47m"
#define CL_BG_DEFAULT ESC"[49m"

// Color 256
#define CL_256(x) ESC"[38;5;" x "m"

// Graphic mode
#define GM_RESET       ESC"[0m"
#define GM_BOLD        ESC"[1m"
#define GM_BOLD_R      ESC"[22m"
#define GM_DIM         ESC"[2m"
#define GM_DIM_R       ESC"[22m"
#define GM_ITALIC      ESC"[3m"
#define GM_ITALIC_R    ESC"[23m"
#define GM_UNDERLINE   ESC"[4m"
#define GM_UNDERLINE_R ESC"[24m"
#define GM_BLINK       ESC"[5m"
#define GM_BLINK_R     ESC"[25m"
#define GM_INVERSE     ESC"[7m"
#define GM_INVERSE_R   ESC"[27m"
#define GM_INVISIBLE   ESC"[8m"
#define GM_INVISIBLE_R ESC"[28m"
#define GM_STRIKE_TH   ESC"[9m"
#define GM_STRIKE_TH_R ESC"[29m"

#define RESET          ESC"[0m"

#if true
#define _TRACE0_COLOR   GM_RESET CL_256("242")
#define _TRACE1_COLOR   GM_RESET CL_256("247")
#define _TRACE2_COLOR   GM_RESET CL_256("74")
#define _INFO_COLOR     GM_RESET CL_256("106")
#define _WARNING_COLOR  GM_RESET CL_256("220")
#define _ERROR_COLOR    GM_RESET CL_256("196");
#else
#define _TRACE0_COLOR   GM_DIM    CL_WHT
#define _TRACE1_COLOR   GM_DIM    CL_WHT
#define _TRACE2_COLOR   GM_DIM    CL_WHT
#define _INFO_COLOR     GM_RESET  CL_CYN
#define _WARNING_COLOR  GM_RESET  CL_YEL
#define _ERROR_COLOR    GM_RESET  CL_RED
#endif
#endif


void logg_console(int level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    const char* color = "";
    uint32_t millis = TIME_Millis();
    uint32_t hour, min, sec;
    
    // Convert milliseconds in hour minutes and seconds
    sec = millis / 1000;
    millis = millis - sec * 1000;
    min = sec / 60;
    sec = sec - min * 60;
    hour = min / 60;
    min = min - hour * 60;
 
#if (_LOG_USE_COLORS == true)
    // Determine color
    switch (level)
    {
        case _LOGLV_TRACE0:     color = _TRACE0_COLOR;  break;
        case _LOGLV_TRACE1:     color = _TRACE1_COLOR;  break;
        case _LOGLV_TRACE2:     color = _TRACE2_COLOR;  break;
        case _LOGLV_INFO:       color = _INFO_COLOR;    break;
        case _LOGLV_WARNING:    color = _WARNING_COLOR; break;
        case _LOGLV_ERROR:      color = _ERROR_COLOR;   break;
        default:                color = _ERROR_COLOR;   break;
    }


    printf("%s[%02ld:%02ld:%02ld.%03ld]: ", color, hour, min, sec, millis);
    vprintf(format, args);
    printf("%s", RESET);
#else
    printf("[%02ld:%02ld:%02ld.%03ld]: ", hour, min, sec, millis);
    
    vprintf(format, args);
#endif

    va_end(args);
}


