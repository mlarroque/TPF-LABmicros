/*
 * bluetooth_com.h
 *
 *  Created on: Jan 24, 2021
 *      Author: Lu
 */

#ifndef BLUETOOTH_COM_H_
#define BLUETOOTH_COM_H_


void InitBluetooth(void);

void sendBTPackage(int32_t heartRate, int32_t sp02, uint16_t temp, int32_t ecg_samples[], int32_t ox_samples[]);


#endif /* BLUETOOTH_COM_H_ */
