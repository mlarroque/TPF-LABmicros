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
#include "timer.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

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
    uint16_t fs = 200;
    //ECG_init_t init_data = {200};
    oxi_init_t ox_init_data = {fs};
    InitializeTimers();
    //InitializeECG(&init_data);
    InitializeOximetry(&ox_init_data);


    /* Force the counter to be placed into memory. */
//volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    int32_t reset_value = 4*fs;
    int32_t counter = reset_value;
    int32_t sp02 = 0;
    uint8_t new_samples = 0;
    uint16_t unread_ppg_samp = 0;
    pleth_sample_t samp;
    while(1) {
        //i++ ;
        if(IsOxEvent()){
        	PopOxEvent();
        	new_samples = AddInputSamples();
        	counter -= new_samples;
        	if(counter<=0){
        		CalculateSpO2();
        		sp02 = GetSpO2();
        		//PRINTF("%d \n", sp02);
        		counter = reset_value;
        		unread_ppg_samp = GetUnreadNum();
        		for(int i=0; i<unread_ppg_samp;i++){
        			samp = GetPlethSample();
        			PRINTF("%d \n", samp.red_sample);
        		}
        	}
        	//sample =GetEcgSample();

        	//if(!(--counter)){
        		//counter = 200;
        		//CalculateHeartBeat();
        		//PRINTF("%d \n", GetHeartBeat());
        	//}
        }
    }
    return 0 ;
}
