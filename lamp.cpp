/*
 * lamp.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#include <stdint.h>
#include <stdlib.h>

#include "log.h"
#include "hsi.h"
#include "timer.h"

#include "lamp.h"

typedef struct {
	HSI_dio_struct io;

	uint8_t state;
	uint16_t flash_period;
	Timer timer;

	CB_callbackClient flashOnCB;
	CB_callbackClient flashOffCB;
	CB_callbackClient oneShotCB;

} Lamp_struct;

static void Lamp_setState(Lamp_struct* obj, uint8_t state);
static void Lamp_flashOnCB(void*, void*);
static void Lamp_flashOffCB(void*, void*);
static void Lamp_oneShotCB(void*, void*);

Lamp Lamp_construct(HSI_dio_struct dio) {
	Log_entry(PSTR("Lamp_construct"));

	Lamp_struct* result = (Lamp_struct*) calloc(1, sizeof(Lamp_struct));

	result->io = dio;
	result->timer = Timer_construct();

	result->flashOffCB.callback = Lamp_flashOffCB;
	result->flashOffCB.obj = result;
	result->flashOffCB.context = NULL;

	result->flashOnCB.callback = Lamp_flashOnCB;
	result->flashOnCB.obj = result;
	result->flashOnCB.context = NULL;

	result->flashOffCB.callback = Lamp_oneShotCB;
	result->flashOffCB.obj = result;
	result->flashOffCB.context = NULL;

	Log_exit(PSTR("Lamp_construct"));
	return result;
}

void Lamp_on(Lamp lamp) {
	Log_entry(PSTR("Lamp_on"));

	Lamp_struct* obj = (Lamp_struct*) lamp;
	Lamp_setState(obj, 1);

	Log_exit(PSTR("Lamp_on"));
}

void Lamp_off(Lamp lamp) {
	Log_entry(PSTR("Lamp_off"));

	Lamp_struct* obj = (Lamp_struct*) lamp;
	Lamp_setState(obj, 0);

	Log_exit(PSTR("Lamp_off"));
}

void Lamp_flash(Lamp lamp, uint16_t period) {
	Log_entry(PSTR("Lamp_flash"));

	Lamp_struct* obj = (Lamp_struct*) lamp;
	obj->flash_period = period;
	Lamp_flashOnCB(obj, NULL);

	Log_exit(PSTR("Lamp_flash"));
}

void Lamp_oneShot(Lamp lamp, uint32_t duration) {
	Log_entry(PSTR("Lamp_oneShot"));

	Lamp_struct* obj = (Lamp_struct*) lamp;
	Lamp_setState(obj, 1);
	Timer_setTimer(obj->timer, &obj->oneShotCB, duration);

	Log_exit(PSTR("Lamp_oneShot"));
}

static void Lamp_setState(Lamp_struct* obj, uint8_t state) {
	Log_entry(PSTR("Lamp_setState"));

	HSI_writePin(obj->io, state);
	obj->state = state;
	Timer_cancelTimer(obj->timer);

	Log_exit(PSTR("Lamp_setState"));
}

static void Lamp_flashOnCB(void* lamp, void* context) {
	Log_entry(PSTR("Lamp_flashOnCB"));

	Lamp_struct* obj = (Lamp_struct*) lamp;

	Lamp_setState(obj, 1);
	Timer_setTimer(obj->timer, &obj->flashOffCB, obj->flash_period);

	Log_exit(PSTR("Lamp_flashOnCB"));
}

static void Lamp_flashOffCB(void* lamp, void* context) {
	Log_entry(PSTR("Lamp_flashOffCB"));

	Lamp_struct* obj = (Lamp_struct*) lamp;

	Lamp_setState(obj, 0);
	Timer_setTimer(obj->timer, &obj->flashOnCB, obj->flash_period);

	Log_exit(PSTR("Lamp_flashOffCB"));
}

static void Lamp_oneShotCB(void* lamp, void* context) {
	Log_entry(PSTR("Lamp_oneShotCB"));

	Lamp_struct* obj = (Lamp_struct*) lamp;
	Lamp_setState(obj, 0);
	Timer_cancelTimer(obj->timer);

	Log_exit(PSTR("Lamp_oneShotCB"));
}
