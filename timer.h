/*
 * timer.h
 *
 *  Created on: 14 apr. 2017
 *      Author: wilbert
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

#include "callback.h"


#define TIMER_PERIOD      (10)
#define TIMER_MINUTES(m)  ((m)*60*1000/TIMER_PERIOD)
#define TIMER_SECONDS(s)  ((s)*1000/TIMER_PERIOD)
#define TIMER_MS(ms)      ((uint32_t)(((double)(ms))/(double)(TIMER)))

typedef void* Timer;

Timer Timer_construct();
void Timer_setTimer(Timer timer, const CB_callbackClient* cb, uint32_t delta);
void Timer_cancelTimer(Timer timer);
void Timer_tick(Timer timer);


void Timer_tickAllTimers();

#endif /* TIMER_H_ */
