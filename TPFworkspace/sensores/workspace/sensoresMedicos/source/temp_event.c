/*
 * temp_event.c
 *
 *  Created on: 30 Jan 2021
 *      Author: G5
 */


#include <stdbool.h>
#include <stdint.h>
#include "temp_event.h"
#include "fsl_debug_console.h"


static bool samples_ready = false;

bool IsTempEvent(void){
	return samples_ready;
}
void PushTempEvent(TimerHandle_t handler){
	PRINTF("ENTER \n");
	samples_ready = true;
}

void PopTempEvent(void){
	samples_ready = false;
}
