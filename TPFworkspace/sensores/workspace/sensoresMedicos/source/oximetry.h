/*
 * oximetry.h
 *
 *  Created on: 24 Jan 2021
 *      Author: G5
 */

#ifndef OXIMETRY_H_
#define OXIMETRY_H_

/********************************************************
 * 					HEADERS UTILIZADOS					*
 ********************************************************/
#include <stdint.h>

/********************************************************
 * 						DEFINICIONES					*
 ********************************************************/
typedef uint16_t ppg_sample_t;
/********************************************************
 * 						FUNCIONES						*
 ********************************************************/

//Inicializa recursos necesarios para obtener el Sp02 y
//las PPG del led Ir y rojo.
void InitializeOximetry(void);

//Devuelve la ultima muestra sin leer del PPG correspondiente
//al led Ir.
ppg_sample_t GetIrPlethSample(void);

//Devuelve la ultima muestra sin leer del PPG correspondiente
//al led rojo.
ppg_sample_t GetRedPlethSample(void);

//Devuelve el valor de SpO2 como numero en el rango 0-100.
float GetSpO2(void);

//Almacena nueva muestra del led rojo
void AddRedSample(ppg_sample_t);

//Almacena nueva muestra del led infrarojo
void AddIrSample(ppg_sample_t);

#endif /* OXIMETRY_H_ */
