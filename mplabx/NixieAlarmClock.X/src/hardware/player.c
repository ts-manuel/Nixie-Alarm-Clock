
#include "../../mcc_generated_files/system.h"
#include "../../mcc_generated_files/pin_manager.h"
#include "../../mcc_generated_files/dma.h"
#include "../../mcc_generated_files/fatfs/ff.h"
#include "player.h"
#include "time/delay.h"
#include "PAM8407.h"
#include "../hardware/PAM8407.h"
#include "../../mcc_generated_files/sd_spi/sd_spi.h"
#include "settings.h"
#include "logging/logging.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

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

static int16_t __attribute__ ((address(0x2000))) buffer[_BUFFER_SIZE];

static eDAC_State_t playerState = e_PLAYER_IDLE;
static FATFS    drive;
static FIL      file;

static FRESULT MountSD(FATFS* drive);
static void UnmountSD(void);
static uint16_t GetWavFileCount(void);
static void FindRandomWAVName(char* fname, uint16_t fcount);


/**
 * Start playing a random song from the SD Card
 */
void PLAYER_Play(uint8_t volume)
{
    FRESULT res;
    uint16_t wavCount;
    char fname[FF_SFN_BUF];
    
    // Try mounting the SD card
    if ((res = MountSD(&drive)) != FR_OK)
    {
        printf("PLAYER_Play(): MountSD failed with code: %s\n", frToStr[res]);
        return;
    }
    
    // Find number of WAV files
    wavCount = GetWavFileCount();
    
    // Find random file
    FindRandomWAVName(fname, wavCount);
    
    //strcpy(fname, "TESTTONE.WAV");
    
    if ((res = f_open(&file, fname, FA_READ)) != FR_OK)
    {
        LOG_ERROR("Unable to find .WAV file\n");
        return;
    }
    
    LOG_INFO("Playing file: %s\n", fname);
    
    // Skip header and fill buffer
    f_read(&file, buffer, 44, NULL);
    f_read(&file, buffer, _BUFFER_SIZE * 2, NULL);
    
    // Enable amplifier
    PAM8407_Enable(volume);
    
    // Enable DAC
    O_DAC_MUTE_SetLow();
    
    // Start DMA Transfer
    DMA_TransferCountSet(DMA_CHANNEL_0, _BUFFER_SIZE);
    DMA_SourceAddressSet(DMA_CHANNEL_0, (uint16_t)buffer);
    DMA_SoftwareTriggerEnable(DMA_CHANNEL_0);
    
    playerState = e_PLAYER_BUSY;
}


/**
 * Stop playback
 */
void PLAYER_Stop(void)
{
    // Disable power amplifier
    PAM8407_Disable();
    
    // Mute DAC
    O_DAC_MUTE_SetHigh();
    
    UnmountSD();
    
    playerState = e_PLAYER_IDLE;
}


/* Get player state */
eDAC_State_t PLAYER_GetState(void)
{
    return playerState;
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
    
    if (playerState == e_PLAYER_BUSY)
    {
        f_read(&file, dest, _BUFFER_SIZE, &byte_read);
        
        if (byte_read < _BUFFER_SIZE)
        {
            f_close(&file);
            
            PLAYER_Stop();
        }
    }
}


 /*
 
  */
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
        LOG_ERROR("MountSD - f_mount returned: %s\n", frToStr[res]);
    }
    
    return res;
}


static void UnmountSD(void)
{
    //O_SD_PWR_EN_SetLow();
    
    f_mount(0,"0:",0);
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
        LOG_ERROR("GetWavFileCount - f_findfirst returned: %s\n", frToStr[res]);
        return 0;
    }

    LOG_TRACE1("Searching for .WAV files:\n");
    
    while (res == FR_OK && fno.fname[0]) {         // Repeat while an item is found
        LOG_TRACE2("   %s\n", fno.fname);
        fileCount ++;
        res = f_findnext(&dir, &fno);               /* Search for next item */
    }

    if ((res = f_closedir(&dir)) != FR_OK)
    {
        LOG_ERROR("GetWavFileCount - f_closedir returned: %s\n", frToStr[res]);
        return 0;
    }

    LOG_TRACE2("Found %d files\n", fileCount);
    
    return fileCount;
}



/**
 * 
 * @param fname
 * @param fcount
 */
static void FindRandomWAVName(char* fname, uint16_t fcount)
{
    FFDIR dir;
    FILINFO fno;
    FRESULT res;
    int index = rand() % fcount;
    
    fname[0] = "\0";
    
    if ((res =  f_findfirst(&dir, &fno, "", "*.WAV")) != FR_OK)
    {
        LOG_ERROR("FindRandomWAVName - f_findfirst returned: %s\n", frToStr[res]);
        return;
    }
    
    for (uint16_t i = 0; i < index; i++)
    {
        if ((res = f_findnext(&dir, &fno)) != FR_OK)
        {
            LOG_ERROR("FindRandomWAVName - f_closedir returned: %s\n", frToStr[res]);
            return;
        }
    }
    
    strcpy(fname, fno.fname);
}