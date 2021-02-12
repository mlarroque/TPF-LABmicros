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

/* Peripherals */
#include "thermometer.h"
#include "ecg.h"
#include "oximetry.h"

/*
 * @brief   Application entry point.
 */

#define MAX_HB 180
#define MIN_HB 60
#define MAX_TEMP 0//??
#define MIN_TEMP 0//??
#define MAX_SPO2 0//??
#define MIN_SPO2 0//??
#define OX_COUNTER_INIT (50*4)	//Cada cuantas muestras se actualiza el SP02
#define TEMP_UPDATE_TIME 1000 //Cada cuanto se lee la temperatura en ms.


void prvSetupHardware(void);

void procTask(void*);
void tempTask(void*);
void audioTask(void*);
void transmiterTask(void*);


int main(void) {
	/* Perform any hardware setup necessary. */
	prvSetupHardware();
	/* Thermometer Task */
	if(xTaskCreate(tempTask, "Temperature Thread", 200, NULL, 1, NULL) != pdPASS){
		PRINTF("Task Receiver creation failed!.\r\n");
	}
	/* ECG and Oximetry thread*/
	if(xTaskCreate(procTask, "Oximetry and ECG Thread", 1500, NULL, 1, NULL) != pdPASS){
		PRINTF("Task Receiver creation failed!.\r\n");
	}
	/* Audio Task */
	if(xTaskCreate(audioTask, "Audio Thread", 100, NULL, 1, NULL) != pdPASS){
		PRINTF("Task Receiver creation failed!.\r\n");
	}
	/* Transmiter Task */
	if(xTaskCreate(transmiterTask, "Transmission Thread", 100, NULL, 1, NULL) != pdPASS){
		PRINTF("Task Receiver creation failed!.\r\n");
	}
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
    PRINTF("Hardware Setup Finished\n");
}

void procTask(void*){
	ECG_init_t ECG_init = {.fs=200};
	oxi_init_t oxi_init = {.fs=50};
	InitializeECG(&ECG_init);
	InitializeOximetry(&oxi_init);

	int16_t ox_counter = OX_COUNTER_INIT;
	uint8_t n_samples = 0;
	while(1){
		n_samples = AddInputSamples();
		ox_counter -= n_samples;
		if( ox_counter <= 0 ){
			ox_counter = OX_COUNTER_INIT;
			CalculateSpO2();
			int32_t spo2 = GetSpO2();
			if((spo2 > MAX_SPO2) || (spo2 < MIN_SPO2)){
				// DESPERTAR THREAD DE AUDIO
			}
		}
	}
}

void tempTask(void*){
	InitializeThermometer();
	uint16_t temp;
	while(1){
		vTaskDelay( pdMS_TO_TICKS(TEMP_UPDATE_TIME) );/* Bloquea la thread por TEMP_UPDATE_TIME ms*/
		AddTempInputSample();
		temp = getTemperature();
		if((temp > MAX_TEMP) || (temp < MIN_TEMP)){
			// DESPERTAR THREAD DE AUDIO
		}
		newSampleRequest();
	}
}

void audioTask(void*)
{
	// Init Bluethoot
	while(1){
		//terminar
	}
}

void transmiterTask(void*)
{
	// Init Bluethoot
	while(1){
		//terminar
	}
}
