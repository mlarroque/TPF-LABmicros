/*
 * UDA.c
 *
 *  Created on: 3 feb. 2021
 *      Author: Matias
 */


#include "UDA.h"
//#include "fsl_i2c"
#include "peripherals.h"
//#include <stdint.h>


#define UDA_ADRESS_RX 0x18
#define UDA_SOFT_ADRESS 0x07
#define UDA_REG(a)  a



status_t i2c_write_UDA(uint8_t tx_adress, uint8_t * tx_content);


int init_UDA(void){
	status_t status;
	//1) codec software reset
	uint8_t tx_content[] = {0x00, 0x00};
	status = i2c_write_UDA(UDA_SOFT_ADRESS, tx_content);

	//2) config de registros
	tx_content[0] = 0x0F;
	tx_content[1] = 0x31;
	status = i2c_write_UDA(UDA_REG(0x00), tx_content);

	tx_content[0] = 0x00;
	tx_content[1] = 0x00;
	status = i2c_write_UDA(UDA_REG(0x01), tx_content);
	//turn on headphone!
	tx_content[0] = 0xA5;
	tx_content[1] = 0xDF;
	status = i2c_write_UDA(UDA_REG(0x02), tx_content);

	tx_content[0] = 0x00;
	tx_content[1] = 0x00;
	status = i2c_write_UDA(UDA_REG(0x03), tx_content);

	tx_content[0] = 0x02;
	tx_content[1] = 0x02;
	status = i2c_write_UDA(UDA_REG(0x04), tx_content);
	//solo quedan resets...
	tx_content[0] = 0x00;
	tx_content[1] = 0x00;
	status = i2c_write_UDA(UDA_REG(0x10), tx_content);


	status = i2c_write_UDA(UDA_REG(0x11), tx_content);


	status = i2c_write_UDA(UDA_REG(0x13), tx_content);

	return status;
}

status_t i2c_write_UDA(uint8_t tx_adress, uint8_t * tx_content){
	status_t status;
	I2C_EnableDMA(I2C0, 1);
	status = I2C_MasterStart(I2C0, UDA_ADRESS_RX, kI2C_Write);
	if (status == kStatus_Success){
		status = I2C_MasterWriteBlocking(I2C0, &tx_adress, 1, kI2C_TransferNoStopFlag);
		if (status == kStatus_Success){
			status = I2C_MasterWriteBlocking(I2C0, tx_content, 2, kI2C_TransferDefaultFlag);
		}
	}


	return status;
}


