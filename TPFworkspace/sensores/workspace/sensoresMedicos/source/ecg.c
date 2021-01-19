/*
 * ecg.c
 *
 *  Created on: 17 Jan 2021
 *      Author: Ezequiel Vijande
 */

/****************************************************
 *					HEADERS UTILIZADOS				*
 ****************************************************/
#include "ecg.h"
#include <stdint.h>
/****************************************************
 * 					DEFINICIONES					*
 ****************************************************/
#define ECG_SIZE 1024

/****************************************************
 * 					VARIABLES GLOBALES				*
 ****************************************************/
static uint16_t fs;
static ecg_sample_t ecg_signal[ECG_SIZE]; //buffer circular para el ECG
static uint16_t start = 0;
static uint16_t curr = ECG_SIZE-1; //indice a la muestra no leida mas vieja.



/****************************************************
 * 					FUNCIONES LOCALES				*
 ****************************************************/

void AddEcgSample(ecg_sample_t sample){
	start++;
	uint16_t last_index = (start + ECG_SIZE - 1)%ECG_SIZE;
	ecg_signal[last_index] = sample;
}

/****************************************************
 *					FUNCIONES DEL HEADER			*
 ****************************************************/

void InitializeECG(ECG_init_t* init_data){
	fs = init_data->fs;
}

ecg_sample_t GetEcgSample(void){
	ecg_sample_t sample = ecg_signal[curr];
	curr = (curr+1)%ECG_SIZE;	//Actulaizo donde esta la ultima muestra no leida
	return sample;
}

uint16_t GetHeartBeat(void){

}
