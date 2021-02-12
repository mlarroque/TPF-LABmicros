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

#define MAX_HB 120
#define MIN_HB 60
#define MAX_TEMP 0//??
#define MIN_TEMP 0//??
#define MAX_SPO2 0//??
#define MIN_SPO2 0//??
#define INIT_ECG_COUNTER 0//??
#define INIT_OX_COUNTER 0//??


void prvSetupHardware(void);

void procTask(void);
void tempTask(void);
void audioTask(void);
void transmiterTask(void);


int main(void) {
	/* Perform any hardware setup necessary. */
	prvSetupHardware();
	/* Thermometer Task */
	if(xTaskCreate(tempTask, "Temperature Thread", 500, NULL, 1, NULL) != pdPASS){
		PRINTF("Task Receiver creation failed!.\r\n");
	}
	/* Audio Task */
	if(xTaskCreate(audioTask, "Audio Thread", 500, NULL, 1, NULL) != pdPASS){
		PRINTF("Task Receiver creation failed!.\r\n");
	}
	/* Transmiter Task */
	if(xTaskCreate(transmiterTask, "Transmission Thread", 500, NULL, 1, NULL) != pdPASS){
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

void procTask(void){
	ECG_init_t ECG_init;
	oxi_init_t oxi_init;
	InitializeECG(&ECG_init);
	InitializeOximetry(&oxi_init);
	while(1){
//			AddInputSamples(oxUd.sample_red, oxUd.sample_ir);
//			if(oxCounter == 0){
//				oxCounter = INIT_OX_COUNTER;
//				CalculateSpO2();
//				int32_t spo2 = GetSpO2();
//				if((spo2 > MAX_SPO2) || (spo2 < MIN_SPO2)){
//					// DESPERTAR THREAD DE AUDIO
//				}
//			}
//			else{
//				ecgCounter = ecgCounter-1;
//			}
//			AddEcgSample(ecgUd.sample);
//			if(ecgCounter == 0){
//				ecgCounter = INIT_ECG_COUNTER;
//				CalculateHeartBeat();
//				uint16_t hb = GetHeartBeat();
//				if((hb > MAX_HB) || (hb < MIN_HB)){
//					// DESPERTAR THREAD DE AUDIO
//				}
//			}
//			else{
//				ecgCounter = ecgCounter-1;
//			}
	}
}

void tempTask(void){
	InitializeThermometer();
	uint16_t temp;
	while(1){
		/* Semaforo que se prende en el timer */
		AddTempInputSample();
		temp = getTemperature();
		if((temp > MAX_TEMP) || (temp < MIN_TEMP)){
			// DESPERTAR THREAD DE AUDIO
		}
		newSampleRequest();
		/* Volver a bloqear thread hasta nuevo semaforo */
	}
}

void audioTask(void)
{
	// Init Bluethoot
	while(1){
		//terminar
	}
}

void transmiterTask(void)
{
	// Init Bluethoot
	while(1){
		//terminar
	}
}
