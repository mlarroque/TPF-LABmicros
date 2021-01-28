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

uint8_t WriteByte(uint8_t address, uint8_t reg, uint8_t byte){
	I2C_MasterWriteBlocking(I2C0, const uint8_t *txBuff, size_t txSize, uint32_t flags);
}
