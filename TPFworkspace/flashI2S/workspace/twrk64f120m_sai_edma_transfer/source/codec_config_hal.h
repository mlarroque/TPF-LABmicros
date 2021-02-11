/*
 * codec_config_hal.h
 *
 *  Created on: 7 feb. 2021
 *      Author: Matias
 */

#ifndef CODEC_CONFIG_HAL_H_
#define CODEC_CONFIG_HAL_H_
#include "stdint.h"

void BOARD_I2C_ReleaseBus(void);
int set_config_audio_codec(uint32_t masterClockHz, uint32_t sampleRate_Hz, uint32_t bitWidth);

#endif /* CODEC_CONFIG_HAL_H_ */
