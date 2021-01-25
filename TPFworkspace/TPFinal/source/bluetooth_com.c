/*
 * bluetooth_com.c
 *
 *  Created on: Jan 24, 2021
 *      Author: Lu
 */

#include "bluetooth_com.h"
#include "fsl_uart.h"
#include "peripherals.h"
#include "fsl_debug_console.h" //TODO es solo para debug

#include "uart_interrupt_transfer.h"

typedef void (*uart_isr_t)(UART_Type *base, uart_handle_t *handle);



//init(){
//	UART_TransferCreateHandle(UART_Type *base,
//	                               uart_handle_t *handle,
//	                               uart_transfer_callback_t callback,
//	                               void *userData)
//}

void sendBTDataTEST(void)
{
	UART_WriteByte(UART3_PERIPHERAL, 8);
	//uart_main(); //TODO cambiar UART3_PERIPHERAL POR ALGO INVISIBLE TIPO GET INSTANCE
}



void UART3_SERIAL_RX_TX_IRQHANDLER(void) {
  uint32_t intStatus;
   //Reading all interrupt flags of status registers
  intStatus = UART_GetStatusFlags(UART3_PERIPHERAL);
  intStatus &= ~(kUART_RxOverrunFlag | kUART_NoiseErrorFlag | kUART_TxFifoOverflowFlag | kUART_TxFifoEmptyFlag);
  status_t status = UART_ClearStatusFlags(UART3_PERIPHERAL, intStatus);

   //Flags can be cleared by reading the status register and reading/writing data registers.
    //See the reference manual for details of each flag.
    //The UART_ClearStatusFlags() function can be also used for clearing of flags in case the content of data regsiter is not used.
    //For example:
    //    status_t status;
    //    intStatus &= ~(kUART_RxOverrunFlag | kUART_NoiseErrorFlag | kUART_FramingErrorFlag | kUART_ParityErrorFlag);
    //    status = UART_ClearStatusFlags(UART3_PERIPHERAL, intStatus);


  PRINTF("UART3_SERIAL_RX_TX_IRQHANDLER\n");


}
