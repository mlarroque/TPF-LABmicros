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
#include <stdbool.h>
/****************************************************
 * 					DEFINICIONES					*
 ****************************************************/
#define ECG_SIZE 1024
#define WINDOW_DURATION 0.06
#define PEAK_LIST_SIZE 50

typedef struct{
	int area;
	uint16_t next_node;
	uint16_t prev_node;

}peak_node_t;

/****************************************************
 * 					VARIABLES GLOBALES				*
 ****************************************************/
static uint16_t fs;
static ecg_sample_t ecg_signal[ECG_SIZE]; //buffer circular para el ECG
static uint16_t start = 0;
static uint16_t curr = ECG_SIZE-1; //indice a la muestra no leida mas vieja.

//Variables para calculo del ritmo cardiaco
static peak_node_t peaks_list[PEAK_LIST_SIZE];
static uint8_t first_node = 0;
static uint8_t list_size = 0;



/****************************************************
 * 					FUNCIONES LOCALES				*
 ****************************************************/

void AddEcgSample(ecg_sample_t sample){
	start++;
	uint16_t last_index = (start + ECG_SIZE - 1)%ECG_SIZE;
	ecg_signal[last_index] = sample;
}

bool AddPeak2List(int curr_area){
	peak_node_t curr_peak = {.area=curr_area, .next_node=0};
	bool done = false;
	uint16_t list_index = peaks_list[first_node].next_node;
	if( list_size >= PEAK_LIST_SIZE ){
		return false; //Lista llena
	}
	else{
		if(list_size == 0){
			first_node = 0;
			curr_peak.next_node = 0;
			curr_peak.prev_node = 0;
		}
		else{
			if( curr_area > (peaks_list[first_node].area) ){
				curr_peak.prev_node = list_size;
				curr_peak.next_node = first_node;
				peaks_list[first_node].prev_node = list_size;
				first_node = list_size;
			}
			else{
				for(int i=1;(i<list_size)&&(!done);i++){
					if( curr_area > (peaks_list[list_index].area) ){
						curr_peak.prev_node = peaks_list[list_index].prev_node;
						curr_peak.next_node = list_index;
						peaks_list[list_index].prev_node = list_size;
						done = true;
					}
					list_index = peaks_list[list_index].next_node;
				}
			}
			if(!done){
				peaks_list[list_index].next_node = list_size;
				curr_peak.prev_node = list_index;
				curr_peak.next_node = list_size;
			}
			peaks_list[list_size++] = curr_peak;
		}

	}
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
	//Variables locales
	bool IsMax = true;	//Indica si la muestra corresponde a un maximo local
	int current_area = 0; //Area bajo la curva del maximo actual
	int delta = 0;
	uint16_t window_size = fs * WINDOW_DURATION; //Numero de vecinos a un lado del maximo.
	bool successful = true;
	//Reseteo la lista con picos
	first_node = 0;
	list_size = 0;
	for(int i=window_size; i<(ECG_SIZE-window_size);i++){
		IsMax = true;
		current_area = 0;
		//Recorro los vecinos de la muestra
		for(int j=-window_size;(j<=window_size)&&(IsMax);j++){
			delta = ecg_signal[i] - ecg_signal[i+j];
			if( delta < 0 ){
				IsMax = false; //Si hay un descenso no es maximo local
			}
			else{
				current_area += delta;
			}
		}
		if(IsMax){
			successful = AddPeak2List(current_area);
			if(!successful){
				//error
			}
			else{

			}
		}
	}
}
