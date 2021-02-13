/*
 * i2c_config.h
 *
 *  Created on: 11 feb. 2021
 *      Author: Matias
 */

#ifndef AUDIO_I2C_CONFIG_H_
#define AUDIO_I2C_CONFIG_H_

#define I2C_RELEASE_SDA_PORT PORTE
#define I2C_RELEASE_SCL_PORT PORTE
#define I2C_RELEASE_SDA_GPIO GPIOE
#define I2C_RELEASE_SDA_PIN 25U
#define I2C_RELEASE_SCL_GPIO GPIOE
#define I2C_RELEASE_SCL_PIN 24U



#define I2C_RELEASE_BUS_COUNT 100U

void BOARD_I2C_ReleaseBus(void);

#endif /* AUDIO_I2C_CONFIG_H_ */
