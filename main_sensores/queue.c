/*
 * queue.c
 *
 *  Created on: Sep 1, 2019
 *      Author: Grupo 5
 */

#include "queue.h"

#define MAX_CANT_EVENTS 200

typedef struct{
	eventData_t eventsList[MAX_CANT_EVENTS];
	int top;
}events_t;

events_t ev_queue;
ev_queue.top = -1;

bool getEvent(eventData_t * ev){
	bool status;
	ev_name name = NUM_EVENTS;
	if(ev_queue.top >= 0){
		&ev = ev_queue.eventsList[ev_queue.top];
		ev_queue.top = ev_queue.top-1;
		status = true;
	} 
	else{
		status = false;
	}
	return status;
}

bool pushEvent(eventData_t * ev){
	bool status;
	if(ev_queue.top < MAX_CANT_EVENTS){
		ev_queue.top = ev_queue.top+1;
		ev_queue.eventsList[ev_queue.top] = &ev;
		status = true;
	}
	else{
		status = false;
	}
	return status;
}

