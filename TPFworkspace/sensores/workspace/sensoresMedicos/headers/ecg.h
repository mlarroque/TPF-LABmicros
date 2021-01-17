/*
 * ecg.h
 *
 *  Created on: 17 Jan 2021
 *      Author: Ezequiel Vijande
 */

#ifndef ECG_H_
#define ECG_H_

/*************************************************
 *					LIBRERIAS
 *************************************************/
#include <stdint.h>

/*************************************************
 *					FUNCIONES
 *************************************************/
//Devuelve el valor del ritmo cardiaco en bpm.
uint8_t GetHeartBeat(void);

//Devuelve un puntero con los valores del ECG.
float * GetECGsignal(void);

#endif /* ECG_H_ */
