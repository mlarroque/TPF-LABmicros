/*
 * sensors.h
 *
 *  Created on: Sep 6, 2019
 *      Author: Grupo 5
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#define MAX_BUFFER_SIZE 50

#include "ecg.h"
// int32_t GetEcgSample(void);
// uint16_t GetEcgUnreadNum(void);

#include "oximetry.h"
// pleth_sample_t GetPlethSample(void);
// uin16_t GetUnreadNum(void);
// int32_t GetRedPlethSample(void);
// uin16_t GetRedUnreadNum(void);

#include "thermometer.h"
// uint8_t GetThermoSample(void);
// uint16_t GetTempUnreadNum(void);

typedef struct{
	uint16_t sample_red;
	uint16_t sample_ir;
}oxUd_t;

typedef struct{
	uint16_t sample;
}tempUd_t;

typedef struct{
	uint16_t sample;
}ecgUd_t;

#endif /* SENSORS_H_ */