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
#define BUFFER_SIZE 20

/********************************************************
 * 					VARIABLES GLOBALES					*
 ********************************************************/

static uint16_t temperature;	// Buffer circular que guarda inputs


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

void AddTempInputSample(void){
	uint16_t sample = 0;
	sample = GetTempSample();
	if(sample != 0){
		// Valid Sample
		xSemaphoreTake( temp_mutex, portMAX_DELAY ); //Bloquea recurso compartido
		temperature = sample;
		xSemaphoreGive( temp_mutex );				//Desbloquea recuros compartido

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
	ret_value = temperature;
	xSemaphoreGive( temp_mutex );				//Desbloquea recuros compartido

	return ret_value;
}
