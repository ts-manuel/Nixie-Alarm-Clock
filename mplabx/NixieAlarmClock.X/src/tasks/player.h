/* 
 * File:   player.h
 * Author: ts-manuel
 *
 * 
 */

#ifndef _TASKS_PLAYER_H_
#define	_TASKS_PLAYER_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {e_PLAYER_IDLE, e_PLAYER_BUSY} e_PlayerState_t;
typedef enum {
    e_PLAYER_CMD_NOP = 0,
    e_PLAYER_CMD_PLAY,
    e_PLAYER_CMD_STOP,
    e_PLAYER_CMD_VOLUME_UP,
    e_PLAYER_CMD_VOLUME_DOWN
} e_PlayerCmd_t;

typedef struct {
    e_PlayerCmd_t   cmd;
    e_PlayerState_t state;
} PlayerCtrl_t;

extern PlayerCtrl_t playerCtrl;
extern int8_t playerVolume;


void PLAYER_Initialize(void);

void PLAYER_Update(void);

#endif	/* _TASKS_PLAYER_H_ */

