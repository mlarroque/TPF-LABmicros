/*
 * userData.h
 *
 *  Created on: Sep 6, 2019
 *      Author: Grupo 5
 */

#ifndef USERDATA_H_
#define USERDATA_H_

#include "sensors.h"

typedef enum {MEAS_OX_EV, MEAS_ECG_EV, MEAS_TEMP_EV, NUM_EVENTS}ev_name;

/* UserData contiene toda la informacion necesaria que necesita una rutina para manejar
 un evento que se saco de la cola.
 */

typedef struct{
	oxUd_t oxUd;
	tempUd_t tempUd;
	ecgUd_t ecgUd;
	ev_name name;
}UserData_t;

#endif /* USERDATA_H_ */
