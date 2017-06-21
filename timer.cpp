/*
 * timer.c
 *
 *  Created on: 14 apr. 2017
 *      Author: wilbert
 */

#include <stdio.h>

#include "log.h"
#include "timer.h"
#include "callback.h"

#define TIMER_NRTIMERS (10)

typedef struct
{
  const char* id;
  
	uint32_t remaining;

	CB_callbackClient callback;
} Timer_struct;

static Timer_struct timer_Timers[TIMER_NRTIMERS];
static int timer_nrTimers = 0;

Timer Timer_construct(const char* id) {
	if (timer_nrTimers < TIMER_NRTIMERS) {

		Timer_struct* result = &timer_Timers[timer_nrTimers++];

    result->id = id;
		result->remaining = 0;
		CB_unregister(&result->callback);

		return (Timer)result;
	} else {
		Log_error_PP(PSTR("Timer_construct"), PSTR("Not enough timers"));
		return NULL;
	}
}

void Timer_setTimer(Timer timer, const CB_callbackClient* cb, uint32_t delta) {
	Timer_struct* obj = (Timer_struct*)timer;
	obj->remaining = delta;
	CB_register(&obj->callback, cb->callback, cb->obj, cb->context);
}

void Timer_cancelTimer(Timer timer) {
	Timer_struct* obj = (Timer_struct*)timer;
	CB_unregister(&obj->callback);
}

void Timer_tick(Timer timer) {
	Timer_struct* obj = (Timer_struct*)timer;
  Log_P(PSTR("Ticking: "));
  Log(obj->id);
	if (CB_isRegistered(&obj->callback)) {
		obj->remaining--;
  Log_P(PSTR(": "));
    LogInt((int)obj->remaining);
		if (obj->remaining == 0) {
      Log_P(PSTR(" - ALARM"));
      Logln(".");
			CB_callbackClient cb = obj->callback;
			CB_unregister(&obj->callback);

			CB_notify(&cb);
		}
   else {
    Logln(".");
   }
	}
}

void Timer_tickAllTimers()
{
	int i;
  Log(".");
  
	for (i=0; i<timer_nrTimers; i++) {
		Timer_tick(&timer_Timers[i]);
	}

 
}
