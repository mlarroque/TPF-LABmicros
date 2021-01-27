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
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* UART instance and clock */
#define UART UART3_PERIPHERAL
#define TX_BUFFER_LEN 200
#define TX_MESSAGE_MAX_LEN 8
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* UART user callback */
//void UART_UserCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData);
void userCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData);


/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t g_tipString[] = "Hola soy un test\n";

static char TXbuffers[TX_BUFFER_LEN];
static uint8_t lengthTXbuffer = 20;
static uint8_t outMarkersTXbuffer = 20;
static uint8_t inMarkersTXbuffer= 20;

struct userData
{
    uint8_t *data;   /*!< The buffer of data to be transfer.*/
    size_t dataSize; /*!< The byte count to be transfer. */
};


void *userData;
/*******************************************************************************
 * Code
 ******************************************************************************/
//extern void UART3_DriverIRQHandler(void);



/*
void userCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData)
{
    userData = userData;
    PRINTF("va benne\n");
    if (kStatus_UART_TxIdle == status)
    {
        txBufferFull = false;
        txOnGoing    = false;

    }

    if (kStatus_UART_RxIdle == status)
    {
        rxBufferEmpty = false;
        rxOnGoing     = false;
    }
}
*/
void copyTXmsg(const uint8_t * msg, uint8_t cant);



uint8_t BTisTxMsgComplete(){
	uint8_t ret = 0;
	ret = UART->C2 & kUART_TransmissionCompleteInterruptEnable;
	return ret;
}

uint8_t uartWriteMsg()	//(const char* msg, uint8_t cant)
{
	uint8_t* msg = g_tipString;
	uint8_t cant = sizeof(g_tipString)/sizeof(g_tipString[0]);
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

	uint8_t i = 0;

	if(UART->S1 & kUART_TxDataRegEmptyFlag){ //se termino una transmisión
		if(true){ //si queda algo por transmitir
			UART_WriteByte(UART, TXbuffers[outMarkersTXbuffer]); //transmito otro char del buffer
			if(outMarkersTXbuffer < TX_BUFFER_LEN -1){ //actualizo el outMarker
				outMarkersTXbuffer++;
			}
			else{
				outMarkersTXbuffer = 0;
			}
			lengthTXbuffer--; //actualizo el len ya que termino un mensaje
			i++;

		}
		else if(UART->S1 & kUART_TransmissionCompleteFlag){

		}
		else{ //si no queda nada por transmitir deshabilito transmisión
			UART_EnableTx(UART, false);
			UART->C2 = UART->C2 & (~UART_C2_TIE_MASK);

		}

	}
}


void copyTXmsg(const uint8_t * msg, uint8_t cant)
{
	uint8_t i = 0;
	for(i = 0; i < cant; i++)
	{
		TXbuffers[inMarkersTXbuffer] = msg[i];
		if(inMarkersTXbuffer < TX_BUFFER_LEN - 1)
		{
			inMarkersTXbuffer++;  //aumento el in siempre que no llegue al final del arreglo
		}
		else
		{
			inMarkersTXbuffer = 0; //si llego al final del arreglo vuelvo al principio
		}
		lengthTXbuffer++;
	}
}

void UART3_SERIAL_RX_TX_IRQHANDLER(void){
	UARTX_RX_TX_IRQHandler();
}

/* UART3_RX_TX_IRQn interrupt handler
void UART3_SERIAL_RX_TX_IRQHANDLER(void) {
	uint32_t intStatus;
	 Reading all interrupt flags of status registers
	intStatus = UART_GetStatusFlags(UART);

	status_t status;
	intStatus &= ~(kUART_TransmissionCompleteFlag | kUART_TxDataRegEmptyFlag);
	status = UART_ClearStatusFlags(UART, intStatus);

	if(intStatus && kUART_TransmissionCompleteFlag)
		PRINTF("llegó?\n");

	 Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
	Store immediate overlapping exception return operation might vector to incorrect interrupt.
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}*/


/*!
 * @brief Main function
 */
void sendBTData(void)	//BLOQUEANTE!!
{
	UART_EnableTx(UART, true);
	static uint32_t jv=0;
	uint8_t max = (sizeof(g_tipString)/sizeof(g_tipString[0]));


	if(UART->S1 & UART_S1_TDRE_MASK){ //puedo transmitir?
		if(jv == max-1)	//no transmito el ultimo caracter porque es un '\0' y es un caracter erroneo
			jv=0;
		UART_WriteByte(UART, g_tipString[jv++]);
	}
}

