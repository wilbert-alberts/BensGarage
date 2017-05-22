/*
 * door.h
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifndef DOOR_H_
#define DOOR_H_

#include "callback.h"
#include "hsi.h"

typedef void* Door;

Door Door_construct(char id, const CB_callbackClient* openedCB,
		const CB_callbackClient* inbetweenCB, const CB_callbackClient* closedCB,
		HSI_dio_struct openedSensor, HSI_dio_struct closedSensor);

#endif /* DOOR_H_ */
