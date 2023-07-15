/* 
 * File:   dac_i2s.h
 * Author: Manuel
 *
 * Created on November 20, 2022, 6:09 PM
 */

#ifndef DAC_I2S_H
#define	DAC_I2S_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum {e_PLAYER_IDLE, e_PLAYER_BUSY} eDAC_State_t;
    
    /* Start playback of buff samples */
    void PLAYER_Play(uint8_t volume);
    
    /* Stop playback */
    void PLAYER_Stop(void);
    
    /* Get player state */
    eDAC_State_t PLAYER_GetState(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* DAC_I2S_H */

