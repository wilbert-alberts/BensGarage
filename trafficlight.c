/*
 * trafficlight.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#include <stdlib.h>

#include "log.h"
#include "lamp.h"
#include "trafficlight.h"
#include "timer.h"

typedef struct {
	Lamp red;
	Lamp yellow;
	Lamp green;
} TrafficLight_struct;

TrafficLight TL_construct(HSI_dio_struct red, HSI_dio_struct yellow,
		HSI_dio_struct green) {
	Log_entry(__func__, "");

	TrafficLight_struct* result = (TrafficLight_struct*) calloc(1,
			sizeof(TrafficLight_struct));

	result->red = Lamp_construct(red);
	result->yellow = Lamp_construct(yellow);
	result->green = Lamp_construct(green);

	Log_exit(__func__, "");
	return (TrafficLight) result;
}

void TL_red(TrafficLight tl) {
	Log_entry(__func__, "");

	TrafficLight_struct* obj = (TrafficLight_struct*) tl;

	Lamp_off(obj->yellow);
	Lamp_off(obj->green);
	Lamp_on(obj->red);

	Log_exit(__func__, "");
}

void TL_yellow(TrafficLight tl) {
	Log_entry(__func__, "");

	TrafficLight_struct* obj = (TrafficLight_struct*) tl;

	Lamp_off(obj->red);
	Lamp_off(obj->green);
	Lamp_on(obj->yellow);

	Log_exit(__func__, "");
}

void TL_yellowFlash(TrafficLight tl) {
	Log_entry(__func__, "");

	TrafficLight_struct* obj = (TrafficLight_struct*) tl;

	Lamp_off(obj->red);
	Lamp_off(obj->green);
	Lamp_flash(obj->yellow, TIMER_SECONDS(1));

	Log_exit(__func__, "");
}

void TL_green(TrafficLight tl) {
	Log_entry(__func__, "");

	TrafficLight_struct* obj = (TrafficLight_struct*) tl;

	Lamp_off(obj->red);
	Lamp_off(obj->yellow);
	Lamp_on(obj->green);

	Log_exit(__func__, "");
}

void TL_off(TrafficLight tl) {
	Log_entry(__func__, "");

	TrafficLight_struct* obj = (TrafficLight_struct*) tl;

	Lamp_off(obj->red);
	Lamp_off(obj->yellow);
	Lamp_off(obj->green);

	Log_exit(__func__, "");
}

