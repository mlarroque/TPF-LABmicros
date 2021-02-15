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
void sendSpO2Packet(int32_t spo2);
void sendPPGPacket(int32_t* samples, uint8_t n);
void sendECGPacket(int32_t* samples, uint8_t n);
void sendTempPacket(int16_t temp);
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
	//sendHeartRatePacket(pkg.heartRate);
	//sendSpO2Packet(pkg.sp02);
	//sendTempPacket(pkg.temp);
	sendECGPacket(pkg.ecg_samples, pkg.n_samples_ecg);
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
	uint8_t packetO[5];
	packetO[0] = 'O';
	packetO[1] = (uint8_t) 0x00;
	packetO[2] = (uint8_t)spo2;
	packetO[3] = 4;	//checksum
	packetO[4] = '\r';	//terminador

	uartWriteMsg(packetO, 5);
}

void sendTempPacket(int16_t temp){
	uint8_t packetT[6];
	packetT[0] = 'T';
	packetT[1] = (uint8_t) 0x00;
	uint8_t parte_alta = temp>>8;
	uint8_t parte_baja = (uint8_t) temp;
	packetT[2] = parte_alta;
	packetT[3] = parte_baja;
	packetT[4] = 5;	//checksum
	packetT[5] = '\r';	//terminador

	uartWriteMsg(packetT, 6);
}

void sendECGPacket(int32_t* samples, uint8_t n){
	if(n>0){
		uint8_t size = 1+1+1+1+2*n;
		uint8_t packetECG[20];
		packetECG[0] = 'E';
		packetECG[1] = n;
		for(int i=0; i<n; i++){
			uint8_t parte_alta = samples[i]>>8;
			uint8_t parte_baja = (uint8_t) samples[i];
			packetECG[2+2*i] = parte_alta;
			packetECG[2+2*i+1] = parte_baja;
		}
		packetECG[size-2] = size-1;	//checksum
		packetECG[size-1] = '\r';	//terminador

		uartWriteMsg(packetECG, size);
	}

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
		packetPPG[size-2] = size-1;	//checksum
		packetPPG[size-1] = '\r';	//terminador

		uartWriteMsg(packetPPG, size);
	}

}


