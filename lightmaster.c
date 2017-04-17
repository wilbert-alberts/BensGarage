/*
 * lightmaster.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#include <stdlib.h>

#include "log.h"
#include "lightmaster.h"
#include "trafficlight.h"
#include "hsi.h"
#include "callback.h"
#include "timer.h"

typedef struct {
	LM_trafficLightState leftLight;
	LM_trafficLightState rightLight;
	LM_side_enum trafficLightOwner;

	TrafficLight trafficLight;

	HSI_dio_struct ambient;
	CB_callbackClient ambientOffCB;
	Timer ambientTimer;
} LightMaster_struct;

static void LM_ambientOn(LightMaster_struct* obj);
static void LM_ambientOffCB(void* lm, void* context);

LightMaster LM_construct(HSI_dio_struct red, HSI_dio_struct yellow,
		HSI_dio_struct green, HSI_dio_struct ambient) {
	Log_entry(__func__, "");

	LightMaster_struct* result = (LightMaster_struct*) calloc(1,
			sizeof(LightMaster_struct));

	result->leftLight = LM_OFF;
	result->rightLight = LM_OFF;
	result->trafficLightOwner = LM_NONE;

	result->trafficLight = TL_construct(red, yellow, green);

	result->ambient = ambient;
	result->ambientTimer = Timer_construct();
	CB_callbackClient cb = { result, LM_ambientOffCB, NULL };
	result->ambientOffCB = cb;

	Log_exit(__func__, "");
	return result;
}

void LM_setLight(LightMaster lm, LM_side_enum side, LM_trafficLightState state) {
	Log_entry(__func__, "");

	LightMaster_struct* obj = (LightMaster_struct*) lm;
	LM_trafficLightState newState;

	// Update traffic light state and determine owner
	switch (side) {
	case LM_LEFT:
		obj->leftLight = state;
		if ((state == LM_OFF) && (obj->rightLight != LM_OFF)) {
			obj->trafficLightOwner = LM_RIGHT;
		} else {
			if ((state == LM_OFF) && (obj->rightLight == LM_OFF)) {
				obj->trafficLightOwner = LM_NONE;
			} else {
				if ((state != LM_OFF) && (obj->rightLight == LM_OFF)) {
					obj->trafficLightOwner = LM_LEFT;
				} else {
					Log_error(__func__,
							"Unable to determine traffic light owner");
				}
			}
		}

		break;
	case LM_RIGHT:
		obj->rightLight = state;
		if ((state == LM_OFF) && (obj->leftLight != LM_OFF)) {
			obj->trafficLightOwner = LM_LEFT;
		} else {
			if ((state == LM_OFF) && (obj->leftLight == LM_OFF)) {
				obj->trafficLightOwner = LM_NONE;
			} else {
				if ((state != LM_OFF) && (obj->leftLight == LM_OFF)) {
					obj->trafficLightOwner = LM_RIGHT;
				} else {
					Log_error(__func__,
							"Unable to determine traffic light owner");
				}
			}
		}

		break;
	default:
		break;
	}

	// determine new color for light
	newState = LM_OFF;
	if (obj->trafficLightOwner == LM_NONE) {
		newState = LM_OFF;
	} else {
		if (obj->trafficLightOwner == LM_LEFT) {
			newState = obj->leftLight;
		} else {
			if (obj->trafficLightOwner == LM_RIGHT) {
				newState = obj->rightLight;
			} else {
				Log_error(__func__, "Unable to determine color");
			}
		}
	}

	// Set traffic light
	switch (newState) {
	case LM_OFF:
		TL_off(obj->trafficLight);
		Timer_setTimer(obj->ambientTimer, &obj->ambientOffCB,
		LM_AMBIENT_OFF_TIMER);
		break;
	case LM_GREEN:
		TL_green(obj->trafficLight);
		LM_ambientOn(obj);
		break;
	case LM_YELLOW:
		TL_yellow(obj->trafficLight);
		LM_ambientOn(obj);
		break;
	case LM_YELLOW_FLASH:
		TL_yellow(obj->trafficLight);
		LM_ambientOn(obj);
		break;
	case LM_RED:
		TL_red(obj->trafficLight);
		LM_ambientOn(obj);
		break;
	default:
		Log_error(__func__, "Illegal color");
		break;
	}

	Log_exit(__func__, "");
}

static void LM_ambientOn(LightMaster_struct* obj) {
	Log_entry(__func__, "");

	HSI_writePin(obj->ambient, 1);
	Timer_cancelTimer(obj->ambientTimer);

	Log_exit(__func__, "");
}

static void LM_ambientOffCB(void* lm, void* context) {
	Log_entry(__func__, "");

	LightMaster_struct* obj = (LightMaster_struct*) lm;
	HSI_writePin(obj->ambient, 0);

	Log_exit(__func__, "");
}
