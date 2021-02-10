/*
 * i2c.c
 *
 *  Created on: 28 Jan 2021
 *      Author: user
 */

/************************************************************
 * 					HEADERS UTILIZADOS						*
 ************************************************************/
#include "i2c.h"
#include "peripherals.h"
#include "MK64F12.h"
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "fsl_i2c_freertos.h"
/***************************************************************
 * 					FUNCIONES DEL HEADER
 ***************************************************************/

bool WriteByte(uint8_t address, uint8_t reg, uint8_t* tx_buffer, uint8_t n_bytes){
	bool valid = true;
	i2c_master_transfer_t trans_data = {
			.flags= kI2C_TransferDefaultFlag,
			.slaveAddress= address,
			.direction = kI2C_Write,
			.subaddress = (uint32_t)reg,
			.subaddressSize = 1,
			.data = tx_buffer,
			.dataSize = n_bytes
	};

	if( I2C_RTOS_Transfer(&I2C0_rtosHandle, &trans_data) != kStatus_Success ){
		valid = false;
	}
	return valid;
}

bool ReadByte(uint8_t address, uint8_t reg, uint8_t * r_buff, uint8_t n_bytes){
	bool valid = true;
	i2c_master_transfer_t trans_data = {
			.flags= kI2C_TransferDefaultFlag,
			.slaveAddress= address,
			.direction = kI2C_Read,
			.subaddress = (uint32_t)reg,
			.subaddressSize = 1,
			.data = r_buff,
			.dataSize = n_bytes
	};

	if( I2C_RTOS_Transfer(&I2C0_rtosHandle, &trans_data) != kStatus_Success ){
		valid = false;
	}
	return valid;
}


