/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 *
 * SDCard_API.c
 *
 ******************************************************************************/

#include "stdlib.h"
#include "string.h"
#include "SDCard_API.h"
#include "driverlib.h"
#include "sdcard.h"
#include "HAL_SDCard.h"

SDCardLib sdCardLib;
char dirs[10][MAX_DIR_LEN];
char files[10][MAX_FILE_LEN];
uint8_t dirNum = 0;
uint8_t fileNum = 0;

#define MAX_BUF_SIZE 32
char buffer[MAX_BUF_SIZE];

// FatFs Static Variables
FRESULT fr;
static FIL fil;                 /* File object */
static char filename[31];
static char directory[31];

/*
 * Writes string buffer to filename
 */
SDCardLib_Status write_SDCard(char* buffer)
{
    //Plugin SDcard interface to SDCard lib
    SDCardLib_init(&sdCardLib, &sdIntf_MSP430FR5994LP);

    //Detect SD card
    SDCardLib_Status st = SDCardLib_detectCard(&sdCardLib);
    if (st == SDCARDLIB_STATUS_NOT_PRESENT) {
    	SDCardLib_unInit(&sdCardLib);
    	return(st);
    }

    //Configure pull up resistor on MISO per MMC/SDC documentation
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN7);

    __delay_cycles(100000);

    fr = f_open(&fil, filename, FA_WRITE | FA_OPEN_EXISTING);
    if (fr) {
    	f_close(&fil);
    	SDCardLib_unInit(&sdCardLib);
    	return (st);
    }

    fr = f_lseek(&fil, f_size(&fil));
    f_puts(buffer, &fil);
    f_puts("\r", &fil);
    fr = f_close(&fil);

    SDCardLib_unInit(&sdCardLib);

    //Disable SPI module -- Reconfigure I/O
    EUSCI_B_SPI_disable(EUSCI_B0_BASE);

    GPIO_setOutputLowOnPin(
        GPIO_PORT_P2,
        GPIO_PIN2
    );

    //Configure Pins for UCB0CLK
    GPIO_setAsOutputPin(
        GPIO_PORT_P2,
        GPIO_PIN2
    );

    //Configure Pins for UCB0TXD/UCB0SIMO, UCB0RXD/UCB0SOMI
    GPIO_setOutputLowOnPin(
        GPIO_PORT_P1,
        GPIO_PIN6 + GPIO_PIN7
    );

    //Set CS pin high
    GPIO_setOutputHighOnPin(
        GPIO_PORT_P4,
        GPIO_PIN0
        );

    //Configure Pins for UCB0TXD/UCB0SIMO, UCB0RXD/UCB0SOMI
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN6 + GPIO_PIN7
    );

    return(SDCARDLIB_STATUS_PRESENT);
}


SDCardLib_Status createFile_SDCard(char* path, char* file)
{
    uint16_t bw = 0;

    //Configure SDCard CS pin
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, BIT0);
    GPIO_setAsOutputPin(GPIO_PORT_P4, BIT0);

    //Plugin SDcard interface to SDCard lib
    SDCardLib_init(&sdCardLib, &sdIntf_MSP430FR5994LP);

    //Detect SD card
    SDCardLib_Status st = SDCardLib_detectCard(&sdCardLib);
    if (st == SDCARDLIB_STATUS_NOT_PRESENT) {
        SDCardLib_unInit(&sdCardLib);
        return(st);
    }

    // setup the directory name
    memset(directory,0,sizeof(directory));
    strcpy(directory, path);

    // Read the card (if file exists ???)
    fr = SDCardLib_getDirectory(&sdCardLib, directory, dirs, &dirNum, files, &fileNum);

    //Create the directory under the root directory
    fr = SDCardLib_createDirectory(&sdCardLib, directory);
    if (fr != FR_OK && fr != FR_EXIST) {
        SDCardLib_unInit(&sdCardLib);
        return(st);
    }


    // Clear previous filename
    memset(filename,0,sizeof(filename));

    // Construct file's full path and name
    strcpy(filename, directory);
    strcat(filename, "/");
    strcat(filename, file);

    _no_operation();  // set break point to check filename is ok

    // Write new log file (creates file on disk and writes first entry)
    char header[] = "Creating new file\r\n";
    fr = SDCardLib_writeFile(&sdCardLib, filename, header, sizeof(header), &bw);
    if (fr != FR_OK) {
        SDCardLib_unInit(&sdCardLib);
        return(st);
    }


#if 0
    // Optional second line
    fr = f_open(&fil, filename, FA_WRITE | FA_OPEN_EXISTING);
    if (fr) {
        f_close(&fil);
        SDCardLib_unInit(&sdCardLib);
        return(st);
    }
    fr = f_lseek(&fil, f_size(&fil));
    f_puts("Add a second line\r", &fil);
    fr = f_close(&fil);
#endif

    SDCardLib_unInit(&sdCardLib);
    return(st);
}

/*
 * Converts integer to string
 */
char *itoa(int num, char *str, int radix) {
    char sign = 0;
    char temp[17];  //an int can only be 16 bits long
                    //at radix 2 (binary) the string
                    //is at most 16 + 1 null long.
    int temp_loc = 0;
    int digit;
    int str_loc = 0;

    //save sign for radix 10 conversion
    if (radix == 10 && num < 0) {
        sign = 1;
        num = -num;
    }

    //construct a backward string of the number.
    do {
        digit = (unsigned int)num % radix;
        if (digit < 10)
            temp[temp_loc++] = digit + '0';
        else
            temp[temp_loc++] = digit - 10 + 'A';
        num = (((unsigned int)num) / radix);
    } while ((unsigned int)num > 0);

    //now add the sign for radix 10
    if (radix == 10 && sign) {
        temp[temp_loc] = '-';
    } else {
        temp_loc--;
    }


    //now reverse the string.
    while ( temp_loc >=0 ) {// while there are still chars
        str[str_loc++] = temp[temp_loc--];
    }
    str[str_loc] = 0; // add null termination.

    return str;
}
