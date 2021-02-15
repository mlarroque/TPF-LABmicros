/*
 * sai_edma_hal.c
 *
 *  Created on: 11 feb. 2021
 *      Author: Matias
 */


#include "sai_edma_hal.h"
#include "MK64F12.h"
#include "fsl_clock.h"
#include "fsl_dmamux.h"
#include "fsl_common.h"

/* SAI, I2C instance and clock */
#define DEMO_SAI I2S0
#define DEMO_I2C I2C0
#define DEMO_SAI_CLKSRC kCLOCK_CoreSysClk
#define DEMO_SAI_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define DEMO_I2C_CLKSRC kCLOCK_BusClk
#define DEMO_I2C_CLK_FREQ CLOCK_GetFreq(kCLOCK_BusClk)
#define EXAMPLE_DMA (DMA0)
#define EXAMPLE_CHANNEL (0U)
#define EXAMPLE_SAI_TX_SOURCE kDmaRequestMux0I2S0Tx

#define OVER_SAMPLE_RATE (384U)

void init_sai_format(void);



uint32_t mclkSourceClockHz = 0U;

AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t txHandle) = {0};
edma_handle_t dmaHandle = {0};

sai_config_t config;
sai_transfer_format_t format;
edma_config_t dmaConfig = {0};

void sai_edma_init(sai_edma_callback_t callback, void *userData){

	init_sai_format();

	EDMA_GetDefaultConfig(&dmaConfig);
	EDMA_Init(EXAMPLE_DMA, &dmaConfig);
	EDMA_CreateHandle(&dmaHandle, EXAMPLE_DMA, EXAMPLE_CHANNEL);

	DMAMUX_Init(DMAMUX0);
	DMAMUX_SetSource(DMAMUX0, EXAMPLE_CHANNEL, EXAMPLE_SAI_TX_SOURCE);
	DMAMUX_EnableChannel(DMAMUX0, EXAMPLE_CHANNEL);

	SAI_TxGetDefaultConfig(&config);

	SAI_TxInit(DEMO_SAI, &config);
	SAI_TransferTxCreateHandleEDMA(DEMO_SAI, &txHandle, callback, NULL, &dmaHandle);
	mclkSourceClockHz = DEMO_SAI_CLK_FREQ;
	SAI_TransferTxSetFormatEDMA(DEMO_SAI, &txHandle, &format, mclkSourceClockHz, format.masterClockHz);

}

void sai_edma_reset(sai_edma_callback_t callback, void *userData){
    /* Enable the channel FIFO */
    DEMO_SAI->RCR3 |= I2S_RCR3_RCE_MASK;

    /* Enable DMA enable bit */
    SAI_TxEnableDMA(DEMO_SAI, kSAI_FIFORequestDMAEnable, true);

    /* Enable Tx */
    SAI_TxEnable(DEMO_SAI, true);

}

void init_sai_format(void){
	/* Configure the audio format */
	format.bitWidth = kSAI_WordWidth16bits;
	format.channel = 0U;
	format.sampleRate_Hz = kSAI_SampleRate16KHz;
	format.masterClockHz = OVER_SAMPLE_RATE * format.sampleRate_Hz;
	format.protocol = config.protocol;
	format.stereo = kSAI_Stereo;
	format.isFrameSyncCompact = false;
	format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
}

uint32_t getMasterClockHz(void){
	return format.masterClockHz;
}
uint32_t getSampleRateHz(void){
	return format.sampleRate_Hz;
}
uint32_t getBitWidth(void){
	return format.bitWidth;
}

status_t sendSAIdata(sai_transfer_t * p2xfer){
	return SAI_TransferSendEDMA(DEMO_SAI, &txHandle,p2xfer);
}

void stopSAIsend(void){
	SAI_TransferAbortSendEDMA(DEMO_SAI, &txHandle);
}
void sai_deinit(void){
	SAI_Deinit(DEMO_SAI);
}



