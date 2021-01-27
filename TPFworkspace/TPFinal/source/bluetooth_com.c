/*
 * bluetooth_com.c
 *
 *  Created on: Jan 24, 2021
 *      Author: Lu
 */

#include "bluetooth_com.h"
#include "uart_cmplx_drv.h"


/*!
 * @brief Main function
 */
void sendBTPackage(void)	//BLOQUEANTE!!
{

	uartWriteMsg();
	/*if(modo=bloqueante){
		UART_EnableTx(UART, true);
		static uint32_t jv=0;
		uint8_t max = (sizeof(g_tipString)/sizeof(g_tipString[0]));


		if(UART->S1 & UART_S1_TDRE_MASK){ //puedo transmitir?
			if(jv == max-1)	//no transmito el ultimo caracter porque es un '\0' y es un caracter erroneo
				jv=0;
			UART_WriteByte(UART, g_tipString[jv++]);
		}
	}*/
}

