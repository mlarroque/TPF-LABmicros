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

/********************************************************
 * 						DEFINCIONES						*
 ********************************************************/
#define BUFFER_SIZE 500
#define UPDATE_SPO2_TIME 1 //Cada cuantos segundos se actualiza el valor del SP02
#define SAMPLE_BATCH_SIZE 20 //se agregan de a 20 muestras a la vez en el buffer
#define MAX_NUM_SAMPLES	32 //Maximo numero de muestras a leer seguidas

/********************************************************
 * 					VARIABLES GLOBALES					*
 ********************************************************/
static uint16_t fs;
static int32_t Sp02 = 0;
static ppg_sample_t RedInput[BUFFER_SIZE];	//Buffer circular que guarda inputs del led rojo.
static ppg_sample_t IrInput[BUFFER_SIZE];	//Buffer circular que guarda inputs del led ir.

static uint16_t start = 0; //Indice donde se encuentra la muestra mas vieja.
static uint16_t curr = 0; //Indice con la muestra mas vieja sin leer del pleth
static uint16_t unread_samples = 0;

static int32_t RedPleth[BUFFER_SIZE];
static int32_t IrPleth[BUFFER_SIZE];

/********************************************************
 * 					FUNCIONES LOCALES					*
 ********************************************************/
void OxSampleCallback(void){
	uint8_t n_samples = GetNumOfSamples();
	max_sample_t led_samples[MAX_NUM_SAMPLES];
	for(int i=0; i<n_samples; i++){
		led_samples[i] = GetLedSamples();
	}
	//Pushear evento
}

/********************************************************
 * 					FUNCIONES DEL HEADER				*
 ********************************************************/
void InitializeOximetry(oxi_init_t* init_data){
	fs = init_data->fs;
	unsigned long int timeout = (SAMPLE_BATCH_SIZE*1000)/fs;
	max_init_t hard_init = {OxSampleCallback,timeout};
	InitializeOxHardware(&hard_init);
}

void CalculateSpO2(void){
	int32_t result = 0;
	int8_t valid = 0;
	maxim_oxygen_saturation(IrInput,  BUFFER_SIZE,  RedInput,  &result, &valid,
							start, IrPleth, RedPleth);
	if(valid){
		Sp02 = result;
	}
	unread_samples +=fs * UPDATE_SPO2_TIME; //REVISAR!!!!!!!
}

pleth_sample_t GetPlethSample(void){
	pleth_sample_t sample = {-1,-1};
	if(unread_samples){
		sample.ir_sample =	 IrPleth[curr];
		sample.red_sample =  RedPleth[curr];
		curr = (curr+1)%BUFFER_SIZE;
		unread_samples--;
	}
	return sample;
}


int32_t GetSpO2(void){
	return Sp02;
}

void AddInputSamples(ppg_sample_t red_sample, ppg_sample_t ir_sample){
	++start;
	RedInput[(start+BUFFER_SIZE-1)%BUFFER_SIZE] = red_sample;
	IrInput[(start+BUFFER_SIZE-1)%BUFFER_SIZE] = ir_sample;

}

