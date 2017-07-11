/*
 * controller.h
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "callback.h"
#include "hsi.h"

#define AUTO_CLOSE_DOOR_TIMEOUT TIMER_MINUTES(3)
//#define AUTO_CLOSE_DOOR_TIMEOUT TIMER_SECONDS(10)
#define AUTO_CLOSE_DOOR_PULSE   TIMER_SECONDS(1)

typedef void* Controller;

Controller Controller_construct(char id, HSI_dio_struct openedSensor, HSI_dio_struct closedSensor, HSI_dio_struct gateSensor, HSI_dio_struct remote,
		CB_callbackClient cbRed, CB_callbackClient cbYellow, CB_callbackClient cbYellowFlash, CB_callbackClient cbGreen, CB_callbackClient cbOff);

#endif /* CONTROLLER_H_ */
