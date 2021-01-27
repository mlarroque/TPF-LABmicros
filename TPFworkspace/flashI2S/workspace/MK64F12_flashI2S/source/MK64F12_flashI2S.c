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
#include "flashHal.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
#define DEBUG_FLASH_1 1
#define DEBUG_FLASH_2 0
#define DEBUG_SAI_1 0
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
    char data[] = {2, 4, 6, 8};  //array size: 5
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


#elif DEBUG_SAI_1

#endif /*DEBUG_FLASH_1, DEBUG_SAI_1*/

    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
}
