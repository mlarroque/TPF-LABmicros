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
#include <stdint.h>
#include <stdbool.h>

/********************************************************
 * 						DEFINCIONES						*
 ********************************************************/
#define BUFFER_SIZE 1000
/********************************************************
 * 					VARIABLES GLOBALES					*
 ********************************************************/
float Sp02 = 0;
ppg_sample_t RedInput[BUFFER_SIZE];
ppg_sample_t IrInput[BUFFER_SIZE];

ppg_sample_t RedPleth[BUFFER_SIZE];
ppg_sample_t IrPleth[BUFFER_SIZE];
/********************************************************
 * 					FUNCIONES LOCALES					*
 ********************************************************/

/********************************************************
 * 					FUNCIONES DEL HEADER				*
 ********************************************************/
void InitializeOximetry(void){

}

ppg_sample_t GetIrPlethSample(void){

}

ppg_sample_t GetRedPlethSample(void){

}

float GetSpO2(void){

}

void AddRedSample(ppg_sample_t){

}


void AddIrSample(ppg_sample_t){

}
