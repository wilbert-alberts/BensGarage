/*
 ============================================================================
 Name        : GarageBen.c
 Author      : Wilbert Alberts and Ben Slaghekke
 Version     :
 Copyright   : (c) by us
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "hsi.h"
#include "controller.h"
#include "lightmaster.h"
#include "callback.h"

#include "garage.h"

#define GARAGE_LEFT ((void*)1)
#define GARAGE_RIGHT ((void*)2)

typedef struct {
	Controller left;
	Controller right;
	LightMaster lm;
} Garage_struct;

static void Garage_Red(void* obj, void* context);
static void Garage_Yellow(void* obj, void* context);
static void Garage_YellowFlash(void* obj, void* context);
static void Garage_Green(void* obj, void* context);
static void Garage_Off(void* obj, void* context);

Garage* Garage_construct(Garage_io_struct* io) {
	Log_entry(__func__, "");

	Garage_struct* result = (Garage_struct*) calloc(1, sizeof(Garage_struct));
	CB_callbackClient cbRed = { result, GARAGE_LEFT, Garage_Red };
	CB_callbackClient cbYellow = { result, GARAGE_LEFT, Garage_Yellow };
	CB_callbackClient cbYellowFlash =
			{ result, GARAGE_LEFT, Garage_YellowFlash };
	CB_callbackClient cbGreen = { result, GARAGE_LEFT, Garage_Green };
	CB_callbackClient cbOff = { result, GARAGE_LEFT, Garage_Off };

	result->left = Controller_construct(io->leftOpenedSensor,
			io->leftClosedSensor, io->leftGateSensor, io->leftRemote, cbRed,
			cbYellow, cbYellowFlash, cbGreen, cbOff);

	cbRed.context = GARAGE_RIGHT;
	cbYellow.context = GARAGE_RIGHT;
	cbYellowFlash.context = GARAGE_RIGHT;
	cbGreen.context = GARAGE_RIGHT;
	cbOff.context = GARAGE_RIGHT;

	result->right = Controller_construct(io->rightOpenedSensor,
			io->rightClosedSensor, io->rightGateSensor, io->rightRemote, cbRed,
			cbYellow, cbYellowFlash, cbGreen, cbOff);

	result->lm = LM_construct(io->trafficRed, io->trafficYellow,
			io->trafficGreen, io->ambientLight);

	Log_exit(__func__, "");
	return (Garage*) result;
}

static void Garage_Red(void* obj, void* context) {
	Log_entry(__func__, "");

	Garage_struct* g = (Garage_struct*) obj;
	if (context == GARAGE_LEFT) {
		LM_setLight(g->lm, LM_LEFT, LM_RED);
	} else {
		if (context == GARAGE_RIGHT) {
			LM_setLight(g->lm, LM_RIGHT, LM_RED);
		} else {
			Log_error(__func__,
					"Illegal context, should be GARAGE_LEFT or GARAGE_RIGHT");
		}
	}
	Log_exit(__func__, "");
}

static void Garage_Yellow(void* obj, void* context) {
	Log_entry(__func__, "");

	Garage_struct* g = (Garage_struct*) obj;
	if (context == GARAGE_LEFT) {
		LM_setLight(g->lm, LM_LEFT, LM_YELLOW);
	} else {
		if (context == GARAGE_RIGHT) {
			LM_setLight(g->lm, LM_RIGHT, LM_YELLOW);
		} else {
			Log_error(__func__,
					"Illegal context, should be GARAGE_LEFT or GARAGE_RIGHT");
		}
	}
	Log_exit(__func__, "");
}

static void Garage_YellowFlash(void* obj, void* context) {
	Log_entry(__func__, "");

	Garage_struct* g = (Garage_struct*) obj;
	if (context == GARAGE_LEFT) {
		LM_setLight(g->lm, LM_LEFT, LM_YELLOW_FLASH);
	} else {
		if (context == GARAGE_RIGHT) {
			LM_setLight(g->lm, LM_RIGHT, LM_YELLOW_FLASH);
		} else {
			Log_error(__func__,
					"Illegal context, should be GARAGE_LEFT or GARAGE_RIGHT");
		}
	}

	Log_exit(__func__, "");
}

static void Garage_Green(void* obj, void* context) {
	Log_entry(__func__, "");

	Garage_struct* g = (Garage_struct*) obj;
	if (context == GARAGE_LEFT) {
		LM_setLight(g->lm, LM_LEFT, LM_GREEN);
	} else {
		if (context == GARAGE_RIGHT) {
			LM_setLight(g->lm, LM_RIGHT, LM_GREEN);
		} else {
			Log_error(__func__,
					"Illegal context, should be GARAGE_LEFT or GARAGE_RIGHT");
		}
	}

	Log_exit(__func__, "");
}

static void Garage_Off(void* obj, void* context) {
	Log_entry(__func__, "");

	Garage_struct* g = (Garage_struct*) obj;
	if (context == GARAGE_LEFT) {
		LM_setLight(g->lm, LM_LEFT, LM_OFF);
	} else {
		if (context == GARAGE_RIGHT) {
			LM_setLight(g->lm, LM_RIGHT, LM_OFF);
		} else {
			Log_error(__func__,
					"Illegal context, should be GARAGE_LEFT or GARAGE_RIGHT");
		}
	}

	Log_exit(__func__, "");
}
