/*
 * thermometer.c
 *
 *  Created on: 24 Jan 2021
 *      Author: G5
 */

/********************************************************
 *					 HEADERS UTILIZADOS					*
 ********************************************************/

#include "thermometer.h"
#include "max30205.h"
#include "fsl_debug_console.h"
#include <stdint.h>
#include <stdbool.h>
//FreeRTOS includes
#include "FreeRTOS.h"
#include "semphr.h"

/********************************************************
 * 						DEFINCIONES						*
 ********************************************************/
#define BUFFER_SIZE 100

/********************************************************
 * 					VARIABLES GLOBALES					*
 ********************************************************/

static uint16_t tempBuffer[BUFFER_SIZE];	// Buffer circular que guarda inputs

static uint16_t start = 0; // Indice donde se encuentra la muestra mas vieja.
static uint16_t curr = 0; // Indice con la muestra mas vieja sin leer
static uint16_t unread_samples = 0;

static SemaphoreHandle_t temp_mutex;
/********************************************************
 * 					FUNCIONES LOCALES					*
 ********************************************************/

/********************************************************
 * 					FUNCIONES DEL HEADER				*
 ********************************************************/

void InitializeThermometer(void){
	temp_mutex = xSemaphoreCreateMutex();
	InitializeTempHardware();
}

uint16_t GetThermoSample(void){
	uint16_t sample = 0;

	if(unread_samples){
		xSemaphoreTake( temp_mutex, portMAX_DELAY );
		sample = tempBuffer[curr];
		curr = (curr+1)%BUFFER_SIZE;
		unread_samples--;
		xSemaphoreGive( temp_mutex );
	}

	return sample;
}

uint16_t GetTempUnreadNum(void){
	uint16_t ret = 0;

	xSemaphoreTake( temp_mutex, portMAX_DELAY ); //Bloquea recurso compartido
	ret = unread_samples;
	xSemaphoreGive( temp_mutex );				//Desbloquea recuros compartido

	return ret;
}

void AddTempInputSample(void){
	uint16_t sample = 0;
	sample = GetTempSample();
	if(sample != 0){
		// Valid Sample
		++start;

		xSemaphoreTake( temp_mutex, portMAX_DELAY ); //Bloquea recurso compartido
		tempBuffer[(start+BUFFER_SIZE-1)%BUFFER_SIZE] = sample;
		unread_samples++;
		xSemaphoreGive( temp_mutex );				//Desbloquea recuros compartido

		//PRINTF("SAMPLE VALUE: %d \n", sample);
	}
	else{
		// Sample Not Ready
	}
}

void newSampleRequest(void){
	startOneShot();
}

uint16_t getTemperature(void){
	uint16_t ret_value = 0;

	xSemaphoreTake( temp_mutex, portMAX_DELAY ); //Bloquea recurso compartido
	ret_value = tempBuffer[curr];
	xSemaphoreGive( temp_mutex );				//Desbloquea recuros compartido

	return ret_value;
}
