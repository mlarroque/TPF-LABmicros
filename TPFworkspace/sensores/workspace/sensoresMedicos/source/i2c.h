/*
 * i2c.h
 *
 *  Created on: 27 Jan 2021
 *      Author: user
 */

#ifndef I2C_H_
#define I2C_H_

/********************************************************
 * 						FUNCIONES						*
 ********************************************************/
uint8_t WriteByte(uint8_t address, uint8_t reg, uint8_t byte);

uint8_t ReadByte(uint8_t address, uint8_t reg);

#endif /* I2C_H_ */
