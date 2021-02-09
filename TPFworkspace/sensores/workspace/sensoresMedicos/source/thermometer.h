/*
 * thermometer.h
 *
 *  Created on: 24 Jan 2021
 *      Author: G5
 */

#ifndef THERMOMETER_H_
#define THERMOMETER_H_

/********************************************************
 * 					HEADERS UTILIZADOS					*
 ********************************************************/

#include <stdint.h>

/********************************************************
 * 						DEFINICIONES					*
 ********************************************************/

/********************************************************
 * 						FUNCIONES						*
 ********************************************************/

void InitializeThermometer(void);

/* Pops last temperature without */
uint16_t GetThermoSample(void);

/* Returns current temperature without modifying buffer */
uint16_t getTemperature(void);

/* Returns number of unread samples */
uint16_t GetTempUnreadNum(void);

/* Adds temperature sample to buffer */
void AddTempInputSample(void);

/* Request to start new temperature measurment */
void newSampleRequest(void);

#endif /* THERMOMETER_H_ */
