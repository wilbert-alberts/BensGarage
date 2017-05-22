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
static void LM_logSideAndState(LM_side_enum side, LM_trafficLightState state);


LightMaster LM_construct(HSI_dio_struct red, HSI_dio_struct yellow,
		HSI_dio_struct green, HSI_dio_struct ambient) {
	Log_entry(PSTR("LM_construct"));

	LightMaster_struct* result = (LightMaster_struct*) calloc(1,
			sizeof(LightMaster_struct));

	result->leftLight = LM_OFF;
	result->rightLight = LM_OFF;
	result->trafficLightOwner = LM_NONE;

	result->trafficLight = TL_construct(red, yellow, green);

	result->ambient = ambient;
	result->ambientTimer = Timer_construct();
	CB_callbackClient cb = { result, NULL, LM_ambientOffCB };
	result->ambientOffCB = cb;

	Log_exit(PSTR("LM_construct"));
	return result;
}

void LM_setLight(LightMaster lm, LM_side_enum side, LM_trafficLightState state) {
	Log_entry(PSTR("LM_setLight"));

	LightMaster_struct* obj = (LightMaster_struct*) lm;

  LM_logSideAndState(side, state);
 
	LM_trafficLightState newState;

	// Update traffic light state and determine owner
	switch (side) {
	case LM_LEFT:
		obj->leftLight = state;
		if ((state == LM_OFF) && (obj->rightLight != LM_OFF)) {
			obj->trafficLightOwner = LM_RIGHT;
      Logln(PSTR("Owner set to Right"));
		} else {
			if ((state == LM_OFF) && (obj->rightLight == LM_OFF)) {
				obj->trafficLightOwner = LM_NONE;
        Logln(PSTR("Owner set to None"));
			} else {
				if ((state != LM_OFF) && (obj->rightLight == LM_OFF)) {
					obj->trafficLightOwner = LM_LEFT;
          Logln(PSTR("Owner set to Left"));
				} else {
					Log_error(PSTR("LM_setLight"),
							PSTR("Unable to determine traffic light owner"));
				}
			}
		}

		break;
	case LM_RIGHT:
		obj->rightLight = state;
		if ((state == LM_OFF) && (obj->leftLight != LM_OFF)) {
			obj->trafficLightOwner = LM_LEFT;
      Logln(PSTR("Owner set to Left"));
		} else {
			if ((state == LM_OFF) && (obj->leftLight == LM_OFF)) {
				obj->trafficLightOwner = LM_NONE;
        Logln(PSTR("Owner set to None"));
			} else {
				if ((state != LM_OFF) && (obj->leftLight == LM_OFF)) {
					obj->trafficLightOwner = LM_RIGHT;
          Logln(PSTR("Owner set to Right"));
				} else {
					Log_error(PSTR("LM_setLight"),
							PSTR("Unable to determine traffic light owner"));
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
				Log_error(PSTR("LM_setLight"), PSTR("Unable to determine color"));
			}
		}
	}

	// Set traffic light
	switch (newState) {
	case LM_OFF:
		TL_off(obj->trafficLight);
		Timer_setTimer(obj->ambientTimer, &obj->ambientOffCB,
		LM_AMBIENT_OFF_TIMER);
    Logln(PSTR("State set to Off"));
		break;
	case LM_GREEN:
		TL_green(obj->trafficLight);
		LM_ambientOn(obj);
    Logln(PSTR("Light set to Green"));
		break;
	case LM_YELLOW:
		TL_yellow(obj->trafficLight);
		LM_ambientOn(obj);
    Logln(PSTR("Light set to Yellow"));
		break;
	case LM_YELLOW_FLASH:
		TL_yellow(obj->trafficLight);
		LM_ambientOn(obj);
    Logln(PSTR("Light set to Yellow (flash)"));
		break;
	case LM_RED:
		TL_red(obj->trafficLight);
		LM_ambientOn(obj);
    Logln(PSTR("Light set to Red"));
		break;
	default:
		Log_error(PSTR("LM_setLight"), PSTR("Illegal color"));
		break;
	}

	Log_exit(PSTR("LM_setLight"));
}

static void LM_ambientOn(LightMaster_struct* obj) {
	Log_entry(PSTR("LM_ambientOn"));

	HSI_writePin(obj->ambient, 1);
	Timer_cancelTimer(obj->ambientTimer);

	Log_exit(PSTR("LM_ambientOn"));
}

static void LM_ambientOffCB(void* lm, void* context) {
	Log_entry(PSTR("LM_ambientOffCB"));

	LightMaster_struct* obj = (LightMaster_struct*) lm;
	HSI_writePin(obj->ambient, 0);

	Log_exit(PSTR("LM_ambientOffCB"));
}

static void LM_logSideAndState(LM_side_enum side, LM_trafficLightState state)
{
  if (side == LM_LEFT)
    Log(PSTR(" side: Left,"));
  if (side == LM_RIGHT)
    Log(PSTR(" side: Right,"));
  if (state == LM_OFF)
    Log(PSTR(" state: Off\n"));
  if (state == LM_RED)
    Log(PSTR(" state: Red\n"));
  if (state == LM_YELLOW)
    Log(PSTR(" state: Yellow\n"));
  if (state == LM_YELLOW_FLASH)
    Log(PSTR(" state: Yellow (flash)\n"));
  if (state == LM_GREEN)
    Log(PSTR(" state: Green\n"));
}

