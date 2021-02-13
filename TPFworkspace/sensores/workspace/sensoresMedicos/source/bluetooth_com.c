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

void sendBTPackage(data_BT_t pkg)
{
	sendHeartRatePacket(pkg.heartRate);
	sendSpO2Packet(pkg.sp02);
	sendTempPacket(pkg.temp);
	sendECGPacket(pkg.ecg_samples, pkg.n_samples_ecg);
	sendPPGPacket(pkg.ox_samples, pkg.n_samples_ppg);

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

void sendSpO2Packet(int32_t spo2){

}

void sendTempPacket(int16_t temp){

}

void sendECGPacket(int32_t* samples, uint8_t n){

}

void sendPPGPacket(int32_t* samples, uint8_t n){
	uint8_t size = 1+1+1+1+2*n;
	uint16_t samples16 = (uint16_t) samples;
	char packetPPG[20];
	packetPPG[0] = 'P';
	packetPPG[1] = n;
	for(int i=2; i<n+2; i++){
		uint8_t parte_alta = samples[i]>>8;
		uint8_t parte_baja = (uint8_t) samples[i];
		packetPPG[i] = parte_alta;
		packetPPG[i+1] = parte_baja;
	}
	packetPPG[size-2] = size;	//checksum
	packetPPG[size-1] = '\r';	//terminador

	uartWriteMsg(packetPPG, size);
}


