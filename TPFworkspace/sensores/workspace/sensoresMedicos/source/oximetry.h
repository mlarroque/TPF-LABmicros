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
typedef int32_t ppg_sample_t;
typedef struct {
   uint16_t fs;
} oxi_init_t;

typedef struct {
	int32_t red_sample;
	int32_t ir_sample;
} pleth_sample_t;
/********************************************************
 * 						FUNCIONES						*
 ********************************************************/

//Inicializa recursos necesarios para obtener el Sp02 y
//las PPG del led Ir y rojo.
void InitializeOximetry(oxi_init_t*);

//Devuelve la ultima muestra sin leer del PPG.
//si no hay muestras disponibles devuelve -1 en cada muestra.
pleth_sample_t GetPlethSample(void);

//Devuelve cuantas muestras quedan por leer
uin16_t GetUnreadNum(void);

//Devuelve la ultima muestra sin leer del PPG correspondiente
//al led rojo.
int32_t GetRedPlethSample(void);

uin16_t GetRedUnreadNum(void);

//Devuelve el valor de SpO2 como numero en el rango 0-100.
int32_t GetSpO2(void);

//Almacena nuevas muestras de ambos leds.
void AddInputSamples(ppg_sample_t red_sample, ppg_sample_t ir_sample);

#endif /* OXIMETRY_H_ */