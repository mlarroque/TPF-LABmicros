/*
 * max30205.h
 *
 *  Created on: 26 Jan 2021
 *      Author: G5
 */

#ifndef MAX30205_H_
#define MAX30205_H_

/************************************************************
 * 					HEADERS UTILIZADOS						*
 ************************************************************/
#include "FreeRTOS.h"
#include "timers.h"

/************************************************************
 *						DEFINICIONES						*
 ************************************************************/
typedef struct{
	TimerCallbackFunction_t callback;
	TickType_t timeout;
}temp_init_t;

/************************************************************
 * 						FUNCIONES							*
 ************************************************************/

void InitializeTempHardware(temp_init_t*);

// Returns oldest unread sample
uint16_t GetTempSample(void);

void startOneShot(void);

void PrintTempRegister(uint8_t reg);



#endif /* MAX30205_H_ */
