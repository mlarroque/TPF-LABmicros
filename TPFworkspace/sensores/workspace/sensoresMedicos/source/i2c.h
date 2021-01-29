/*
 * i2c.h
 *
 *  Created on: 27 Jan 2021
 *      Author: user
 */

#ifndef I2C_H_
#define I2C_H_

/********************************************************
 * 					HEADERS UTILIZADOS					*
 ********************************************************/
#include <stdint.h>
#include <stdbool.h>

/********************************************************
 * 						FUNCIONES						*
 ********************************************************/
bool WriteByte(uint8_t address, uint8_t reg, uint8_t* tx_buffer, uint8_t n_bytes);

bool ReadByte(uint8_t address, uint8_t reg, uint8_t * r_buff, uint8_t n_bytes);

#endif /* I2C_H_ */
