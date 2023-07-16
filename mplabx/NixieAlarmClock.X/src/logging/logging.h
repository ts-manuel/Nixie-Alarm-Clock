/* 
 * File:   logging.h
 * Author: ts-manuel
 *
 * Macros to log stuff with colors and different logging levels
 * 
 * Log level usage:
 *      TRACE0:	for tracing the execution of the application used for frequent events
 *      TRACE1:	for tracing the execution of the application used for less frequent events
 *      TRACE2:	for tracing the execution of the application used for un frequent events
 *      INFO:	useful information to print to the console in the final application
 *      WARNIG:	unwanted condition that doesn't stop the application
 *      ERROR:	error condition that causes the application to stop
 */

#ifndef LOGGING_H
#define	LOGGING_H

#include <stdbool.h>


/*
 * Turns on or off the colored output.
 */
#define _LOG_USE_COLORS true


/*
 * Active log level, messages 
 */
#define _LOGLV_TRACE0	0
#define _LOGLV_TRACE1	1
#define _LOGLV_TRACE2	2
#define _LOGLV_INFO		10
#define _LOGLV_WARNING	20
#define _LOGLV_ERROR	30
#define _LOG_LEVEL _LOGLV_TRACE0


void logg_console(int level, const char* format, ...);


#if _LOG_LEVEL <= _LOGLV_TRACE0
#define LOG_TRACE0(msg, ...) logg_console(_LOGLV_TRACE0, msg, ##__VA_ARGS__)
#else
#define LOG_TRACE0(x) do {} while(0)
#endif

#if _LOG_LEVEL <= _LOGLV_TRACE1
#define LOG_TRACE1(msg, ...) logg_console(_LOGLV_TRACE1, msg, ##__VA_ARGS__)
#else
#define LOG_TRACE1(x) do {} while(0)
#endif

#if _LOG_LEVEL <= _LOGLV_TRACE2
#define LOG_TRACE2(msg, ...) logg_console(_LOGLV_TRACE2, msg, ##__VA_ARGS__)
#else
#define LOG_TRACE2(x) do {} while(0)
#endif

#if _LOG_LEVEL <= _LOGLV_INFO
#define LOG_INFO(msg, ...) logg_console(_LOGLV_INFO, msg, ##__VA_ARGS__)
#else
#define LOG_INFO(x) do {} while(0)
#endif

#if _LOG_LEVEL <= _LOGLV_WARNING
#define LOG_WARNING(msg, ...) logg_console(_LOGLV_WARNING, msg, ##__VA_ARGS__)
#else
#define LOG_ERROR(x) do {} while(0)
#endif

#if _LOG_LEVEL <= _LOGLV_ERROR
#define LOG_ERROR(msg, ...) logg_console(_LOGLV_ERROR, msg, ##__VA_ARGS__)
#else
#define LOG_ERROR(x) do {} while(0)
#endif

#endif	/* LOGGING_H */