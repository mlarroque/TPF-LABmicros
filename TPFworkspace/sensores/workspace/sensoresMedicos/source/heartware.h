/*
 * heartware.h
 *
 *  Created on: 21 Jan 2021
 *      Author: Ezequiel Vijande
 */

#ifndef HEARTWARE_H_
#define HEARTWARE_H_

/********************************************************
 * 					HEADERS UTILIZADOS					*
 ********************************************************/
#include "timer.h"

/********************************************************
 * 						DEFINICIONES					*
 ********************************************************/
typedef struct {
   uint16_t fs;
   callback_ptr func;
} heart_init_t;

/********************************************************
 * 						FUNCIONES						*
 ********************************************************/
//Inicializa la adquisicion de muestras del sensor.
void InitializeEcgHardware(heart_init_t*);

uint16_t GetSensorSample(void);

#endif /* HEARTWARE_H_ */
