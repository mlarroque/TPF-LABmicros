/*
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    MK64F12_flashI2S.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"


/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
#define DEBUG_FLASH_1 0  //debug flashHal write and read
#define DEBUG_FLASH_2 0  //debug read data that had been written in DEBUG_FLASH_1
#define DEBUG_UDA_1 0
#define DEBUG_AUDIO_PLAYER_1 0  //debug save and read record
#define DEBUG_AUDIO_PLAYER_2 0  //debug decoding some piece of record
#define DEBUG_SAI_1 1   //debug i2s and dma
#define DEBUG_SAI_2 0

int timerCounter;
/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    PRINTF("Hello World\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;


    /*start debugging*/

#if DEBUG_FLASH_1
#include "flashHal.h"
    char data[] = {2, 4, 6, 8};  //array size: 4
    int dataTag = 1;
    char * p2dataRead = 0;
    int dataReadLen;
    int j = 0;

    if(flashINIT() != -1)
    {
    	PRINTF("flash init succesfuly\n");
        if(flashAlloc((int *)data, sizeof(data[0])*4, dataTag) != -1)
        {
        	PRINTF("flash alloc succesfuly\n");
        	p2dataRead = readFlash(&dataReadLen, dataTag);
        	if( p2dataRead != 0 )
        	{
        		PRINTF("%p\n", p2dataRead);
        		PRINTF("flash read succesfuly\n");
        		for(j = 0; j < 5; j++)
        		{
        			printf("%d \n", p2dataRead[j]);
        		}

        	}
        	else
        	{
        		PRINTF("READING FLASH ERROR\n");
        	}
        	}
        else
        {
        	printf("WRITING FLASH ERROR\n");
        }
     }
     else
     {
        printf("INITIALIZING FLASH ERROR\n");
     }
#elif DEBUG_FLASH_2
#include "flashHal.h"
    int dataTag = 1;
    char * p2dataRead = 0;
    int dataReadLen;
    int j = 0;
    if(flashINIT() != -1)
    {
        PRINTF("flash init succesfuly\n");
        p2dataRead = readFlash(&dataReadLen, dataTag);
        if( p2dataRead != 0 )
        {
        	PRINTF("%p\n", p2dataRead);
            PRINTF("flash read succesfuly\n");
            for(j = 0; j < 5; j++){
                printf("%d \n", p2dataRead[j]);
            }

         }
         else
         {
              PRINTF("READING FLASH ERROR\n");
         }
     }
    else{
    	printf("INITIALIZING FLASH ERROR\n");
    }

#elif DEBUG_UDA_1

#include "UDA.h"



#elif DEBUG_AUDIO_PLAYER_1

#include "audioPlayer.h"

    char data[] = {2, 4, 6, 8, 10};  //array size: 5
    int k = 0;

    audioData_t audioData;
    audioResult_t result;

    audioData.p2audioData = data;
    audioData.audioDataLen = sizeof(data[0]) * 5;
    audioData.audioFormat = AUDIO_MP3;
    audioData.audioTag = ALERTA_0;

    audioData_t readData;
    readData.audioFormat = AUDIO_I2S_STEREO_DECODED;
    readData.audioTag = ALERTA_0;

    result = save_record(&audioData);
    result = read_record(&readData);

    if ((result == AUDIO_SUCCES)  && (readData.p2audioData != 0)){

    	PRINTF("%p\n", readData.p2audioData);
    	for (k = 0; k < 5; k++){
    		PRINTF("%d \n", readData.p2audioData[k]);
    	}

    }
    else{
    	PRINTF("ERROR IN AUDIO PLAYER\n");
    }
#elif DEBUG_AUDIO_PLAYER_2
#include "timer.h"
#include "audioPlayer.h"
#include "mp3data.h"

#include "UDA.h"

    init_UDA();
    timerCounter = 0;
    InitializeTimers();

    audioData_t audioData;
    audioResult_t result;

    audioData.p2audioData = mp3data;
    audioData.audioDataLen = sizeof(mp3data[0]) * 5;
    audioData.audioFormat = AUDIO_MP3;
    audioData.audioTag = ALERTA_0;

    result = save_record(&audioData);
    SetTimer(DEBUG_TIMER, 1, debug_timer_callback);   //1ms de timeout
    start_playing(audioData.audioTag, audioData.audioFormat, AUDIO_I2S_STEREO_DECODED);
    DisableTimer(DEBUG_TIMER);
    printf("decoding spends less than %d ms\n", timerCounter);

#elif DEBUG_SAI_1
#include "fsl_sai_edma.h"
#include "GrabacionEmergencia_wavarray.h"
#include "UDA.h"

    sai_transfer_t xfer[1];
    if (init_UDA() == 0){
    	xfer[0].data =  (uint8_t * )GrabacionEmergencia_wavarray;
    	//uint8_t correction = BUFF_LEN_WAV % 8;
    	xfer[0].dataSize = BUFF_LEN_WAV;
    	SAI_TransferSendEDMA(I2S0_PERIPHERAL, &I2S0_SAI_Tx_eDMA_Handle, xfer);
    }
    else{
    	PRINTF("UDA initializing error\n");
    }


#elif DEBUG_SAI_2

#include "audioPlayer.h"
#include "GrabacionEmergencia_array.h"
#include "UDA.h"
	audioData_t audioData;
	audioData.audioTag = ALERTA_0;
	audioData.p2audioData = GrabacionEmergencia_array;
	audioData.audioDataLen = BUFF_LEN;
	audioData.audioFormat = AUDIO_MP3;

	init_UDA();
	init_audio_player();

	audioResult_t result = save_record(&audioData);
	if (result == AUDIO_SUCCES){
		start_playing(ALERTA_0, AUDIO_MP3, AUDIO_I2S_STEREO_DECODED);
	}
	else{
		PRINTF("ERROR SAVING RECORD\n");
	}

#endif /*DEBUG*/

    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
}

#if DEBUG_SAI_1
void finish_I2S0_Transmit(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData){
	printf("SAI CALLBACK CALLED!!!\n");
	SAI_TransferTerminateSendEDMA(I2S0_PERIPHERAL, &I2S0_SAI_Tx_eDMA_Handle);
	printf("%d\n", handle->dmaHandle->flags);
	printf("%d\n", handle->state);
}


#endif



void debug_timer_callback(void){
	timerCounter++;
}
