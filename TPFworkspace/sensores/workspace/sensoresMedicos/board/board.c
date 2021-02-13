/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include <stdint.h>
#include "fsl_common.h"
#include "fsl_debug_console.h"
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_i2c.h"
#endif /* SDK_I2C_BASED_COMPONENT_USED */

#if defined BOARD_USE_CODEC
#include "i2c.h"
#include "fsl_sgtl5000.h"
#include "UDA.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "audio_i2c_config.h"
#endif
/*******************************************************************************
 * Variables
 ******************************************************************************/
#if defined BOARD_USE_CODEC
status_t BOARD_Codec_I2C_Send(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, const uint8_t *txBuff, uint8_t txBuffSize);
status_t BOARD_Codec_I2C_Receive(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);

codec_config_t boardCodecConfig = {
    .I2C_SendFunc = BOARD_Codec_I2C_Send,
    .I2C_ReceiveFunc = BOARD_Codec_I2C_Receive,
	.op.Init = UDA_init,
    .op.Deinit = UDA_deinit,
    .op.SetFormat = UDA_ConfigDataFormat
};


status_t BOARD_Codec_I2C_Send(uint8_t deviceAddress,
                        uint32_t subAddress,
                        uint8_t subaddressSize,
                        const uint8_t *txBuff,
                        uint8_t txBuffSize)
{

	uint8_t reg = (uint8_t)((subAddress) >> (8U * subaddressSize));

    return WriteByte(deviceAddress, reg, txBuff, txBuffSize);
}

status_t BOARD_Codec_I2C_Receive(uint8_t deviceAddress,
                           uint32_t subAddress,
                           uint8_t subaddressSize,
                           uint8_t *rxBuff,
                           uint8_t rxBuffSize)
{


	uint8_t reg = (uint8_t)((subAddress) >> (8U * subaddressSize));

	return ReadByte(deviceAddress, reg, rxBuff, rxBuffSize);
}

void BOARD_audio_init(void){
	//BOARD_InitPins();   //pin_mux
	//BOARD_BootClockRUN();  //clock_config
	BOARD_I2C_ReleaseBus();  //audio_i2c_config
	//BOARD_I2C_ConfigurePins();  //pin_mux
	//BOARD_Codec_I2C_Init();  //board
	//BOARD_InitDebugConsole();

}
#endif

/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ;
    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
}
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
void BOARD_I2C_Init(I2C_Type *base, uint32_t clkSrc_Hz)
{
    i2c_master_config_t i2cConfig = {0};

    I2C_MasterGetDefaultConfig(&i2cConfig);
    I2C_MasterInit(base, &i2cConfig, clkSrc_Hz);
}

status_t BOARD_I2C_Send(I2C_Type *base,
                        uint8_t deviceAddress,
                        uint32_t subAddress,
                        uint8_t subaddressSize,
                        uint8_t *txBuff,
                        uint8_t txBuffSize)
{
    i2c_master_transfer_t masterXfer;

    /* Prepare transfer structure. */
    masterXfer.slaveAddress   = deviceAddress;
    masterXfer.direction      = kI2C_Write;
    masterXfer.subaddress     = subAddress;
    masterXfer.subaddressSize = subaddressSize;
    masterXfer.data           = txBuff;
    masterXfer.dataSize       = txBuffSize;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    return I2C_MasterTransferBlocking(base, &masterXfer);
}

status_t BOARD_I2C_Receive(I2C_Type *base,
                           uint8_t deviceAddress,
                           uint32_t subAddress,
                           uint8_t subaddressSize,
                           uint8_t *rxBuff,
                           uint8_t rxBuffSize)
{
    i2c_master_transfer_t masterXfer;

    /* Prepare transfer structure. */
    masterXfer.slaveAddress   = deviceAddress;
    masterXfer.subaddress     = subAddress;
    masterXfer.subaddressSize = subaddressSize;
    masterXfer.data           = rxBuff;
    masterXfer.dataSize       = rxBuffSize;
    masterXfer.direction      = kI2C_Read;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    return I2C_MasterTransferBlocking(base, &masterXfer);
}

void BOARD_Accel_I2C_Init(void)
{
    BOARD_I2C_Init(BOARD_ACCEL_I2C_BASEADDR, BOARD_ACCEL_I2C_CLOCK_FREQ);
}

status_t BOARD_Accel_I2C_Send(uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint32_t txBuff)
{
    uint8_t data = (uint8_t)txBuff;

    return BOARD_I2C_Send(BOARD_ACCEL_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, &data, 1);
}

status_t BOARD_Accel_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_I2C_Receive(BOARD_ACCEL_I2C_BASEADDR, deviceAddress, subAddress, subaddressSize, rxBuff, rxBuffSize);
}
#endif /* SDK_I2C_BASED_COMPONENT_USED */
