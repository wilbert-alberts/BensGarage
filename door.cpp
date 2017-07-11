/*
 * door.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#include <stdlib.h>

#include "log.h"
#include "hsi.h"
#include "debouncer.h"

#include "door.h"

#define CONTEXT_OPENED_SENSOR ((void*)1)
#define CONTEXT_CLOSED_SENSOR ((void*)2)

typedef enum {
	DOOR_OPEN, DOOR_INBETWEEN, DOOR_CLOSED
} Door_state_enum;

typedef struct {
  char            id;
	Door_state_enum state;

	CB_callbackClient openedCB;
	CB_callbackClient closedCB;
	CB_callbackClient inbetweenCB;

	Debouncer closedSensor;
	Debouncer openedSensor;

} Door_struct;

static void Door_sensorLow(void* obj, void* context);
static void Door_sensorHigh(void* obj, void* context);

Door Door_construct(char id, const CB_callbackClient* openedCB,
		const CB_callbackClient* inbetweenCB, const CB_callbackClient* closedCB,
		HSI_dio_struct openedSensor, HSI_dio_struct closedSensor) {

	Log_entry_P(PSTR("Door_construct"));

	Door_struct* result = (Door_struct*) calloc(1, sizeof(Door_struct));

  result->id = id;
	result->openedCB = *openedCB;
	result->inbetweenCB = *inbetweenCB;
	result->closedCB = *closedCB;

	CB_callbackClient cbOpenedLow;
	CB_callbackClient cbOpenedHigh;
	CB_callbackClient cbClosedLow;
	CB_callbackClient cbClosedHigh;

	cbOpenedLow.obj = result;
	cbOpenedLow.callback = Door_sensorLow;
	cbOpenedLow.context = CONTEXT_OPENED_SENSOR;

	cbOpenedHigh.obj = result;
	cbOpenedHigh.callback = Door_sensorHigh;
	cbOpenedHigh.context = CONTEXT_OPENED_SENSOR;

	cbClosedLow.obj = result;
	cbClosedLow.callback = Door_sensorLow;
	cbClosedLow.context = CONTEXT_CLOSED_SENSOR;

	cbClosedHigh.obj = result;
	cbClosedHigh.callback = Door_sensorHigh;
	cbClosedHigh.context = CONTEXT_CLOSED_SENSOR;

	result->closedSensor = Debouncer_construct(&cbClosedHigh, &cbClosedLow,
			closedSensor);
	result->openedSensor = Debouncer_construct(&cbOpenedHigh, &cbOpenedLow,
			openedSensor);

	Log_exit_P(PSTR("Door_construct"));
	return (Door) result;
}

static void Door_sensorLow(void* obj, void* context) {
	Log_entry_P(PSTR("Door_sensorLow"));

	Door_struct* door = (Door_struct*) obj;

	if (context == CONTEXT_OPENED_SENSOR) {
    Log("Door opened ");
    LogChar(door->id);
    Log("\n");
		CB_notify(&door->openedCB);
	} else if (context == CONTEXT_CLOSED_SENSOR) {
    Log("Door closed ");
    LogChar(door->id);
    Log("\n");
		CB_notify(&door->closedCB);
	} else {
		Log_error_PP(PSTR("Door_sensorLow"), PSTR("Illegal context, should  be CONTEXT_OPENED_SENSOR or CONTEXT_CLOSED_SENSOR"));
	}

	Log_exit_P(PSTR("Door_sensorLow"));
}

static void Door_sensorHigh(void* obj, void* context) {
	Log_entry_P(PSTR("Door_sensorHigh"));

	Door_struct* door = (Door_struct*) obj;

	if ((context == CONTEXT_CLOSED_SENSOR)
			|| (context == CONTEXT_OPENED_SENSOR)) {
    Log("Door moving (transient) ");
    LogChar(door->id);
    Log("\n");
		CB_notify(&door->inbetweenCB);
	} else {
		Log_error_PP(PSTR("Door_sensorHigh"), PSTR("Illegal context, should  be CONTEXT_OPENED_SENSOR or CONTEXT_CLOSED_SENSOR"));
	}

	Log_exit_P(PSTR("Door_sensorHigh"));
}
