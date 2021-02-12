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
#include "heartware.h"
#include <stdint.h>
#include <stdbool.h>
#include "event_prueba.h"
//FreeRTOS includes
#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"

/****************************************************
 * 					DEFINICIONES					*
 ****************************************************/
#define ECG_SIZE 400
#define WINDOW_DURATION 0.06 //Ventana de 60 milisegundos alrededor de un lado del pico
#define PEAK_LIST_SIZE 100

typedef struct{
	int area;
	uint16_t next_node;
}peak_node_t;

/****************************************************
 * 					VARIABLES GLOBALES				*
 ****************************************************/
static uint16_t fs;
static uint16_t heartbeat = 0;
static ecg_sample_t ecg_signal[ECG_SIZE]; //buffer circular para el ECG

static uint16_t start = 0;
static uint16_t curr = ECG_SIZE-1; //indice a la muestra no leida mas vieja.
static uint16_t unread_samples = 0; //Cantidad de muestras sin leer en el buffer

//Variables para calculo del ritmo cardiaco
static peak_node_t peaks_list[PEAK_LIST_SIZE];
static uint8_t first_node = 0;
static uint8_t list_size = 0;


/****************************************************
 * 					FUNCIONES LOCALES				*
 ****************************************************/
void ECG_sample_callback(TimerHandle_t handler){
	ecg_sample_t sample = GetSensorSample();
	AddEcgSample(sample);
}

bool AddPeak2List(int curr_area){
	peak_node_t curr_peak = {.area=curr_area, .next_node=0};
	bool done = false;
	uint16_t list_index = peaks_list[first_node].next_node;
	uint16_t prev = first_node;
	if( list_size >= PEAK_LIST_SIZE ){
		return false; //Lista llena
	}
	else{
		if(list_size == 0){
			first_node = 0;
			curr_peak.next_node = 0;
		}
		else{
			if( curr_area > (peaks_list[first_node].area) ){
				curr_peak.next_node = first_node;
				first_node = list_size;
				done = true;
			}
			else{
				for(int i=1;(i<list_size)&&(!done);i++){
					if( curr_area > (peaks_list[list_index].area) ){
						curr_peak.next_node = list_index;
						peaks_list[prev].next_node = list_size;
						done = true;
					}
					prev = list_index;
					list_index = peaks_list[list_index].next_node;
				}
			}
			if(!done){
				peaks_list[list_index].next_node = list_size;
				curr_peak.next_node = list_size;
			}
		}
		peaks_list[list_size++] = curr_peak;
		return true;
	}
}

int GetListMedian(void){
	uint16_t median_index = first_node;
	for(int i=0; i< (list_size/2); i++){
		median_index = peaks_list[median_index].next_node;
	}
	return peaks_list[median_index].area;
}
/****************************************************
 *					FUNCIONES DEL HEADER			*
 ****************************************************/

void InitializeECG(ECG_init_t* init_data){
	fs = init_data->fs;
	heart_init_t hardware_init = {fs, ECG_sample_callback};
	InitializeEcgHardware(&hardware_init);
}

int32_t GetEcgSample(void){
	int32_t sample = -1;
	if(unread_samples){

		taskENTER_CRITICAL();
		sample = (int32_t) ecg_signal[curr];
		unread_samples--;
		taskEXIT_CRITICAL();

		curr = (curr+1)%ECG_SIZE;	//Actualizo donde esta la ultima muestra no leida
	}
	return sample;
}

uint16_t GetEcgUnreadNum(void){
	uint16_t ret = 0;

	taskENTER_CRITICAL();
	ret = unread_samples;
	taskEXIT_CRITICAL();

	return ret;
}

uint16_t GetHeartBeat(void){
	uint16_t ret = 0;

	taskENTER_CRITICAL();
	ret = heartbeat;
	taskEXIT_CRITICAL();

	return ret;
}

void AddEcgSample(ecg_sample_t sample){

	start = (start +1)%ECG_SIZE;
	uint16_t last_index = (start + ECG_SIZE - 1)%ECG_SIZE;

	taskENTER_CRITICAL();
	unread_samples++;
	ecg_signal[last_index] = sample;
	taskEXIT_CRITICAL();
}

void CalculateHeartBeat(void){
	//Variables locales
	bool IsMax = true;	//Indica si la muestra corresponde a un maximo local
	int current_area = 0; //Area bajo la curva del maximo actual
	int threshold = 0;
	int delta = 0;
	float duration = ECG_SIZE/fs;
	uint16_t current_node = 0;
	uint16_t r_index = 0;
	uint16_t window_size = fs * WINDOW_DURATION; //Numero de vecinos a un lado del maximo.
	bool successful = true;
	bool finished = false; //Flag para encontrar picos que superan el threshold
	//Reseteo la lista con picos
	first_node = 0;
	list_size = 0;
	int start_index = (start+window_size)%ECG_SIZE;
	for(int i=0; i<(ECG_SIZE-2*window_size);i++){
		IsMax = true;
		current_area = 0;
		//Recorro los vecinos de la muestra
		for(int j=-window_size;(j<=window_size)&&(IsMax);j++){
			delta = ecg_signal[(start_index+i)%ECG_SIZE] - ecg_signal[(start_index+i+j)%ECG_SIZE];
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
		}
	}
	threshold = GetListMedian() /2;
	current_node = first_node;
	for(int i=0; (i<list_size)&&(!finished); i++){
		if( peaks_list[current_node].area > threshold){
			r_index++;
			current_node = peaks_list[current_node].next_node;
		}
		else{
			finished = true;
		}
	}
	heartbeat = (60*r_index)/duration;
}
