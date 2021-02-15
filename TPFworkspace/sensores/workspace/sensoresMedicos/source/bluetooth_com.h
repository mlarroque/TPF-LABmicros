/*
 * bluetooth_com.h
 *
 *  Created on: Jan 24, 2021
 *      Author: Lu
 */

#ifndef BLUETOOTH_COM_H_
#define BLUETOOTH_COM_H_

#include <stdint.h>

typedef struct{
	int32_t heartRate;
	int32_t sp02;
	uint16_t temp;
	int32_t ecg_samples[20];
	uint16_t n_samples_ecg;
	int32_t ox_samples[5];
	uint16_t n_samples_ppg;
}data_BT_t;


void InitBluetooth(void);

void sendBTPackage(data_BT_t pkg);

void BlueWaitForSamples(void);


#endif /* BLUETOOTH_COM_H_ */
