/*
 * ox_event.c
 *
 *  Created on: 30 Jan 2021
 *      Author: user
 */


#include <stdbool.h>
#include <stdint.h>
#include "ox_event.h"
#define OX_EVENT_ARRAY_SIZE 60


static ox_event_t ox_ev_sample_array[OX_EVENT_ARRAY_SIZE];
static uint8_t ox_ev_start =0;
static uint8_t ox_ev_array_size = 0;

bool IsOxEvent(void){
	if(ox_ev_array_size>0){
		return true;
	}
	else{
		return false;
	}
}
void PushOxEvent(uint16_t red_sample, uint16_t ir_sample){
	if(ox_ev_array_size<OX_EVENT_ARRAY_SIZE){
		ox_ev_sample_array[(ox_ev_start+ox_ev_array_size)%OX_EVENT_ARRAY_SIZE].red_sample = red_sample;
		ox_ev_sample_array[(ox_ev_start+ox_ev_array_size)%OX_EVENT_ARRAY_SIZE].ir_sample = ir_sample;
		++ox_ev_array_size;
	}
}

ox_event_t PopOxEvent(void){
	ox_event_t sample;
	if(ox_ev_array_size>0){
		sample = ox_ev_sample_array[ox_ev_start++];
		ox_ev_start %= OX_EVENT_ARRAY_SIZE;
		--ox_ev_array_size;
	}
	return sample;
}

