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
//#include "fsl_i2c.h"
#include "fsl_i2c_freertos.h"
#include "peripherals.h"
#include "MK64F12.h"


/***************************************************************
 * FUNCIONES DEL HEADER
 ***************************************************************/

bool WriteByte(uint8_t address, uint8_t reg, uint8_t* tx_buffer, uint8_t n_bytes){
	bool valid = true;
	i2c_master_transfer_t trans_data = {
			.flags= kI2C_TransferDefaultFlag,
			.slaveAddress= address,
			.direction = kI2C_Write,
			.subaddress = (uint32_t)reg<<16,
			.subaddressSize = 8,
			.data = tx_buffer,
			.dataSize = n_bytes
	};

	if( I2C_RTOS_Transfer(&I2C0_rtosHandle, &trans_data) != kStatus_Success ){
		valid = false;
	}
	return valid;
}

bool ReadByte(uint8_t address, uint8_t reg, uint8_t * r_buff, uint8_t n_bytes){
	uint8_t send = reg;
	status_t ret = kStatus_I2C_Busy;
	bool valid = true;
	i2c_master_transfer_t trans_data = {
			.flags= kI2C_TransferDefaultFlag,
			.slaveAddress= address,
			.direction = kI2C_Write,
			.subaddress = (uint32_t)reg<<16,
			.subaddressSize = 8,
			.data = tx_buffer,
			.dataSize = n_bytes
	};

	if( I2C_RTOS_Transfer(&I2C0_rtosHandle, &trans_data) != kStatus_Success ){
		valid = false;
	}
	return valid;

	while(ret == kStatus_I2C_Busy){
		ret = I2C_MasterStart(I2C0, address, kI2C_Write);
	}
	while (0U == (I2C_MasterGetStatusFlags(I2C0) & kI2C_IntPendingFlag))
	{
		__asm__ __volatile__ ("nop");
	}
	ret = I2C_MasterWriteBlocking(I2C0, &send, 1, kI2C_TransferNoStopFlag); //Mando direccion del registro
	if( ret!= kStatus_Success){
		valid = false;
	}
	ret = I2C_MasterRepeatedStart(I2C0, address, kI2C_Read);
	if( ret!= kStatus_Success){
			valid = false;
		}
	while (0U == (I2C_MasterGetStatusFlags(I2C0) & kI2C_IntPendingFlag))
	{
		__asm__ __volatile__ ("nop");
	}
	ret = I2C_MasterReadBlocking(I2C0, r_buff, n_bytes, kI2C_TransferDefaultFlag);
	if( ret != kStatus_Success ){
		valid = false;
	}
	return valid;

}

bool WriteByteNonBlocking(uint8_t address, uint8_t reg, uint8_t* tx_buffer, uint8_t n_bytes){

	i2c_master_transfer_t trans_data = {
			.flags= kI2C_TransferDefaultFlag,
			.slaveAddress= address,
			.direction = kI2C_Write,
			.subaddress = (uint32_t)reg<<16,
			.subaddressSize = 8,
			.data = tx_buffer,
			.dataSize = n_bytes
	};
	I2C_RTOS_Transfer(&I2C0_rtosHandle, &trans_data);
}


