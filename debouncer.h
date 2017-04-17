/*
 * debouncer.h
 *
 *  Created on: 14 apr. 2017
 *      Author: wilbert
 */

#ifndef DEBOUNCER_H_
#define DEBOUNCER_H_

#include <stdint.h>
#include "callback.h"
#include "hsi.h"

typedef void (*DebouncerChangedHighCB)(void* context);
typedef void (*DebouncerChangedLowCB)(void* context);

typedef void* Debouncer;

Debouncer Debouncer_construct(const CB_callbackClient* whenHigh, const CB_callbackClient* whenLow, HSI_dio_struct io);
void Debouncer_sample(Debouncer debouncer);

void Debouncer_sampleAllDebouncers();


#endif /* DEBOUNCER_H_ */
