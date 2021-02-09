/*
 * temp_event.c
 *
 *  Created on: 30 Jan 2021
 *      Author: G5
 */


#include <stdbool.h>
#include <stdint.h>
#include "temp_event.h"


static bool samples_ready = false;

bool IsTempEvent(void){
	return samples_ready;
}
void PushTempEvent(void){
	samples_ready = true;
}

void PopTempEvent(void){
	samples_ready = false;
}