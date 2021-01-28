/*
 * max30102.h
 *
 *  Created on: 26 Jan 2021
 *      Author: user
 */

#ifndef MAX30102_H_
#define MAX30102_H_

/************************************************************
 * 					HEADERS UTILIZADOS						*
 ************************************************************/
#include "timer.h"

/************************************************************
 *						DEFINICIONES						*
 ************************************************************/
typedef struct{
	callback_ptr callback;
	unsigned long int timeout;
}max_init_t;

typedef struct{
	uint16_t red_sample;
	uint16_t ir_sample;
}max_sample_t;
/************************************************************
 * 						FUNCIONES							*
 ************************************************************/

void InitializeHardware(max_init_t*);

//Devuelve el numero de muestras sin leer almacenadas
uint8_t GetNumOfSamples(void);

//Devuelve la muestra mas vieja sin leer.
max_sample_t GetLedSamples(void);



#endif /* MAX30102_H_ */
