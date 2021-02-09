/*
 * queue.h
 *
 *  Created on: Sep 1, 2019
 *      Author: Grupo 5
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#define MAX_EVENTS 30

#include "eventData.h"

bool getEvent(eventData_t * ev);

bool pushEvent(eventData_t ev);

#endif /* QUEUE_H_ */
