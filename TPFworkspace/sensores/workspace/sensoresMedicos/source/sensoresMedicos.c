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
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "event_prueba.h"
#include "ecg.h"
#include "ox_event.h"
#include "oximetry.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "projdefs.h"

#include "thermometer.h"
#include "temp_event.h"

/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */

void prvSetupHardware(void);
void mainTask(void*);

//Parametros de inicializacion
static oxi_init_t ox_data = {
		.fs=200
};

int main(void) {
	/* Perform any hardware setup necessary. */
	prvSetupHardware();

	/* App Tasks */
	BaseType_t status = xTaskCreate(mainTask,
			"mainTask",
			((unsigned short)800),
			NULL,
			1,
			NULL);

	if(status == pdPASS){
		PRINTF("Task Created\n");
	}
	else if(status == pdFAIL){
		PRINTF("Task Creation Failed\n");
	}
	else{
		PRINTF("%d\n", status);
	}

	/* Start the created tasks running. */
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
    PRINTF("Main Starting\n");
    //InitializeThermometer();
}

void mainTask(void* params){
	uint8_t n_samples = 0;
	NVIC_SetPriority(I2C0_IRQn, 2);
	InitializeOximetry(&ox_data);
	while(1){
		if(IsOxEvent()){
			PopOxEvent();
			n_samples = AddInputSamples();
		}
		/*
	    if(IsTempEvent()){
	    	PopTempEvent();
	    	AddTempInputSample();
	    	newSampleRequest();
	    }
	    */
	}
}
