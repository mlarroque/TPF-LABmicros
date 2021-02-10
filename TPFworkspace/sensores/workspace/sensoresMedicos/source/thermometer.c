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
#include "temp_event.h"
#include "fsl_debug_console.h"

#include <stdint.h>
#include <stdbool.h>

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

/********************************************************
 * 					FUNCIONES LOCALES					*
 ********************************************************/

/********************************************************
 * 					FUNCIONES DEL HEADER				*
 ********************************************************/

void InitializeThermometer(void){
	TickType_t timeout = 200; // ms
	temp_init_t hard_init = {PushTempEvent, timeout};
	InitializeTempHardware(&hard_init);
}

uint16_t GetThermoSample(void){
	uint16_t sample = 0;
	if(unread_samples){
		sample = tempBuffer[curr];
		curr = (curr+1)%BUFFER_SIZE;
		unread_samples--;
	}
	return sample;
}

uint16_t GetTempUnreadNum(void){
	return unread_samples;
}

void AddTempInputSample(void){
	uint16_t sample = 0;
	sample = GetTempSample();
	if(sample != 0){
		// Valid Sample
		++start;
		tempBuffer[(start+BUFFER_SIZE-1)%BUFFER_SIZE] = sample;
		PRINTF("SAMPLE VALUE: %d \n", sample);
	}
	else{
		// Sample Not Ready
	}
}

void newSampleRequest(void){
	startOneShot();
}

uint16_t getTemperature(void){
	return tempBuffer[curr];
}
