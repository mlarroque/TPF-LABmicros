/*
 * UDA.c
 *
 *  Created on: 3 feb. 2021
 *      Author: Matias
 */


#include "UDA.h"

#include "fsl_debug_console.h"


#define UDA_ADDRES_RX 0x18
#define UDA_SOFT_RESET_ADDRES 0x7F
#define UDA_REG(a)  (a)

status_t UDA_WriteReg(codec_handle_t *handle, uint16_t reg, uint16_t val);

status_t UDA_ModifyReg(codec_handle_t *handle, uint16_t reg, uint16_t clr_mask, uint16_t val);

status_t UDA_ReadReg(codec_handle_t *handle, uint16_t reg, uint16_t *val);


status_t UDA_init(codec_handle_t *handle, void *config){
	status_t status;
	uint16_t readValCheck = -1;
    handle->slaveAddress = UDA_ADDRES_RX;

// Reset
    status = UDA_WriteReg(handle, UDA_SOFT_RESET_ADDRES, 0x0000);


//WSPLL settings

    status = UDA_WriteReg(handle, UDA_REG(0x00), 0x0F31);
    //status = UDA_ReadReg(handle, UDA_REG(0x00), &readValCheck);
//01H I2S-bus I/O settings
    status = UDA_WriteReg(handle, UDA_REG(0x01), 0x0000);

//02H power control settings - max volume
    status = UDA_WriteReg(handle, UDA_REG(0x02), 0xA5DF);

//03H analog mixer settings
    status = UDA_WriteReg(handle, UDA_REG(0x03), 0x0000);

//04H headphone amplifier settings
    status = UDA_WriteReg(handle, UDA_REG(0x04), 0x0202);
//resets and unmutes
    status = UDA_WriteReg(handle, UDA_REG(0x10), 0x0000);
    status = UDA_WriteReg(handle, UDA_REG(0x11), 0x0000);
    status = UDA_WriteReg(handle, UDA_REG(0x12), 0x0000);
    status = UDA_WriteReg(handle, UDA_REG(0x13), 0x0000);
    status = UDA_ReadReg(handle, UDA_REG(0x13), &readValCheck);
    //PRINTF("READ VAL CHECK: %d \n", readValCheck);
    //status = UDA_WriteReg(handle, UDA_REG(0x14), 0x0000);
    //status = UDA_WriteReg(handle, UDA_REG(0x21), 0x0000);
    //status = UDA_ReadReg(handle, UDA_REG(0x04), &readValCheck);

	return status;
}


status_t UDA_WriteReg(codec_handle_t *handle, uint16_t reg, uint16_t val)
{
    status_t retval = 0;

    retval = CODEC_I2C_WriteReg(handle->slaveAddress, kCODEC_RegAddr8Bit, reg, kCODEC_RegWidth16Bit, val,
                                handle->I2C_SendFunc);

    return retval;
}


status_t UDA_ReadReg(codec_handle_t *handle, uint16_t reg, uint16_t *val)
{
    status_t retval = 0;

   retval = CODEC_I2C_ReadReg(handle->slaveAddress, kCODEC_RegAddr8Bit, reg, kCODEC_RegWidth16Bit, val,
                               handle->I2C_ReceiveFunc);

    return retval;
}

status_t UDA_ModifyReg(codec_handle_t *handle, uint16_t reg, uint16_t clr_mask, uint16_t val)
{
    status_t retval = 0;
    uint16_t reg_val;

    /* Read the register value out */
    retval = UDA_ReadReg(handle, reg, &reg_val);
    if (retval != kStatus_Success)
    {
        return kStatus_Fail;
    }

    /* Modify the value */
    reg_val &= clr_mask;
    reg_val |= val;

    /* Write the data to register */
    retval = UDA_WriteReg(handle, reg, reg_val);
    if (retval != kStatus_Success)
    {
        return kStatus_Fail;
    }

    return kStatus_Success;
}


status_t UDA_ConfigDataFormat(codec_handle_t *handle, uint32_t mclk, uint32_t sample_rate, uint32_t bits){
	return kStatus_Success;
}

status_t UDA_deinit(codec_handle_t *handle){
	return kStatus_Success;
}
