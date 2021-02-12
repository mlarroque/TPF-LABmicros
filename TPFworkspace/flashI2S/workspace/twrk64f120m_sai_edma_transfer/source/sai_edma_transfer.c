/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
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

#include "board.h"
#include "GrabacionEmergencia_array.h"
#include "GrabacionEmergencia_wavarray.h"
#include "fsl_debug_console.h"

#include "audioPlayer.h"
#include "debug_defs.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*DEBUG definitions*/

#define BUFFER_SIZE (1600U)
#define BUFFER_NUM (2U)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/


AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t buffer[BUFFER_NUM*BUFFER_SIZE], 4);
volatile bool isFinished = false;
volatile uint32_t finishIndex = 0U;
volatile uint32_t emptyBlock = BUFFER_NUM;
/*******************************************************************************
 * Code
 ******************************************************************************/

static void callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
{
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

/*!
 * @brief Main function
 */

void wav_test(void);

int main(void)
{
	PRINTF("SAI example started!\n\r");
    BOARD_audio_init();

#if DEBUG_SAI_EDMA_TRANSFER_1
    init_audio_player(callback, NULL);
    wav_test();
    free_audio_player();
#else

    audioData_t audioData;
    audioData.audioTag = ALERTA_0;
    audioData.p2audioData = GrabacionEmergencia_array;
    audioData.audioDataLen = BUFF_LEN;
    audioData.audioFormat = AUDIO_MP3;

    init_audio_player(NULL, NULL);
    audioResult_t result = save_record(&audioData);
    if (result == AUDIO_SUCCES){
    	start_playing(ALERTA_0, AUDIO_MP3, AUDIO_I2S_STEREO_DECODED);
    	}
    else{
    	PRINTF("ERROR SAVING RECORD\n");
    }

    while(get_player_status() == AUDIO_PROCESSING){
    	PRINTF(". ");
    }

    free_audio_player();


#endif




/* If need to handle audio error, enable sai interrupt */
#if defined(DEMO_SAI_IRQ)
    EnableIRQ(DEMO_SAI_IRQ);
    SAI_TxEnableInterrupts(DEMO_SAI, kSAI_FIFOErrorInterruptEnable);
#endif

    
    /* Once transfer finish, disable SAI instance. */

    PRINTF("\n\r SAI EDMA example finished!\n\r ");
    while (1)
    {
    }
}

#if defined(SAI_ErrorIRQHandler)
void SAI_ErrorIRQHandler(void)
{
    /* Clear the FIFO error flag */
    SAI_TxClearStatusFlags(DEMO_SAI, kSAI_FIFOErrorFlag);

    /* Reset FIFO */
    SAI_TxSoftwareReset(DEMO_SAI, kSAI_ResetTypeFIFO);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

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
