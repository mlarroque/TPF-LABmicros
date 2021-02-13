/*
 * bluetooth_com.c
 *
 *  Created on: Jan 24, 2021
 *      Author: Lu
 */

#include "bluetooth_com.h"
#include "uart_cmplx_drv.h"
#include "fsl_debug_console.h"
//FreeRTOS includes
#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"

#define BLUETOOTH_TIMEOUT 20


static SemaphoreHandle_t bluetooth_sem; //Semaforo binario que indica si llegaron muestra

void sendHeartRatePacket(int32_t heartRate);

/*!
 * @brief Main function
 */
static void BluetoothCallback(TimerHandle_t handle){
	BaseType_t woke_flag = pdFALSE;
	xSemaphoreGiveFromISR(bluetooth_sem, &woke_flag);
	portYIELD_FROM_ISR(woke_flag);
}

void InitBluetooth(void){
	bluetooth_sem = xSemaphoreCreateBinary();
	TimerHandle_t handler = xTimerCreate("Bluetooth Timer",
				pdMS_TO_TICKS(BLUETOOTH_TIMEOUT),
				pdTRUE,
				NULL,
				BluetoothCallback);
	if( handler == NULL ){
		PRINTF("Handler is NULL");
	}
	xTimerStart(handler, 0);
	if( xTimerIsTimerActive(handler) == pdFALSE ){
		PRINTF("Timer not active");
	}

}

void BlueWaitForSamples(void){
	xSemaphoreTake( bluetooth_sem, portMAX_DELAY );
}

void sendBTPackage(int32_t heartRate, int32_t sp02, uint16_t temp, int32_t* ecg_samples, int32_t* ox_samples)
{
	sendHeartRatePacket(heartRate);
	sendSpO2Packet();
	sendTempPacket();
	sendECGPacket();
	sendPPGPacket();

}

void sendHeartRatePacket(int32_t heartRate){
	char packetH[5];
	packetH[0] = 'H';
	packetH[1] = (char) 0x00;
	packetH[2] = (char) ((uint8_t)heartRate);
	packetH[3] = 4;	//checksum
	packetH[4] = '\r';	//terminador

	uartWriteMsg(packetH, 5);
}

void sendSpO2Packet(){

}

void sendTempPacket(){

}

void sendECGPacket(){

}

void sendPPGPacket(){

}


