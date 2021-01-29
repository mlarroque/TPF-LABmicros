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
#include "fsl_i2c.h"
#include "MK64F12.h"


/***************************************************************
 * FUNCIONES DEL HEADER
 ***************************************************************/

bool WriteByte(uint8_t address, uint8_t reg, uint8_t byte){
	uint8_t send[2] = {reg, byte};
	status_t ret = kStatus_I2C_Busy;
	bool valid = false;

	while(ret == kStatus_I2C_Busy){
		ret = I2C_MasterStart(I2C0, address, kI2C_Write);
	}
	ret = I2C_MasterWriteBlocking(I2C0, send, 2, kI2C_TransferDefaultFlag);
	if( ret == kStatus_Success ){
		valid = true;
	}
	return valid;
}

bool ReadByte(uint8_t address, uint8_t reg, uint8_t * r_buff){
	uint8_t send = reg;
	status_t ret = kStatus_I2C_Busy;
	bool valid = false;

	while(ret == kStatus_I2C_Busy){
		ret = I2C_MasterStart(I2C0, address, kI2C_Write);
	}
	I2C_MasterWriteBlocking(I2C0, &send, 1, kI2C_TransferNoStopFlag); //Mando direccion del registro
	I2C_MasterRepeatedStart(I2C0, address, kI2C_Read);
	ret = I2C_MasterReadBlocking(I2C0, r_buff, 1, kI2C_TransferDefaultFlag);

	if( ret == kStatus_Success ){
		valid = true;
	}
	return valid;

}
