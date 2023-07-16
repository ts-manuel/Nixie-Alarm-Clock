/* 
 * File:   delay.h
 * Author: ts-manuel
 *
 * Delay functions
 */

#include "delay.h"


void TIME_delay_us(uint16_t x){
    while(x-- > 0){
        __asm__("NOP");
        __asm__("NOP");
        __asm__("NOP");
        __asm__("NOP");
        __asm__("NOP");
        __asm__("NOP");
        __asm__("NOP");
    }
}


void TIME_delay_ms(uint16_t x){
    while(x-- > 0){
        TIME_delay_us(1000);
    }
}







