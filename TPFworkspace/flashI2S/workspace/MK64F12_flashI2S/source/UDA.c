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

#define UDA_SOFT_ADRESS 0x07
#define UDA_REG(a)  a



status_t i2c_write_UDA(uint8_t tx_adress, uint8_t * tx_content);


int init_UDA(void){
	status_t status;
	//1) codec software reset
	uint8_t tx_content = {0x00, 0x00};
	status = i2c_write_UDA(UDA_SOFT_ADRESS, tx_content);

	//2) config de registros
	tx_content[0] = 0x0F;
	tx_content[1] = 0x31;
	status = i2c_write_UDA(UDA_REG(0x00), tx_content);

	tx_content[0] = 0x00;
	tx_content[1] = 0x00;
	status = status = i2c_write_UDA(UDA_REG(0x01), tx_content);
	//turn on headphone!
	tx_content[0] = 0xA5;
	tx_content[1] = 0xDF;
	status = status = i2c_write_UDA(UDA_REG(0x02), tx_content);

	tx_content[0] = 0x00;
	tx_content[1] = 0x00;
	status = status = i2c_write_UDA(UDA_REG(0x03), tx_content);

	tx_content[0] = 0x02;
	tx_content[1] = 0x02;
	status = status = i2c_write_UDA(UDA_REG(0x04), tx_content);
	//solo quedan resets...
	tx_content[0] = 0x00;
	tx_content[1] = 0x00;
	status = status = i2c_write_UDA(UDA_REG(0x10), tx_content);


	status = status = i2c_write_UDA(UDA_REG(0x11), tx_content);


	status = status = i2c_write_UDA(UDA_REG(0x13), tx_content);

	return status;
}

status_t i2c_write_UDA(uint8_t tx_adress, uint8_t * tx_content){
	status_t status;
	status = I2C_MasterWriteBlocking(I2C0, &tx_adress, 1, kI2C_TransferNoStop);
	status = I2C_MasterWriteBlocking(I2C0, tx_content, 2, kI2C_TransferNoStop);
	return status;
}


Escribo el registro 0x00 (Prendo el DAC y configuro para que utilice el WSPLL)
	tempBuff[0] = 0x0F;
	tempBuff[1] = 0x31;
	UDA1380_write_reg_blocking( 0x00, tempBuff, 2);

	//Escribo el registro 0x01 ()
	tempBuff[0] = 0x00;
	tempBuff[1] = 0x00;
	UDA1380_write_reg_blocking( 0x01, tempBuff, 2);

	//Escribo el registro 0x02 (Prendo el WSPLL, el driver del parlante y ganancias asociadas)
	tempBuff[0] = 0xA5;
	tempBuff[1] = 0xDF;
	UDA1380_write_reg_blocking( 0x02, tempBuff, 2);

	//Escribo el registro 0x03 (Minimizo la atenuación hecha por el Codec)
	tempBuff[0] = 0x00;
	tempBuff[1] = 0x00;
	UDA1380_write_reg_blocking( 0x03, tempBuff, 2);

	//Escribo el registro 0x03 (Minimizo la atenuación hecha por el Codec)
	tempBuff[0] = 0x02;
	tempBuff[1] = 0x02;
	UDA1380_write_reg_blocking( 0x04, tempBuff, 2);

	//Escribo el registro 0x10 ()
	tempBuff[0] = 0x00;
	tempBuff[1] = 0x00;
	UDA1380_write_reg_blocking( 0x10, tempBuff, 2);

	//Escribo el registro 0x11 (Minimizo la atenuación del mixer)
	UDA1380_write_reg_blocking( 0x11, tempBuff, 2);

	//Escribo el registro 0x13 (Deshabilito el Master Mute)
	UDA1380_write_reg_blocking( 0x13, tempBuff, 2);

	return true;
}

bool UDA1380_turn_headphone_off(void)
{
	uint8_t tempBuff[2] = {0x85, 0xDF};
	UDA1380_write_reg_blocking( 0x02, tempBuff, 2);
	return true;
}

bool UDA1380_turn_headphone_on(void)
{
	uint8_t tempBuff[2] = {0xA5, 0xDF};
	UDA1380_write_reg_blocking( 0x02, tempBuff, 2);
	return true;
}
