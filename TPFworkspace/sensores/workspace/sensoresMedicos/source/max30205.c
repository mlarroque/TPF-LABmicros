/*
 * max30205.c
 *
 *  Created on: 26 Jan 2021
 *      Author: G5
 */

/*******************************************************
 * 					HEADERS UTILIZADOS
 *******************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "max30205.h"
#include "i2c.h"
#include "fsl_debug_console.h"

/********************************************************
 * 						DEFINCIONES						*
 ********************************************************/

// General

#define RETRY_READ_TIMES	5
#define MAX30205_ADDR	0x48 // 0x90 --> actual address
#define BYTES_PER_SAMPLE	1

// Masks & Shifts

#define READY_MASK	0x7FU

// Registers

#define TEMP	0x00
#define CONFIG	0x01
#define T_HYST	0x02
#define T_OS	0x03

// Mode

typedef enum{CONTINOUS=0x5A, SHUTDOWN=0xDB, ONE_SHOT=0x80}mode_t;

/********************************************************
 * 					FUNCIONES LOCALES					*
 ********************************************************/

void SetTempMode(mode_t mode){
	uint8_t reg = mode;
	bool successful = false;
	while(!successful){
		successful = WriteByte(MAX30205_ADDR, CONFIG, &reg, 1);
	}
}

void ConfigureMax30205(void){
	SetTempMode(SHUTDOWN);
}

/********************************************************
 * 					FUNCIONES DEL HEADER
 ********************************************************/

void InitializeTempHardware(void){
	ConfigureMax30205();
#ifdef MAX_DEBUG
	PrintRegister(TEMP);
	PrintRegister(CONFIG);
#endif
}

uint16_t GetTempSample(void){

	uint8_t sample_buff[2] = {0 , 0};
	uint8_t config_status = 0;
	bool successful = false;

	for(int i=0; (i<RETRY_READ_TIMES)&&(!successful); i++){
		successful = ReadByte(MAX30205_ADDR, TEMP, &config_status, 1);
		if(successful){
			uint8_t sample_ready = ~ (config_status | READY_MASK);
			if(sample_ready){
				PRINTF("SAMPLE READY \n");
				successful = ReadByte(MAX30205_ADDR, TEMP, sample_buff, 2);
			}
			else{
				PRINTF("SAMPLE NOT READY \n");
			}
		}
	}
	uint16_t sample = ( ((uint16_t)sample_buff[0])<<8 ) | (uint16_t)sample_buff[1];
	return sample;
}

void startOneShot(void){
	bool successful = false;
	uint8_t tx_byte = ONE_SHOT;
	while(!successful){
		successful = WriteByte(MAX30205_ADDR, CONFIG, &tx_byte, 1);
	}
}

void PrintTempRegister(uint8_t reg){
	uint8_t recieved = 0;
	ReadByte(MAX30205_ADDR, reg, &recieved, 1);
	PRINTF("%d \n", recieved);
}
