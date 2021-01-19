/*
 * ecg.h
 *
 *  Created on: 17 Jan 2021
 *      Author: Ezequiel Vijande
 */

#ifndef ECG_H_
#define ECG_H_

/************************************************
 *					LIBRERIAS					*
 ************************************************/
#include <stdint.h>

/************************************************
 * 					DEFINICIONES				*
 ************************************************/
typedef float ecg_sample_t;
typedef struct {
   uint16_t fs;
} ECG_init_t;

/************************************************
 *					FUNCIONES					*
 ************************************************/

//Inicializa los recursos necesarios para el ECG
void InitializeECG(ECG_init_t*);

//Devuelve el valor del ritmo cardiaco en bpm.
uint16_t GetHeartBeat(void);

//Devuelve la muestra mas vieja no leida del ECG.
ecg_sample_t GetEcgSample(void);

#endif /* ECG_H_ */
