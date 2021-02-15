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
 * @file    sensoresMedicos.c
 * @brief   Application entry point.
 */

/* Board/Debug */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "projdefs.h"
#include "semphr.h"

/* Peripherals */
#include "thermometer.h"
#include "ecg.h"
#include "oximetry.h"

#include "audioPlayer.h"
#include "debug_defs.h"
#include "GrabacionEmergencia_array.h"

/*
 * @brief   Application entry point.
 */

#define MAX_HB 180 //??
#define MIN_HB 40	//??
#define MAX_TEMP 50//??
#define MIN_TEMP 0//??
#define MAX_SPO2 110//??
#define MIN_SPO2 10//??
#define OX_COUNTER_INIT (50*4)	//Cada cuantas muestras se actualiza el SP02
#define TEMP_UPDATE_TIME 10000 //Cada cuanto se lee la temperatura en ms.

#define BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL 0


void prvSetupHardware(void);

//void procTask(void*);
void tempTask(void*);
void audioTask(void*);
//void transmiterTask(void*);

static SemaphoreHandle_t Audio_sem;

#if DEBUG_SAI_EDMA_TRANSFER_1
#include "GrabacionEmergencia_wavarray.h"
#define BUFFER_SIZE (1600U)
#define BUFFER_NUM (2U)

AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t buffer[BUFFER_NUM*BUFFER_SIZE], 4);
volatile bool isFinished = false;
volatile uint32_t finishIndex = 0U;
volatile uint32_t emptyBlock = BUFFER_NUM;

void wav_test(void);

static void callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

static void callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
{
	//printf("entro callback\n");
    if(kStatus_SAI_RxError == status)
    {
    }
    else
    {

        finishIndex++;
        emptyBlock++;
        /* Judge whether the music array is completely transfered. */
        if(BUFF_LEN_WAV/BUFFER_SIZE == finishIndex)
        {
            isFinished = true;
        }

        //printf("flags: sai: %d, edma: %d\n", handle->state, handle->dmaHandle->flags);
    }
}

#endif

int main(void) {
	/* Perform any hardware setup necessary. */
	prvSetupHardware();

	/* Thermometer Task */
	if(xTaskCreate(tempTask, "Temperature Thread", 400, NULL, 1, NULL) != pdPASS){
		PRINTF("Task Temp creation failed!.\r\n");
	}
//	/* ECG and Oximetry thread*/
//	if(xTaskCreate(procTask, "Oximetry and ECG Thread", 1100, NULL, 1, NULL) != pdPASS){
//		PRINTF("Task Proc creation failed!.\r\n");
//	}
	/* Audio Task */
	if(xTaskCreate(audioTask, "Audio Thread", 400, NULL, 1, NULL) != pdPASS){
		PRINTF("Task Audio creation failed!.\r\n");
	}
//	/* Transmiter Task */
//	if(xTaskCreate(transmiterTask, "Transmission Thread", 200, NULL, 1, NULL) != pdPASS){
//		PRINTF("Task Trans creation failed!.\r\n");
//	}

	Audio_sem = xSemaphoreCreateBinary();
	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	return 0;
}

void prvSetupHardware(void){
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
	#endif
	/* Set interrupt priorities */
	NVIC_SetPriority(I2C0_IRQn, 2);
	NVIC_SetPriority(DMA0_IRQn, 2);
	NVIC_SetPriority(I2S0_Tx_IRQn, 2);

	//BOARD_audio_init(); //esta funcion esta en board.h del proyecto de audioPlayer. Falta Mergear.
	audioData_t audioData = {
			.audioTag = ALERTA_0,
			.p2audioData = GrabacionEmergencia_array,
			.audioDataLen = BUFF_LEN,
	        .audioFormat = AUDIO_MP3
	};
#if DEBUG_SAI_EDMA_TRANSFER_1
	init_audio_player(callback, NULL);
#else
	init_audio_player(NULL, NULL);
#endif

	flashINIT();
	//audioResult_t result = save_record(&audioData);
	//PRINTF("%d \n", result);
    PRINTF("Hardware Setup Finished\n");
}
\
//void procTask(void* params){
//	ECG_init_t ECG_init = {.fs=200};
//	oxi_init_t oxi_init = {.fs=50};
//	InitializeECG(&ECG_init);
//	InitializeOximetry(&oxi_init);
//
//	int16_t ox_counter = OX_COUNTER_INIT;
//	uint8_t n_samples = 0;
//	while(1){
//		WaitForSamples();
//		n_samples = AddInputSamples();
//		ox_counter -= n_samples;
//		if( ox_counter <= 0 ){
//			ox_counter = OX_COUNTER_INIT;
//			CalculateSpO2();
//			int32_t spo2 = GetSpO2();
//			if((spo2 > MAX_SPO2) || (spo2 < MIN_SPO2)){
//				xSemaphoreGive( Audio_sem );
//			}
//		}
//	}
//}

void tempTask(void* params){
//	InitializeThermometer();
//	uint16_t temp;
//	while(1){
//		vTaskDelay( pdMS_TO_TICKS(TEMP_UPDATE_TIME) );/* Bloquea la thread por TEMP_UPDATE_TIME ms*/
//		AddTempInputSample();
//		temp = getTemperature();
//		if((temp > MAX_TEMP) || (temp < MIN_TEMP)){
//			xSemaphoreGive( Audio_sem );
//		}
//		newSampleRequest();
//	}
	while(1){
		xSemaphoreGive(Audio_sem);
		vTaskDelay(pdMS_TO_TICKS(8000));// Delay entre reproducciones
	}
}

void audioTask(void* params)
{
	init_audio_RTOS();
	while(1){
		xSemaphoreTake(Audio_sem, portMAX_DELAY);

#if DEBUG_SAI_EDMA_TRANSFER_1
		wav_test();
#else

		start_playing(ALERTA_0, AUDIO_MP3, AUDIO_I2S_STEREO_DECODED);
#endif

		vTaskDelay( pdMS_TO_TICKS(5000) );// Delay entre reproducciones
	}
	free_audio_player();
}

//void transmiterTask(void* params)
//{
//	// Init Bluetooth
//	while(1){
//		vTaskDelay( pdMS_TO_TICKS(1000) );
//		PRINTF("HR: %d \n",GetHeartRate());
//		PRINTF("SP02: %d \n",GetSpO2());
//		PRINTF("T: %d \n",GetThermoSample());
//	}
//}

#if DEBUG_SAI_EDMA_TRANSFER_1

void wav_test(void){
	sai_transfer_t xfer;

	uint32_t cpy_index = 0U, tx_index=0U;
	/* Waiting until finished. */
	while(!isFinished)
	{
	   if((emptyBlock > 0U) && (cpy_index < BUFF_LEN_WAV/BUFFER_SIZE))
	   {
	      /* Fill in the buffers. */
	      memcpy((uint8_t *)&buffer[BUFFER_SIZE*(cpy_index%BUFFER_NUM)],(uint8_t *)&GrabacionEmergencia_wavarray[cpy_index*BUFFER_SIZE],sizeof(uint8_t)*BUFFER_SIZE);
	      emptyBlock--;
	      cpy_index++;
	   }
	   if(emptyBlock < BUFFER_NUM)
	   {
	     /*  xfer structure */
	     xfer.data = (uint8_t *)&buffer[BUFFER_SIZE*(tx_index%BUFFER_NUM)];
	     xfer.dataSize = BUFFER_SIZE;
	     /* Wait for available queue. */
	     if(kStatus_Success == sendSAIdata(&xfer))
	     {
	    	 tx_index++;
	     }
	   }
	}
}
#endif
