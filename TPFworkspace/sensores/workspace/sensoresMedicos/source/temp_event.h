/*
 * temp_event.h
 *
 *  Created on: 30 Jan 2021
 *      Author: user
 */

#ifndef TEMP_EVENT_H_
#define TEMP_EVENT_H_

#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "timers.h"


bool IsTempEvent(void);

void PushTempEvent(TimerHandle_t);

void PopTempEvent(void);

#endif /* TEMP_EVENT_H_ */
