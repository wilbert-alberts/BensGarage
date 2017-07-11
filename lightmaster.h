/*
 * lightmaster.h
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifndef LIGHTMASTER_H_
#define LIGHTMASTER_H_

#include "hsi.h"
#include "timer.h"

typedef void* LightMaster;

#define LM_AMBIENT_OFF_TIMER TIMER_MINUTES(5L) //  5 minutes

typedef enum
{
	LM_LEFT,
	LM_RIGHT,
	LM_NONE
} LM_side_enum;

typedef enum
{
	LM_OFF,
	LM_GREEN,
	LM_YELLOW,
	LM_YELLOW_FLASH,
	LM_RED
} LM_trafficLightState;


LightMaster LM_construct(HSI_dio_struct red, HSI_dio_struct yellow, HSI_dio_struct green, HSI_dio_struct ambient, HSI_dio_struct powerledGreen, HSI_dio_struct powerledRed);
void LM_setLight(LightMaster lm, LM_side_enum side, LM_trafficLightState state);


#endif /* LIGHTMASTER_H_ */
