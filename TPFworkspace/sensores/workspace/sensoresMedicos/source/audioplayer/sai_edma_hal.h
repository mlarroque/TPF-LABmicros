/*
 * sai_edma_hal.h
 *
 *  Created on: 11 feb. 2021
 *      Author: Matias
 */

#ifndef SAI_EDMA_HAL_H_
#define SAI_EDMA_HAL_H_

#include "fsl_sai_edma.h"

void sai_edma_init(sai_edma_callback_t callback, void *userData);

void sai_edma_reset(sai_edma_callback_t callback, void *userData);

uint32_t getMasterClockHz(void);
uint32_t getSampleRateHz(void);
uint32_t getBitWidth(void);

status_t sendSAIdata(sai_transfer_t * p2xfer);

void stopSAIsend(void);
void sai_deinit(void);



#endif /* SAI_EDMA_HAL_H_ */
