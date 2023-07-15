
#include "../mcc_generated_files/system.h"
#include "../mcc_generated_files/tmr1.h"

static uint32_t seconds = 0;

static void ISR_Millis(void);

void __delay_us(int x){
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


void __delay_ms(int x){
    while(x-- > 0){
        __delay_us(1000);
    }
}


void MillisInitialize(void)
{
    TMR1_SetInterruptHandler(ISR_Millis);
}

static void ISR_Millis(void)
{
    seconds += 2;
}


uint32_t Millis(void)
{
    uint16_t tmr1 = TMR1_Counter16BitGet();
    return (seconds * 1000) + (tmr1 >> 5);
}