/*
 * queue.h
 *
 *  Created on: Sep 1, 2019
 *      Author: Grupo 5
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#define MAX_EVENTS 30

#include "userData.h"

bool getEvent(UserData_t * ud);

bool pushEvent(UserData_t ud);

#endif /* QUEUE_H_ */
