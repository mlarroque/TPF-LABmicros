/*
 * uart_cmplx_drv.c
 *
 *  Created on: Jan 27, 2021
 *      Author: Lu
 */


#include "fsl_uart.h"
#include "peripherals.h"
#include "fsl_debug_console.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* UART instance and clock */
#define UART UART3_PERIPHERAL
#define TX_BUFFER_LEN 500
#define TX_MESSAGE_MAX_LEN 8
/*******************************************************************************
 * Prototypes
 ******************************************************************************/



/*******************************************************************************
 * Variables
 ******************************************************************************/

_Bool TXCompleteFlag = false;

static uint8_t TXbuffers[TX_BUFFER_LEN];
static uint8_t lengthTXbuffer = 0;
static uint8_t outMarkersTXbuffer = 0;
static uint8_t inMarkersTXbuffer= 0;


/*******************************************************************************
 * Code
 ******************************************************************************/

int uartWriteMsg(uint8_t *msg, uint32_t size)	//(const char* msg, uint8_t cant)
{
	return UART_RTOS_Send(&UART3_rtos_handle, msg, size);
}

