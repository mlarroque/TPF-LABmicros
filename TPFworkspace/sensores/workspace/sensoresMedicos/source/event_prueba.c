/*
 * event_prueba.c
 *
 *  Created on: 30 Jan 2021
 *      Author: user
 */

#include <stdbool.h>
#include <stdint.h>
#include "event_prueba.h"
#define EVENT_ARRAY_SIZE 30


static uint16_t ev_sample_array[EVENT_ARRAY_SIZE];
static uint8_t ev_start =0;
static uint8_t ev_array_size = 0;

bool IsEvent(void){
	if(ev_array_size>0){
		return true;
	}
	else{
		return false;
	}
}
void PushEvent(uint16_t sample){
	if(ev_array_size<EVENT_ARRAY_SIZE){
		ev_sample_array[(ev_start+ev_array_size)%EVENT_ARRAY_SIZE] = sample;
		++ev_array_size;
	}
}

int32_t PopEvent(void){
	int32_t sample = -1;
	if(ev_array_size>0){
		sample = ev_sample_array[ev_start++];
		ev_start %= EVENT_ARRAY_SIZE;
		--ev_array_size;
	}
	return sample;
}
