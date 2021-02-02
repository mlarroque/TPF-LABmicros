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
#include "ox_event.h"
#include <stdint.h>
#include <stdbool.h>
#include "fsl_debug_console.h"

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
static ppg_sample_t RedInput[MAX_BUFF_SIZE];	//Buffer circular que guarda inputs del led rojo.
static ppg_sample_t IrInput[MAX_BUFF_SIZE];	//Buffer circular que guarda inputs del led ir.

static uint16_t start = 0; //Indice donde se encuentra la muestra mas vieja.
static uint16_t curr = 0; //Indice con la muestra mas vieja sin leer del pleth
static uint16_t unread_samples = 0;
static uint16_t raw_samples_rx = 0; //Cantidad de muestras sin procesar almacenadas

static int32_t RedPleth[MAX_BUFF_SIZE];
static int32_t IrPleth[MAX_BUFF_SIZE];

/********************************************************
 * 					FUNCIONES LOCALES					*
 ********************************************************/

/********************************************************
 * 					FUNCIONES DEL HEADER				*
 ********************************************************/
void InitializeOximetry(oxi_init_t* init_data){
	fs = init_data->fs;
	unsigned long int timeout = (SAMPLE_BATCH_SIZE*1000)/fs;
	max_init_t hard_init = {PushOxEvent,timeout};
	InitializeOxHardware(&hard_init);
}

void CalculateSpO2(void){
	int32_t result = 0;
	int8_t valid = 0;
	maxim_oxygen_saturation(IrInput,  MAX_BUFF_SIZE,  RedInput,  &result, &valid,
							start, IrPleth, RedPleth);
	if(valid){
		Sp02 = result;
	}
	unread_samples +=raw_samples_rx;
	raw_samples_rx = 0;
}

pleth_sample_t GetPlethSample(void){
	pleth_sample_t sample = {-1,-1};
	if(unread_samples){
		sample.ir_sample =	 IrPleth[curr];
		sample.red_sample =  RedPleth[curr];
		curr = (curr+1)%MAX_BUFF_SIZE;
		unread_samples--;
	}
	return sample;
}


int32_t GetSpO2(void){
	return Sp02;
}

uint16_t GetUnreadNum(void){
	return unread_samples;
}

uint8_t AddInputSamples(void){
	uint8_t n_samples = GetNumOfSamples();
	max_sample_t led_sample = {0,0};
	if(n_samples>0){
		for(int i=0; i<n_samples; i++){
			led_sample = GetLedSamples();
			++start;
			RedInput[(start+MAX_BUFF_SIZE-1)%MAX_BUFF_SIZE] = led_sample.red_sample;
			IrInput[(start+MAX_BUFF_SIZE-1)%MAX_BUFF_SIZE] = led_sample.ir_sample;
			//PRINTF("%d \n", led_sample.red_sample);
		}
		raw_samples_rx += n_samples;
	}
	return n_samples;

}

