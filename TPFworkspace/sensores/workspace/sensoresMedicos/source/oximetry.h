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
typedef uint32_t ppg_sample_t;
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

//Actualiza el valor de oxigeno en sangre
void CalculateSpO2(void);

//Devuelve la ultima muestra sin leer del PPG.
//si no hay muestras disponibles devuelve -1 en cada muestra.
pleth_sample_t GetPlethSample(void);

//Devuelve cuantas muestras quedan por leer
uint16_t GetUnreadNum(void);


//Devuelve el valor de SpO2 como numero en el rango 0-100.
int32_t GetSpO2(void);

//Devuelve el valor del ritmo cardiaco.
int32_t GetHeartRate(void);

//Almacena nuevas muestras de ambos leds. Devuelve cuantas muestras agrego
uint8_t AddInputSamples(void);

#endif /* OXIMETRY_H_ */
