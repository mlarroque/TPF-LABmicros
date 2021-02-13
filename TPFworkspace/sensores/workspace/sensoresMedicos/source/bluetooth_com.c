/*
 * bluetooth_com.c
 *
 *  Created on: Jan 24, 2021
 *      Author: Lu
 */

#include "bluetooth_com.h"
#include "uart_cmplx_drv.h"

#define BLUETOOTH_TIMEOUT 20

static SemaphoreHandle_t bluetooth_sem; //Semaforo binario que indica si llegaron muestra
/*!
 * @brief Main function
 */
static void BluetoothCallback(TimerHandle_t){
	BaseType_t woke_flag = pdFALSE;
	xSemaphoreGiveFromISR(bluetooth_sem, &woke_flag);
	portYIELD_FROM_ISR(woke_flag);
}

void InitBluetooth(void){
	TimerHandle_t handler = xTimerCreate("Bluetooth Timer",
				pdMS_TO_TICKS(BLUETOOTH_TIMEOUT),
				pdTRUE,
				NULL,
				init_data->callback);
	xTimerStart(handler, 0);

}

void sendBTPackage(void)
{
	uartWriteMsg(packet);
}

