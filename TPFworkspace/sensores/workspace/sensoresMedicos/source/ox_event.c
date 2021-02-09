/*
 * ox_event.c
 *
 *  Created on: 30 Jan 2021
 *      Author: user
 */


#include <stdbool.h>
#include <stdint.h>
#include "ox_event.h"


static bool samples_ready = false;

bool IsOxEvent(void){
	return samples_ready;
}

void PushOxEvent(TimerHandle_t handler){
	samples_ready = true;
}

void PopOxEvent(void){
	samples_ready = false;
}

