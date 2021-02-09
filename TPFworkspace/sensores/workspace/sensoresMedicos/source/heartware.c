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
 * 					DEFINICIONES				*
 ************************************************/
#define ADC_CHANNEL 12

/************************************************
 * 				VARIABLES LOCALES
 ************************************************/
const adc16_channel_config_t current_config = {
  .channelNumber = ADC_CHANNEL,
  .enableInterruptOnConversionCompleted = false,
  .enableDifferentialConversion = false
};
/************************************************
 * 				FUNCIONES DEL HEADER			*
 ************************************************/

void InitializeEcgHardware(heart_init_t* init_data){
	unsigned long int timeout = 1000 / (init_data->fs);
	TimerHandle_t handler = xTimerCreate("ECG Timer",
			pdMS_TO_TICKS(timeout),
			pdTRUE,
			NULL,
			init_data->func);
	xTimerStart(handler, 0);
}

uint16_t GetSensorSample(void){
	uint16_t sample = (uint16_t) ADC16_GetChannelConversionValue(ADC0, kADC16_ChannelMuxA);
	ADC16_SetChannelConfig(ADC0, kADC16_ChannelMuxA, &current_config);
	return sample;
}
