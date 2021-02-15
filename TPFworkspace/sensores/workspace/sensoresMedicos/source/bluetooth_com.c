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
void sendPPGPacket(int32_t* samples, uint8_t n);
/*!
 * @brief Main function
 */
static void BluetoothCallback(TimerHandle_t handle){
	//xSemaphoreGive(bluetooth_sem);
	BaseType_t woke_flag = pdFALSE;
	xSemaphoreGiveFromISR(bluetooth_sem, &woke_flag);
	portYIELD_FROM_ISR(woke_flag);
}

void InitBluetooth(void){
	bluetooth_sem = xSemaphoreCreateBinary();
	if( bluetooth_sem == NULL ){
		PRINTF("Blu semphr is NULL \n");
	}
	//xSemaphoreGive(bluetooth_sem);
	TimerHandle_t handler = xTimerCreate("Bluetooth Timer",
				pdMS_TO_TICKS(BLUETOOTH_TIMEOUT),
				pdTRUE,
				NULL,
				BluetoothCallback);
	if( handler == NULL ){
		PRINTF("Handler is NULL \n");
	}
	xTimerStart(handler, 0);
	if( xTimerIsTimerActive(handler) == pdFALSE ){
		PRINTF("Timer not active \n");
	}

}

void BlueWaitForSamples(void){
	xSemaphoreTake( bluetooth_sem, portMAX_DELAY );
}

void sendBTPackage(data_BT_t pkg)
{
	sendHeartRatePacket(pkg.heartRate);
	//sendSpO2Packet(pkg.sp02);
	//sendTempPacket(pkg.temp);
	//sendECGPacket(pkg.ecg_samples, pkg.n_samples_ecg);
	//sendPPGPacket(pkg.ox_samples, pkg.n_samples_ppg);

}

void sendHeartRatePacket(int32_t heartRate){
	uint8_t packetH[5];
	packetH[0] = 'H';
	packetH[1] = (uint8_t) 0x00;
	packetH[2] = (uint8_t)heartRate;
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
	if(n>0){
		uint8_t size = 1+1+1+1+2*n;
		uint8_t packetPPG[20];
		packetPPG[0] = 'P';
		packetPPG[1] = n;
		for(int i=0; i<n; i++){
			uint8_t parte_alta = samples[i]>>8;
			uint8_t parte_baja = (uint8_t) samples[i];
			packetPPG[2+2*i] = parte_alta;
			packetPPG[2+2*i+1] = parte_baja;
		}
		packetPPG[size-2] = size;	//checksum
		packetPPG[size-1] = '\r';	//terminador

		uartWriteMsg(packetPPG, size);
	}

}


