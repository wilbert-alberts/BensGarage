/*
 * trafficlight.h
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifndef TRAFFICLIGHT_H_
#define TRAFFICLIGHT_H_

#include "hsi.h"

typedef void* TrafficLight;

TrafficLight TL_construct(HSI_dio_struct red, HSI_dio_struct yellow, HSI_dio_struct green);
void TL_red(TrafficLight tl);
void TL_yellow(TrafficLight tl);
void TL_yellowFlash(TrafficLight tl);
void TL_green(TrafficLight tl);
void TL_off(TrafficLight tl);

#endif /* TRAFFICLIGHT_H_ */
