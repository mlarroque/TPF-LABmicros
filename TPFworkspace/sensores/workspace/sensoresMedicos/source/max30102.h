/*
 * max30102.h
 *
 *  Created on: 26 Jan 2021
 *      Author: user
 */

#ifndef MAX30102_H_
#define MAX30102_H_

/************************************************************
 * 					HEADERS UTILIZADOS						*
 ************************************************************/
#include "timer.h"

/************************************************************
 *						DEFINICIONES						*
 ************************************************************/
typedef struct{
	callback_ptr callback;
	unsigned long int timeout;
}max_init_t;
/************************************************************
 * 						FUNCIONES							*
 ************************************************************/

void InitializeHardware(max_init_t*);



#endif /* MAX30102_H_ */
