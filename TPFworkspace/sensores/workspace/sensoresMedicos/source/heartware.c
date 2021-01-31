/*
 * heartware.c
 *
 *  Created on: 21 Jan 2021
 *      Author: Ezequiel Vijande
 */


/************************************************
 * 				HEADERS UTILIZADOS				*
 ************************************************/
#include "heartware.h"
#include "fsl_adc16.h"
#include "MK64F12.h"

/************************************************
 * 				VARIABLES LOCALES
 ************************************************/
const adc16_channel_config_t current_config = {
  .channelNumber = kADC16_ChannelMuxA,
  .enableInterruptOnConversionCompleted = false,
  .enableDifferentialConversion = false
};
/************************************************
 * 				FUNCIONES DEL HEADER			*
 ************************************************/

void InitializeEcgHardware(heart_init_t* init_data){
	unsigned long int timeout = 1000 / (init_data->fs);
	SetTimer(HEART_SAMPLER,timeout, init_data->func);
}

uint16_t GetSensorSample(void){
	uint16_t sample = (uint16_t) ADC16_GetChannelConversionValue(ADC0, kADC16_ChannelMuxA);
	ADC16_SetChannelConfig(ADC0, 0, &current_config);
	return sample;
}
