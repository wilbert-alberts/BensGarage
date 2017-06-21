/*
 * debouncer.c
 *
 *  Created on: 14 apr. 2017
 *      Author: wilbert
 */

#include <stdint.h>
#include <stdio.h>

#include "log.h"
#include "hsi.h"
#include "debouncer.h"
#include "callback.h"

#define DEBOUNCER_STABLEDURATION (10)
#define DEBOUNCER_NRDEBOUNCERS (6)

typedef struct {
	HSI_dio_struct io;
	uint8_t remaining;
	uint8_t value;
	CB_callbackClient whenHighCB;
	CB_callbackClient whenLowCB;
} Debouncer_struct;

static Debouncer_struct debouncer_debouncers[DEBOUNCER_NRDEBOUNCERS];
static int debouncer_nrDebouncers = 0;

Debouncer Debouncer_construct(const CB_callbackClient* whenHigh,
		const CB_callbackClient* whenLow, HSI_dio_struct io) {
	Debouncer_struct* result;

	if (debouncer_nrDebouncers < DEBOUNCER_NRDEBOUNCERS) {
		result = &debouncer_debouncers[debouncer_nrDebouncers++];

		result->io = io;
		CB_register(&result->whenHighCB, whenHigh->callback, whenHigh->obj,
				whenHigh->context);
		CB_register(&result->whenLowCB, whenLow->callback, whenLow->obj,
				whenLow->context);
		result->value = HSI_readPin(io);
		result->remaining = DEBOUNCER_STABLEDURATION;

		return (Debouncer) result;
	} else {
		Log_error_PP(PSTR("Debouncer_construct"), PSTR("Not enough debouncers"));
		return NULL;
	}
}

void Debouncer_sample(Debouncer debouncer) {
	Debouncer_struct* obj = (Debouncer_struct*) debouncer;

	uint8_t v = HSI_readPin(obj->io);

	//printf("Sampling debouncer %d, current value: %d, read: %d, remaining: %d\n", obj->io.pin, obj->value, v, obj->remaining);

	if (v != obj->value) {
		obj->remaining--;
		if (obj->remaining == 0) {
			if (v == 0) {
        Log_P(PSTR("Debouncer "));
        LogPort(&obj->io);
        Logln_P(PSTR(" gone low"));
				CB_notify(&obj->whenLowCB);
			}
			if (v == 1) {
        Log_P(PSTR("Debouncer "));
        LogPort(&obj->io);
        Logln_P(PSTR(" gone high"));
				CB_notify(&obj->whenHighCB);
			}
			obj->value = v;
		}
	} else {
		obj->remaining = DEBOUNCER_STABLEDURATION;
	}
}

void Debouncer_sampleAllDebouncers() {
	int i=0;
	for (i = 0; i < debouncer_nrDebouncers; i++) {
		Debouncer_sample(&debouncer_debouncers[i]);
	}
}

