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

typedef enum {
	IN_BETWEEN, OPENED, CLOSED, FREE, BLOCKED
} Controller_event_enum;

typedef void (*Controller_stateHandler)(void* obj, Controller_event_enum event);

typedef struct {
  char id[2];
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
static void Controller_State_Opening_Free(void* controller,
    Controller_event_enum event);
static void Controller_State_Closing_Free(void* controller,
    Controller_event_enum event);
static void Controller_State_Opened_Free(void* controller,
		Controller_event_enum event);
static void Controller_State_Closed_Blocked(void* controller,
		Controller_event_enum event);
static void Controller_State_Opening_Blocked(void* controller,
    Controller_event_enum event);
static void Controller_State_Closing_Blocked(void* controller,
    Controller_event_enum event);
static void Controller_State_Opened_Blocked(void* controller,
		Controller_event_enum event);

Controller Controller_construct(char id, HSI_dio_struct openedSensor,
		HSI_dio_struct closedSensor, HSI_dio_struct gateSensor,
		HSI_dio_struct remote, CB_callbackClient cbRed,
		CB_callbackClient cbYellow, CB_callbackClient cbYellowFlash,
		CB_callbackClient cbGreen, CB_callbackClient cbOff) {
	Log_entry(PSTR("Controller_construct"));

	Controller_struct* result = calloc(1, sizeof(Controller_struct));

  result->id[0] = id;
  result->id[1]= 0;
	CB_callbackClient openedCB = { result, NULL, Controller_doorOpened};
	CB_callbackClient inbetweenCB = { result, NULL, Controller_doorInBetween};
	CB_callbackClient closedCB = { result, NULL, Controller_doorClosed };
	result->door = Door_construct(id, &openedCB, &inbetweenCB, &closedCB,
			openedSensor, closedSensor);

	CB_callbackClient gateHighCB = { result, NULL, Controller_gateBlocked };
	CB_callbackClient gateLowCB = { result, NULL, Controller_gateFree };
	result->gate = Debouncer_construct(&gateHighCB, &gateLowCB, gateSensor);

	result->cbRed = cbRed;
	result->cbYellow = cbYellow;
	result->cbGreen = cbGreen;
	result->cbOff = cbOff;

  // Set the initial state: 
	result->handler = Controller_State_Closed_Free;

	result->autoCloseTimer = Timer_construct();
	CB_callbackClient cbAutoClose = { result, Controller_autoClose, NULL };
	result->cbAutoClose = cbAutoClose;
	result->doorRemote = remote;

	Log_exit(PSTR("Controller_construct"));
	return result;
}

/* Event dispatch functions 
 * 
 * They dispatch the event to the correct controller.
 */
static void Controller_doorOpened(void* controller, void* context) {
	Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_doorOpened"));
  Log(PSTR(" id: ")); Logln(obj->id);

	obj->handler(obj, OPENED);

	Log_exit(PSTR("Controller_doorOpened"));
}

static void Controller_doorInBetween(void* controller, void* context) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_doorInBetween"));
  Log(PSTR(" id: ")); Logln(obj->id);

	obj->handler(obj, IN_BETWEEN);

	Log_exit(PSTR("Controller_doorInBetween"));
}

static void Controller_doorClosed(void* controller, void* context) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_doorClosed"));
  Log(PSTR(" id: ")); Logln(obj->id);

	obj->handler(obj, CLOSED);

	Log_exit(PSTR("Controller_doorClosed"));
}

static void Controller_gateFree(void* controller, void* context) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_gateFree"));
  Log(PSTR(" id: ")); Logln(obj->id);

	obj->handler(obj, FREE);

	Log_exit(PSTR("Controller_gateFree"));
}

static void Controller_gateBlocked(void* controller, void* context) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_gateBlocked"));
  Log(PSTR(" id: ")); Logln(obj->id);

	obj->handler(obj, BLOCKED);

	Log_exit(PSTR("Controller_gateBlocked"));
}

/* State handlers */

static void Controller_State_Closed_Free(void* controller,
		Controller_event_enum event) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_State_Closed_Free"));
  Log(PSTR(" id: ")); Logln(obj->id);

	switch (event) {
	case IN_BETWEEN:
    Logln(PSTR("Event: in between"));
		CB_notify(&obj->cbGreen);    
		obj->handler = Controller_State_Opening_Free;
		break;
	case BLOCKED:
    Logln(PSTR("Event: blocked"));
		CB_notify(&obj->cbYellowFlash);
		obj->handler = Controller_State_Closed_Blocked;
		break;
	case OPENED:
	case CLOSED:
	case FREE:
	default:
		Log_error(PSTR("Controller_State_Closed_Free"), PSTR("Illegal event"));
		break;
	}

	Log_exit(PSTR("Controller_State_Closed_Free"));
}

static void Controller_State_Opening_Free(void* controller,
		Controller_event_enum event) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_State_In_Between_Free"));
  Log(PSTR(" id: ")); Logln(obj->id);

	switch (event) {
	case OPENED:
    Logln(PSTR("Event: opened"));
		Timer_setTimer(obj->autoCloseTimer, &obj->cbAutoClose,
				AUTO_CLOSE_DOOR_TIMEOUT);
		obj->handler = Controller_State_Opened_Free;
		break;
	case CLOSED:
    Logln(PSTR("Event: closed"));
		CB_notify(&obj->cbOff);
		obj->handler = Controller_State_Closed_Free;
		break;
	case BLOCKED:
    Logln(PSTR("Event: blocked"));
		CB_notify(&obj->cbYellow);
		obj->handler = Controller_State_Opening_Blocked;
		break;
	case FREE:
	case IN_BETWEEN:
	default:
		Log_error(PSTR("Controller_State_In_Between_Free"), PSTR("Illegal event"));
		break;
	}

	Log_exit(PSTR("Controller_State_In_Between_Free"));
}

static void Controller_State_Opening_Blocked(void* controller,
    Controller_event_enum event) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_State_In_Between_Free"));
  Log(PSTR(" id: ")); Logln(obj->id);

  switch (event) {
  case OPENED:
    Logln(PSTR("Event: opened"));
    obj->handler = Controller_State_Opened_Blocked;
    break;
  case CLOSED:
    Logln(PSTR("Event: closed"));
    CB_notify(&obj->cbYellowFlash);
    obj->handler = Controller_State_Closed_Blocked;
    break;
  case FREE:
    Logln(PSTR("Event: free"));
    CB_notify(&obj->cbRed);
    obj->handler = Controller_State_Opening_Free;
    break;
  case BLOCKED:
  case IN_BETWEEN:
  default:
    Log_error(PSTR("Controller_State_In_Between_Free"), PSTR("Illegal event"));
    break;
  }

  Log_exit(PSTR("Controller_State_In_Between_Free"));
}



static void Controller_State_Opened_Free(void* controller,
		Controller_event_enum event) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_State_Opened_Free"));
  Log(PSTR(" id: ")); Logln(obj->id);

	switch (event) {
	case IN_BETWEEN:
    Logln(PSTR("Event: in between"));
		CB_notify(&obj->cbYellow);
    Timer_cancelTimer(obj->autoCloseTimer);
		obj->handler = Controller_State_Closing_Free;
		break;
	case BLOCKED:
    Logln(PSTR("Event: blocked"));
		CB_notify(&obj->cbYellow);
    Timer_cancelTimer(obj->autoCloseTimer);
		obj->handler = Controller_State_Opened_Blocked;
		break;
	case FREE:
	case OPENED:
	case CLOSED:
	default:
		Log_error(PSTR("Controller_State_Opened_Free"), PSTR("Illegal event"));
		break;
	}

	Log_exit(PSTR("Controller_State_Opened_Free"));
}

static void Controller_State_Closed_Blocked(void* controller,
		Controller_event_enum event) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_State_Closed_Blocked"));
  Log(PSTR(" id: ")); Logln(obj->id);

	switch (event) {
	case IN_BETWEEN:
    Logln(PSTR("Event: in between"));
		obj->handler = Controller_State_Opening_Blocked;
		break;
	case FREE:
    Logln(PSTR("Event: free"));
		CB_notify(&obj->cbOff);
		obj->handler = Controller_State_Closed_Free;
		break;
	case OPENED:
	case CLOSED:
	case BLOCKED:
	default:
		Log_error(PSTR("Controller_State_Closed_Blocked"), PSTR("Illegal event"));
		break;
	}

	Log_exit(PSTR("Controller_State_Closed_Blocked"));
}

static void Controller_State_Closing_Blocked(void* controller,
		Controller_event_enum event) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_State_In_Between_Blocked"));
  Log(PSTR(" id: ")); Logln(obj->id);

	switch (event) {
	case OPENED:
    Logln(PSTR("Event: opened"));
		obj->handler = Controller_State_Opened_Blocked;
		break;
	case CLOSED:
    Logln(PSTR("Event: closed"));
		CB_notify(&obj->cbYellowFlash);
		obj->handler = Controller_State_Closed_Blocked;
		break;
	case FREE:
    Logln(PSTR("Event: free"));
		CB_notify(&obj->cbOff);
		obj->handler = Controller_State_Closing_Free;
		break;
	case IN_BETWEEN:
	case BLOCKED:
	default:
		Log_error(PSTR("Controller_State_In_Between_Blocked"), PSTR("Illegal event"));
		break;
	}

	Log_exit(PSTR("Controller_State_In_Between_Blocked"));
}

static void Controller_State_Closing_Free(void* controller,
    Controller_event_enum event) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_State_In_Between_Free"));
  Log(PSTR(" id: ")); Logln(obj->id);

  switch (event) {
  case OPENED:
    Logln(PSTR("Event: opened"));
    obj->handler = Controller_State_Opened_Free;
    break;
  case CLOSED:
    Logln(PSTR("Event: closed"));
    CB_notify(&obj->cbOff);
    obj->handler = Controller_State_Closed_Free;
    break;
  case BLOCKED:
    Logln(PSTR("Event: free"));
    CB_notify(&obj->cbYellowFlash);
    obj->handler = Controller_State_Closing_Blocked;
    break;
  case FREE:
  case IN_BETWEEN:
  default:
    Log_error(PSTR("Controller_State_In_Between_Free"), PSTR("Illegal event"));
    break;
  }

  Log_exit(PSTR("Controller_State_In_Between_Free"));
}


static void Controller_State_Opened_Blocked(void* controller,
		Controller_event_enum event) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_State_Opened_Blocked"));
  Log(PSTR(" id: ")); Logln(obj->id);

	switch (event) {
	case IN_BETWEEN:
    Logln(PSTR("Event: in between"));
		CB_notify(&obj->cbYellowFlash);
		obj->handler = Controller_State_Closing_Blocked;
		break;
	case FREE:
    Logln(PSTR("State: free"));
		CB_notify(&obj->cbRed);
		obj->handler = Controller_State_Opened_Free;
		break;
	case BLOCKED:
	case OPENED:
	case CLOSED:
	default:
		Log_error(PSTR("Controller_State_Opened_Blocked"), PSTR("Illegal event"));
		break;
	}

	Log_exit(PSTR("Controller_State_Opened_Blocked"));
}

static void Controller_autoClose(void* controller, void* context) {
  Controller_struct* obj = (Controller_struct*) controller;
  Log_entry(PSTR("Controller_autoClose"));
  Log(PSTR(" id: ")); Logln(obj->id);


	HSI_writePin(obj->doorRemote, 1);
	// TODO: Sleep?
	HSI_writePin(obj->doorRemote, 0);

	Log_exit(PSTR("Controller_autoClose"));
}

