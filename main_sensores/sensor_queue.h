/*
 * sensor_queue.h
 *
 *  Created on: Sep 1, 2019
 *      Author: Grupo 5
 */

#ifndef SENSOR_QUEUE_H_
#define SENSOR_QUEUE_H_

#include "queue.h"

bool pushEcgEvent(UdecgUd_t ecgUd);

bool pushOxEvent(oxUd_t oxUd);

bool pushTempEvent(tempUd_t tempUd);


#endif /* QUEUE_H_ */
