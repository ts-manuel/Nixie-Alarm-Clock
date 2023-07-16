/* 
 * File:   config.h
 * Author: ts-manuel
 *
 * Configuration file for the application.
 */

#ifndef _CONFIG_H_
#define	_CONFIG_H_

// Display configuration
#define _DISPLAY_TOGGLE_PERIOD_ms   1000    // Toggle priod for the segmants
#define _DISPLAY_HALF_CLK_us        5       // Display shift register clock frequency

// Button configuration
#define _BUTTONS_SHORT_PRESS_MS     100     // Time required to trigger a short press
#define _BUTTONS_LONG_PRESS_MS      2000    // Time required to trigger a long press

// Number of available alarm slots
#define _ALARM_SLOT_COUNT  4   // This is fixed by the number of ligts on the fromt panel

#endif	/* _CONFIG_H_ */

