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
void UART3_SERIAL_RX_TX_IRQHANDLER(void);




void copyTXmsg(const uint8_t * msg, uint8_t cant);


uint8_t BTisTxMsgComplete(){
	uint8_t ret = 0;
	ret = UART->C2 & kUART_TransmissionCompleteFlag;
	return ret;
}

uint8_t uartWriteMsg(uint8_t *msg)	//(const char* msg, uint8_t cant)
{
	TXCompleteFlag = false;
	uint8_t cant = strlen(msg);
//	int i = 0;
	uint8_t cantTX = 0;
	_Bool uartWasSleeping = false;

	/*if(uartMode[id] == BLOCKING)
	{
		while(i < cant)	 //me queda algo por transmitir?
		{
			if((UART->S1 & UART_S1_TDRE_MASK)) //puedo transmitir?
			{
				UART_WriteByte(UART, msg[i]);
				i++;
			}
		}
		cantTX = cant;
	}
	else if;*/
	if(lengthTXbuffer == 0){ //no habia nada transmitiendose, recuerdo que debo iniciar la transmision
		uartWasSleeping = true;
	}

	cantTX = TX_BUFFER_LEN - lengthTXbuffer; //cantTX hasta aqui es la cantidad disponible
	cantTX = ( (cant <= cantTX)? cant : cantTX ); //cantTX es el minimo entre la cantidad disponible y la cantidad requerida
	if (cantTX == cant){
		copyTXmsg(msg, cantTX); //se copian cantTX chars del mensaje
		if(  (uartWasSleeping || ~(UART->C2 & UART_C2_TCIE_MASK) ) && (cantTX > 0)) //si la uart estaba durmiendo desde un principio o estaba activaba pero ahora ya termino
		{
			UART->C2 |= UART_C2_TE_MASK;
			UART->C2 |= UART_C2_TIE_MASK; //habilito la interrupcion para el fin de transmisión
			UART->C2 |= UART_C2_TCIE_MASK;	//habilito interrupcion de transmission complete

		}
	}
	else{
		cantTX = 0;
	}
	return cantTX;
}

void UARTX_RX_TX_IRQHandler(){
	//tengo que ver que me llamo a la interrupción
	//para esto, leo el status

	if(UART->S1 & kUART_TxDataRegEmptyFlag){ //se termino una transmisión
		if(lengthTXbuffer > 0){ //si queda algo por transmitir
			UART_WriteByte(UART, TXbuffers[outMarkersTXbuffer]); //transmito otro char del buffer
			uint8_t dummy_read = UART->S1;
			if(outMarkersTXbuffer < TX_BUFFER_LEN -1) //actualizo el outMarker
				outMarkersTXbuffer++;
			else
				outMarkersTXbuffer = 0;

			lengthTXbuffer--; //actualizo el len ya que termino un mensaje
		}
	}
	if(UART->S1 & kUART_TransmissionCompleteFlag){	//si no queda nada por transmitir deshabilito transmisión
		if(lengthTXbuffer == 0){
			UART->C2 = UART->C2 & (~UART_C2_TCIE_MASK);
			UART->C2 = UART->C2 & (~UART_C2_TIE_MASK);
			UART_EnableTx(UART, false);
			TXCompleteFlag = true;
//			PRINTF("transmission complete\n");
		}
	}
//		else{ //si no queda nada por transmitir deshabilito transmisión

//			TXCompleteFlag = true;
//			PRINTF("transmission complete: %x\n", BTisTxMsgComplete());
//		}
}


void copyTXmsg(const uint8_t * msg, uint8_t cant)
{
	uint8_t i = 0;
	for(i = 0; i < cant; i++)
	{
		TXbuffers[inMarkersTXbuffer] = msg[i];
		if(inMarkersTXbuffer < TX_BUFFER_LEN - 1){
			inMarkersTXbuffer++;  //aumento el in siempre que no llegue al final del arreglo
		}
		else{
			inMarkersTXbuffer = 0; //si llego al final del arreglo vuelvo al principio
		}
		lengthTXbuffer++;
	}
}

void UART3_SERIAL_RX_TX_IRQHANDLER(void){
	UARTX_RX_TX_IRQHandler();
}

