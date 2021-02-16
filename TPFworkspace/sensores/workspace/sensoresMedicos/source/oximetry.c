/*
 * oximetry.c
 *
 *  Created on: 24 Jan 2021
 *      Author: G5
 */

/********************************************************
 *					 HEADERS UTILIZADOS					*
 ********************************************************/
#include "oximetry.h"
#include "spo2Algorithm.h"
#include "max30102.h"
#include <stdint.h>
#include <stdbool.h>
#include "fsl_debug_console.h"
//FreeRTOS headers
#include "FreeRTOS.h"
#include "semphr.h"

/********************************************************
 * 						DEFINCIONES						*
 ********************************************************/
#define SAMPLE_BATCH_SIZE 15 //se agregan de a 15 muestras a la vez en el buffer
#define MAX_NUM_SAMPLES	32 //Maximo numero de muestras a leer seguidas

/********************************************************
 * 					VARIABLES GLOBALES					*
 ********************************************************/
static uint16_t fs;
static int32_t Sp02 = 0;
static int32_t HeartRate = 0;
static ppg_sample_t RedInput[BUFFER_SIZE];	//Buffer circular que guarda inputs del led rojo.
static ppg_sample_t IrInput[BUFFER_SIZE];	//Buffer circular que guarda inputs del led ir.

static uint16_t start = 0; //Indice donde se encuentra la muestra mas vieja.
static uint16_t curr = 0; //Indice con la muestra mas vieja sin leer del pleth
static uint16_t unread_samples = 0;
static uint16_t raw_samples_rx = 0; //Cantidad de muestras sin procesar almacenadas

int32_t RedPleth[BUFFER_SIZE];
int32_t IrPleth[BUFFER_SIZE];

static SemaphoreHandle_t ox_mutex; //Protege recursos compartidos
static SemaphoreHandle_t ox_sem; //Semaforo binario que indica si llegaron muestras

/********************************************************
 * 					FUNCIONES LOCALES					*
 ********************************************************/
void OximeterCallback(TimerHandle_t handle){
	BaseType_t woke_flag = pdFALSE;
	xSemaphoreGiveFromISR(ox_sem, &woke_flag);
	portYIELD_FROM_ISR(woke_flag);
}

/********************************************************
 * 					FUNCIONES DEL HEADER				*
 ********************************************************/
void InitializeOximetry(oxi_init_t* init_data){
	fs = init_data->fs;
	ox_mutex = xSemaphoreCreateMutex();
	ox_sem = xSemaphoreCreateBinary();
	unsigned long int timeout = (SAMPLE_BATCH_SIZE*1000)/fs;
	max_init_t hard_init = {OximeterCallback,timeout};
	InitializeOxHardware(&hard_init);
}

void CalculateSpO2(void){
	float result = 0;
	int32_t hr_aux = 0;
	int8_t valid = 0;
	int8_t hr_valid = 0;
	float ratio = 0;
	float correl = 0;

	xSemaphoreTake( ox_mutex, portMAX_DELAY ); //Bloquea recurso compartido
	rf_heart_rate_and_oxygen_saturation(IrInput, BUFFER_SIZE, RedInput, &result, &valid,
			&hr_aux, &hr_valid, &ratio, &correl, RedPleth, IrPleth);

	if(valid){
		Sp02 = (int32_t) result;
	}
	if(hr_valid){
		HeartRate = hr_aux;
	}
	unread_samples +=raw_samples_rx;
	raw_samples_rx = 0;
	xSemaphoreGive( ox_mutex );
}

pleth_sample_t GetPlethSample(void){
	pleth_sample_t sample = {-1,-1};
	if(unread_samples){
		xSemaphoreTake( ox_mutex, portMAX_DELAY ); //Bloquea recurso compartido
		sample.ir_sample =	 IrPleth[curr];
		sample.red_sample =  RedPleth[curr];
		curr = (curr+1)%BUFFER_SIZE;
		unread_samples--;
		xSemaphoreGive( ox_mutex );				//Desbloquea recuros compartido
	}
	return sample;
}


int32_t GetSpO2(void){
	int32_t ret = 0;

	xSemaphoreTake( ox_mutex, portMAX_DELAY ); //Bloquea recurso compartido
	ret = Sp02;
	xSemaphoreGive( ox_mutex );				//Desbloquea recuros compartido

	return ret;
}

int32_t GetHeartRate(void){
	int32_t ret = 0;

	xSemaphoreTake( ox_mutex, portMAX_DELAY ); //Bloquea recurso compartido
	ret = HeartRate;
	xSemaphoreGive( ox_mutex );				//Desbloquea recuros compartido

	return ret;
}

uint16_t GetUnreadNum(void){
	uint16_t ret = 0;

	xSemaphoreTake( ox_mutex, portMAX_DELAY ); //Bloquea recurso compartido
	ret = unread_samples;
	xSemaphoreGive( ox_mutex );				//Desbloquea recuros compartido

	return ret;
}

uint8_t AddInputSamples(void){
	uint8_t n_samples = GetNumOfSamples();
	max_sample_t led_sample = {0,0};
	if(n_samples>0){
		for(int i=0; i<n_samples; i++){
			led_sample = GetLedSamples();
			start = (start +1)%BUFFER_SIZE;
			RedInput[(start+BUFFER_SIZE-1)%BUFFER_SIZE] = led_sample.red_sample;
			IrInput[(start+BUFFER_SIZE-1)%BUFFER_SIZE] = led_sample.ir_sample;
			//PRINTF("%d \n", led_sample.red_sample);
		}
		raw_samples_rx += n_samples;
	}
	return n_samples;

}

void WaitForSamples(void){
	xSemaphoreTake( ox_sem, portMAX_DELAY );
}

