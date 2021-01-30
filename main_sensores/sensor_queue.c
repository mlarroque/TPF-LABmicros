/*
 * queue.c
 *
 *  Created on: Sep 1, 2019
 *      Author: Grupo 5
 */

#include "sensor_queue.h"

bool pushEcgEvent(UdecgUd_t ecgUd){
	UserData_t ud;
	ud.ecgUd = ecgUd;
	bool status = pushEvent(&ud);
	return status;
}

bool pushOxEvent(oxUd_t oxUd){
	UserData_t ud;
	ud.oxUd = oxUd;
	bool status = pushEvent(&ud);
	return status;
}

bool pushTempEvent(tempUd_t tempUd){
	UserData_t ud;
	ud.tempUd = tempUd;
	bool status = pushEvent(&ud);
	return status;
}