/*
 * ox_event.h
 *
 *  Created on: 30 Jan 2021
 *      Author: user
 */

#ifndef OX_EVENT_H_
#define OX_EVENT_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct{
	 uint16_t red_sample;
	 uint16_t ir_sample;
}ox_event_t;

bool IsOxEvent(void);
void PushOxEvent(uint16_t red_sample, uint16_t ir_sample);
ox_event_t PopOxEvent(void);

#endif /* OX_EVENT_H_ */
