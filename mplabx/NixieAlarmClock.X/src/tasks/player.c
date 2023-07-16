/* 
 * File:   player.h
 * Author: ts-manuel
 *
 * 
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "system.h"
#include "pin_manager.h"
#include "dma.h"
#include "sd_spi/sd_spi.h"
#include "fatfs/ff.h"
#include "hardware/PAM8407.h"
#include "utils/delay.h"
#include "utils/logging.h"
#include "settings.h"
#include "player.h"


#define _BUFFER_SIZE    512


const char* frToStr[] = 
{
	"FR_OK",
	"FR_DISK_ERR",              /* (1) A hard error occurred in the low level disk I/O layer */
	"FR_INT_ERR",				/* (2) Assertion failed */
	"FR_NOT_READY",             /* (3) The physical drive cannot work */
	"FR_NO_FILE",				/* (4) Could not find the file */
	"FR_NO_PATH",				/* (5) Could not find the path */
	"FR_INVALID_NAME",          /* (6) The path name format is invalid */
	"FR_DENIED",				/* (7) Access denied due to prohibited access or directory full */
	"FR_EXIST",                 /* (8) Access denied due to prohibited access */
	"FR_INVALID_OBJECT",		/* (9) The file/directory object is invalid */
	"FR_WRITE_PROTECTED",		/* (10) The physical drive is write protected */
	"FR_INVALID_DRIVE",         /* (11) The logical drive number is invalid */
	"FR_NOT_ENABLED",			/* (12) The volume has no work area */
	"FR_NO_FILESYSTEM",         /* (13) There is no valid FAT volume */
	"FR_MKFS_ABORTED",          /* (14) The f_mkfs() aborted due to any problem */
	"FR_TIMEOUT",				/* (15) Could not get a grant to access the volume within defined period */
	"FR_LOCKED",				/* (16) The operation is rejected according to the file sharing policy */
	"FR_NOT_ENOUGH_CORE",		/* (17) LFN working buffer could not be allocated */
	"FR_TOO_MANY_OPEN_FILES",	/* (18) Number of open files > FF_FS_LOCK */
	"FR_INVALID_PARAMETER"      /* (19) Given parameter is invalid */
};

const char* playerCmdStr[] =
{
    "NOP",
    "PLAY",
    "STOP",
    "VOLUME_UP",
    "VOLUME_DOWN"
};


static int16_t __attribute__ ((address(0x2000))) buffer[_BUFFER_SIZE];

static FATFS    drive;
static FIL      file;

static void Play(void);
static void Stop(void);
static void VolumeUp(void);
static void VolumeDown(void);
static FRESULT MountSD(FATFS* drive);
static void UnmountSD(void);
static uint16_t GetWavFileCount(void);
static void FindRandomWAVName(char* fname);


void PLAYER_Initialize(void)
{
    playerVolume = 11;
    playerCtrl.cmd = e_PLAYER_CMD_NOP;
    playerCtrl.state = e_PLAYER_IDLE;
}


void PLAYER_Update(void)
{
    e_PlayerState_t nextState = playerCtrl.state;
    
    if (playerCtrl.cmd == e_PLAYER_CMD_NOP)
        return;
    
    LOG_TRACE1("PLAYER_Update(): Command = %s\n", playerCmdStr[playerCtrl.cmd]);
    
    switch (playerCtrl.cmd)
    {
        case e_PLAYER_CMD_PLAY:
            if (playerCtrl.state == e_PLAYER_BUSY)
                Stop();
            Play();
            nextState = e_PLAYER_BUSY;
            break;
            
        case e_PLAYER_CMD_STOP:
            if (playerCtrl.state == e_PLAYER_BUSY)
                Stop();
            nextState = e_PLAYER_IDLE;
            break;
            
        case e_PLAYER_CMD_VOLUME_UP:
            VolumeUp();
            break;
            
        case e_PLAYER_CMD_VOLUME_DOWN:
            VolumeDown();
            break;
            
        default:
            break;
    }
    
    playerCtrl.cmd = e_PLAYER_CMD_NOP;
    playerCtrl.state = nextState;
}


/**
 * Start playing a random song from the SD Card
 */
static void Play(void)
{
    FRESULT res;
    char fname[FF_SFN_BUF + 1];
    
    // Try mounting the SD card
    if ((res = MountSD(&drive)) != FR_OK)
    {
        LOG_ERROR("PLAYER Play(): MountSD() exit code: %s\n", frToStr[res]);
        return;
    }
    
    // Find random file
    FindRandomWAVName(fname);
    
    if ((res = f_open(&file, fname, FA_READ)) != FR_OK)
    {
        LOG_ERROR("PLAYER Play(): f_open() exit code: %s\n", frToStr[res]);
        return;
    }
    
    LOG_INFO("Playing file: %s\n", fname);
    
    // Skip header and fill buffer
    f_read(&file, buffer, 44, NULL);
    f_read(&file, buffer, _BUFFER_SIZE * 2, NULL);
    
    // Enable amplifier
    PAM8407_Enable();
    PAM8407_SetVolume(playerVolume);
    
    // Enable DAC
    O_DAC_MUTE_SetLow();
    
    // Start DMA Transfer
    DMA_ChannelEnable(DMA_CHANNEL_0);
    DMA_TransferCountSet(DMA_CHANNEL_0, _BUFFER_SIZE);
    DMA_SourceAddressSet(DMA_CHANNEL_0, (uint16_t)buffer);
    DMA_SoftwareTriggerEnable(DMA_CHANNEL_0);
}


/**
 * Stop playback
 */
static void Stop(void)
{
    LOG_TRACE1("PLAYER Stop()\n");
    
    // Disable power amplifier
    PAM8407_Disable();
    
    // Mute DAC
    O_DAC_MUTE_SetHigh();
    
    // Power off SD card
    UnmountSD();
    
    // Stop DMA channel from triggering interrupts
    DMA_ChannelDisable(DMA_CHANNEL_0);
}


static void VolumeUp(void)
{
    PAM8407_SetVolume(playerVolume + 1);
    playerVolume = PAM8407_GetVolume();
}


static void VolumeDown(void)
{
    PAM8407_SetVolume(playerVolume - 1);
    playerVolume = PAM8407_GetVolume();
}


/**
 * DMA0 Interrupt on half and full transfer
 * 
 */
void DMA_Channel0_CallBack(void)
{
    int16_t* dest = DMAINT0bits.HALFIF ? buffer : buffer + _BUFFER_SIZE / 2;
    UINT byte_read;
    DMAINT0bits.HALFIF = 0;
    
    if (playerCtrl.state == e_PLAYER_BUSY)
    {
        f_read(&file, dest, _BUFFER_SIZE, &byte_read);
        
        if (byte_read < _BUFFER_SIZE)
        {
            f_close(&file);
            
            playerCtrl.cmd = e_PLAYER_CMD_STOP;
        }
    }
}


static FRESULT MountSD(FATFS* drive)
{
    FRESULT res;
    
    O_SD_PWR_EN_SetHigh();
    TIME_delay_ms(500);
    
    if( SD_SPI_IsMediaPresent() == false)
    {
        return FR_DISK_ERR;
    }

    if ((res = f_mount(drive,"0:",1)) != FR_OK)
    {
        LOG_ERROR("PLAYER MountSD(): f_mount() exit code: %s\n", frToStr[res]);
    }
    
    return res;
}


/*
 
 */
static void UnmountSD(void)
{
    // Unmount from file system
    f_mount(0,"0:",0);
    
    // Remove power
    O_SD_PWR_EN_SetLow();
}


/**
 * 
 */
static uint16_t GetWavFileCount(void)
{
    FFDIR dir;
    FILINFO fno;
    FRESULT res;
    uint16_t fileCount = 0;
    
    if ((res =  f_findfirst(&dir, &fno, "", "*.WAV")) != FR_OK)
    {
        LOG_ERROR("PLAYER GetWavFileCount() - f_findfirst exit code: %s\n", frToStr[res]);
        return 0;
    }

    LOG_TRACE1("PLAYER GetWavFileCount(): Searching for .WAV files:\n");
    
    while (res == FR_OK && fno.fname[0]) {         // Repeat while an item is found
        LOG_TRACE0("   %s\n", fno.fname);
        fileCount ++;
        res = f_findnext(&dir, &fno);               /* Search for next item */
    }

    if ((res = f_closedir(&dir)) != FR_OK)
    {
        LOG_ERROR("PLAYER GetWavFileCount() - f_closedir exit code: %s\n", frToStr[res]);
        return 0;
    }

    LOG_TRACE1("PLAYER GetWavFileCount(): Found %d files\n", fileCount);
    
    return fileCount;
}



/**
 * 
 * @param fname
 * @param fcount
 */
static void FindRandomWAVName(char* fname)
{
    FFDIR dir;
    FILINFO fno;
    FRESULT res;
    uint16_t wavCount;
    uint16_t index;
    static uint16_t lastIndex = 0;
    
    // Find number of WAV files
    wavCount = GetWavFileCount();
    
    // Compute random index different from the previous
    do
    {
        index = rand() % wavCount;
    }
    while (index == lastIndex);
    lastIndex  = index;
    
    fname[0] = '\0';
    
    if ((res =  f_findfirst(&dir, &fno, "", "*.WAV")) != FR_OK)
    {
        LOG_ERROR("PLAYER FindRandomWAVName() - f_findfirst exit code: %s\n", frToStr[res]);
        return;
    }
    
    LOG_TRACE0("PLAYER FindRandomWAVName(): searching for index %d\n", index);
    
    for (uint16_t i = 0; i < index; i++)
    {
        LOG_TRACE0("PLAYER FindRandomWAVName(): loop %d\n", i);
        
        if ((res = f_findnext(&dir, &fno)) != FR_OK)
        {
            LOG_ERROR("PLAYER FindRandomWAVName() - f_closedir exit code: %s\n", frToStr[res]);
            return;
        }
    }
    
    LOG_TRACE0("PLAYER FindRandomWAVName(): strcpy %s\n", fno.fname);
    
    strcpy(fname, fno.fname);
}