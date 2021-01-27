/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "stdlib.h"
#include "fsl_common.h"
#include "fsl_sai.h"
#include "fsl_clock.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Definitions for BOARD_InitPeripherals functional group */
/* Used DMA device. */
#define DMA_DMA_BASEADDR DMA0
/* Associated DMAMUX device that is used for muxing of requests. */
#define DMA_DMAMUX_BASEADDR DMAMUX

  /* Channel audio_transport definitions */
/* DMA eDMA source request. */
#define DMA_AUDIO_TRANSPORT_DMA_REQUEST kDmaRequestMux0I2S0Tx
/* Selected eDMA channel number. */
#define DMA_AUDIO_TRANSPORT_DMA_CHANNEL 0
/* TCD pool size */
#define DMA_AUDIO_TRANSPORT_TCD_SIZE 2
/* DMA interrupt vector ID (number). */
#define DMA_DMA_CH_INT_DONE_0_IRQN DMA0_IRQn
/* DMA interrupt handler identifier. */
#define DMA_DMA_CH_INT_DONE_0_IRQHANDLER DMA0_IRQHandler
/* Definition of peripheral ID */
#define I2S0_PERIPHERAL I2S0
/* I2S0 interrupt vector ID (number). */
#define I2S0_SERIAL_TX_IRQN I2S0_Tx_IRQn
/* I2S0 interrupt handler identifier. */
#define I2S0_SERIAL_TX_IRQHANDLER I2S0_Tx_IRQHandler
/* Master clock source frequency used for counting the master clock divider in the Tx SetFormat type functions, not available on all devices. */
#define I2S0_TX_MCLK_SOURCE_CLOCK_HZ 120000000UL
/* Bit clock source frequency used for counting the bit clock divider in the Tx SetFormat type functions. */
#define I2S0_TX_BCLK_SOURCE_CLOCK_HZ 6144000UL

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
extern const edma_config_t DMA_config;
extern edma_handle_t DMA_audio_transport_Handle;
extern void * p2userdata;
extern const sai_config_t I2S0_tx_config;
extern sai_transfer_format_t I2S0_tx_format;

/***********************************************************************************************************************
 * Callback functions
 **********************************************************************************************************************/
/* eDMA callback function for the 0 channel.*/
extern void callback_dma_config(edma_handle_t*, void*, bool, uint32_t);

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/

void BOARD_InitPeripherals(void);

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void);

#if defined(__cplusplus)
}
#endif

#endif /* _PERIPHERALS_H_ */
