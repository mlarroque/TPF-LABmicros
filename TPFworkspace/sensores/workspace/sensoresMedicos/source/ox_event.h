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


bool IsOxEvent(void);
void PushOxEvent(void);
void PopOxEvent(void);

#endif /* OX_EVENT_H_ */
