/*
 * UDA.h
 *
 *  Created on: 3 feb. 2021
 *      Author: Matias
 */

#ifndef UDA_H_
#define UDA_H_

#include "fsl_codec_common.h"

status_t UDA_init(codec_handle_t *handle, void *config);

status_t UDA_deinit(codec_handle_t *handle);

status_t UDA_ConfigDataFormat(codec_handle_t *handle, uint32_t mclk, uint32_t sample_rate, uint32_t bits);

#endif /* UDA_H_ */
