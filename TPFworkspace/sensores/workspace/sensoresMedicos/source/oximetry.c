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
#include <stdint.h>
#include <stdbool.h>

/********************************************************
 * 						DEFINCIONES						*
 ********************************************************/
#define BUFFER_SIZE 500
#define UPDATE_SPO2_TIME 1 //Cada cuantos segundos se actualiza el valor del SP02

/********************************************************
 * 					VARIABLES GLOBALES					*
 ********************************************************/
uint16_t fs;
int32_t Sp02 = 0;
ppg_sample_t RedInput[BUFFER_SIZE];	//Buffer circular que guarda inputs del led rojo.
ppg_sample_t IrInput[BUFFER_SIZE];	//Buffer circular que guarda inputs del led ir.

uint16_t start = 0; //Indice donde se encuentra la muestra mas vieja.
uint16_t curr = 0; //Indice con la muestra mas vieja sin leer del pleth
uint16_t unread_samples = 0;

ppg_sample_t RedPleth[BUFFER_SIZE];
ppg_sample_t IrPleth[BUFFER_SIZE];


uint16_t counter=BUFFER_SIZE; //Muestras restantes para calcular nuevo SpO2

/********************************************************
 * 					FUNCIONES LOCALES					*
 ********************************************************/
void CalculateSpO2(void){
	int32_t result = 0;
	int8_t valid = 0;
	maxim_oxygen_saturation(IrInput,  BUFFER_SIZE,  RedInput,  &result, &valid,
							start, IrPleth, RedPleth);
	if(valid){
		Sp02 = result;
	}
}

/********************************************************
 * 					FUNCIONES DEL HEADER				*
 ********************************************************/
void InitializeOximetry(oxi_init_t* init_data){
	fs = init_data->fs;
	counter = fs * UPDATE_SPO2_TIME;
	InitializeHardware(fs);
}

pleth_sample_t GetPlethSample(void){
	pleth_sample_t sample = {-1,-1};
	if(unread_samples){
		sample.ir_sample = (int32_t) IrPleth[curr];
		sample.red_sample = (int32_t) RedPleth[curr];
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

	if( !(--counter) ){
		counter = fs * UPDATE_SPO2_TIME;	//Resetteo contador
		CalculateSpO2();
		unread_samples +=fs * UPDATE_SPO2_TIME; //REVISAR!!!!!!!
	}
}

