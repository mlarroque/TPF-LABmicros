/*
 * event_prueba.h
 *
 *  Created on: 30 Jan 2021
 *      Author: user
 */

#ifndef EVENT_PRUEBA_H_
#define EVENT_PRUEBA_H_

#include <stdbool.h>
#define EVENT_ARRAY_SIZE 30

bool IsEvent(void);
void PushEvent(uint16_t);
int32_t PopEvent(void);

#endif /* EVENT_PRUEBA_H_ */
