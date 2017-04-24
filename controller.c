/*
 * controller.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#include <stdlib.h>

#include "log.h"
#include "door.h"
#include "trafficlight.h"
#include "debouncer.h"
#include "callback.h"
#include "lamp.h"
#include "timer.h"

#include "controller.h"

#define AUTO_CLOSE_DOOR_TIMEOUT (uint32_t)(10*60*1000L)

typedef enum {
	IN_BETWEEN, OPENED, CLOSED, FREE, BLOCKED
} Controller_event_enum;

typedef void (*Controller_stateHandler)(void* obj, Controller_event_enum event);

typedef struct {
	Door door;
	Debouncer gate;

	Timer autoCloseTimer;
	CB_callbackClient cbAutoClose;
	HSI_dio_struct doorRemote;

	CB_callbackClient cbRed;
	CB_callbackClient cbYellow;
	CB_callbackClient cbYellowFlash;
	CB_callbackClient cbGreen;
	CB_callbackClient cbOff;

	Controller_stateHandler handler;
} Controller_struct;

static void Controller_doorOpened(void* obj, void* context);
static void Controller_doorInBetween(void* obj, void* context);
static void Controller_doorClosed(void* obj, void* context);
static void Controller_gateFree(void* obj, void* context);
static void Controller_gateBlocked(void* obj, void* context);
static void Controller_autoClose(void* obj, void* context);

static void Controller_State_Closed_Free(void* controller,
		Controller_event_enum event);
static void Controller_State_In_Between_Free(void* controller,
		Controller_event_enum event);
static void Controller_State_Opened_Free(void* controller,
		Controller_event_enum event);
static void Controller_State_Closed_Blocked(void* controller,
		Controller_event_enum event);
static void Controller_State_In_Between_Blocked(void* controller,
		Controller_event_enum event);
static void Controller_State_Opened_Blocked(void* controller,
		Controller_event_enum event);

Controller Controller_construct(HSI_dio_struct openedSensor,
		HSI_dio_struct closedSensor, HSI_dio_struct gateSensor,
		HSI_dio_struct remote, CB_callbackClient cbRed,
		CB_callbackClient cbYellow, CB_callbackClient cbYellowFlash,
		CB_callbackClient cbGreen, CB_callbackClient cbOff) {
	Log_entry(__func__, "");

	Controller_struct* result = calloc(1, sizeof(Controller_struct));

	CB_callbackClient openedCB = { result, NULL, Controller_doorOpened};
	CB_callbackClient inbetweenCB = { result, NULL, Controller_doorInBetween};
	CB_callbackClient closedCB = { result, NULL, Controller_doorClosed };
	result->door = Door_construct(&openedCB, &inbetweenCB, &closedCB,
			openedSensor, closedSensor);

	CB_callbackClient gateHighCB = { result, NULL, Controller_gateBlocked };
	CB_callbackClient gateLowCB = { result, NULL, Controller_gateFree };
	result->gate = Debouncer_construct(&gateHighCB, &gateLowCB, gateSensor);

	result->cbRed = cbRed;
	result->cbYellow = cbYellow;
	result->cbGreen = cbGreen;
	result->cbOff = cbOff;

	result->handler = Controller_State_Closed_Free;

	result->autoCloseTimer = Timer_construct();
	CB_callbackClient cbAutoClose = { result, Controller_autoClose, NULL };
	result->cbAutoClose = cbAutoClose;
	result->doorRemote = remote;

	Log_exit(__func__, "");
	return result;
}

static void Controller_doorOpened(void* controller, void* context) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;
	obj->handler(obj, OPENED);

	Log_exit(__func__, "");
}

static void Controller_doorInBetween(void* controller, void* context) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;
	obj->handler(obj, IN_BETWEEN);

	Log_exit(__func__, "");
}

static void Controller_doorClosed(void* controller, void* context) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;
	obj->handler(obj, CLOSED);

	Log_exit(__func__, "");
}

static void Controller_gateFree(void* controller, void* context) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;
	obj->handler(obj, FREE);

	Log_exit(__func__, "");
}

static void Controller_gateBlocked(void* controller, void* context) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;
	obj->handler(obj, BLOCKED);

	Log_exit(__func__, "");
}

static void Controller_State_Closed_Free(void* controller,
		Controller_event_enum event) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;
	switch (event) {
	case IN_BETWEEN:
		CB_notify(&obj->cbYellow);
		obj->handler = Controller_State_In_Between_Free;
		break;
	case BLOCKED:
		CB_notify(&obj->cbYellowFlash);
		obj->handler = Controller_State_Closed_Blocked;
		break;
	case OPENED:
	case CLOSED:
	case FREE:
	default:
		Log_error(__func__, "Illegal event");
		break;
	}

	Log_exit(__func__, "");
}

static void Controller_State_In_Between_Free(void* controller,
		Controller_event_enum event) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;
	switch (event) {
	case OPENED:
		CB_notify(&obj->cbGreen);
		Timer_setTimer(obj->autoCloseTimer, &obj->cbAutoClose,
				AUTO_CLOSE_DOOR_TIMEOUT);
		obj->handler = Controller_State_Opened_Free;
		break;
	case CLOSED:
		CB_notify(&obj->cbOff);
		obj->handler = Controller_State_Closed_Free;
		break;
	case BLOCKED:
		CB_notify(&obj->cbYellowFlash);
		obj->handler = Controller_State_In_Between_Blocked;
		break;
	case FREE:
	case IN_BETWEEN:
	default:
		Log_error(__func__, "Illegal event");
		break;
	}

	Log_exit(__func__, "");
}

static void Controller_State_Opened_Free(void* controller,
		Controller_event_enum event) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;
	switch (event) {
	case IN_BETWEEN:
		CB_notify(&obj->cbYellow);
		obj->handler = Controller_State_In_Between_Free;
		break;
	case BLOCKED:
		CB_notify(&obj->cbYellowFlash);
		obj->handler = Controller_State_Opened_Blocked;
		break;
	case FREE:
	case OPENED:
	case CLOSED:
	default:
		Log_error(__func__, "Illegal event");
		break;
	}

	Log_exit(__func__, "");
}

static void Controller_State_Closed_Blocked(void* controller,
		Controller_event_enum event) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;
	switch (event) {
	case IN_BETWEEN:
		CB_notify(&obj->cbYellowFlash);
		obj->handler = Controller_State_In_Between_Blocked;
		break;
	case FREE:
		CB_notify(&obj->cbOff);
		obj->handler = Controller_State_Closed_Free;
		break;
	case OPENED:
	case CLOSED:
	case BLOCKED:
	default:
		Log_error(__func__, "Illegal event");
		break;
	}

	Log_exit(__func__, "");
}

static void Controller_State_In_Between_Blocked(void* controller,
		Controller_event_enum event) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;
	switch (event) {
	case OPENED:
		CB_notify(&obj->cbYellowFlash);
		obj->handler = Controller_State_Opened_Blocked;
		break;
	case CLOSED:
		CB_notify(&obj->cbYellowFlash);
		obj->handler = Controller_State_Closed_Blocked;
		break;
	case FREE:
		CB_notify(&obj->cbYellow);
		obj->handler = Controller_State_In_Between_Free;
		break;
	case IN_BETWEEN:
	case BLOCKED:
	default:
		Log_error(__func__, "Illegal event");
		break;
	}

	Log_exit(__func__, "");
}

static void Controller_State_Opened_Blocked(void* controller,
		Controller_event_enum event) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;
	switch (event) {
	case IN_BETWEEN:
		CB_notify(&obj->cbYellowFlash);
		obj->handler = Controller_State_In_Between_Blocked;
		break;
	case FREE:
		CB_notify(&obj->cbRed);
		obj->handler = Controller_State_Opened_Free;
		break;
	case BLOCKED:
	case OPENED:
	case CLOSED:
	default:
		Log_error(__func__, "Illegal event");
		break;
	}

	Log_exit(__func__, "");
}

static void Controller_autoClose(void* controller, void* context) {
	Log_entry(__func__, "");

	Controller_struct* obj = (Controller_struct*) controller;

	HSI_writePin(obj->doorRemote, 1);
	// Sleep?
	HSI_writePin(obj->doorRemote, 0);

	Log_exit(__func__, "");
}

