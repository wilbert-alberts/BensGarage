/*
 * lamp.h
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifndef LAMP_H_
#define LAMP_H_


#include <stdint.h>
#include "hsi.h"

typedef void* Lamp;

Lamp Lamp_construct(HSI_dio_struct dio);
void Lamp_on(Lamp lamp);
void Lamp_off(Lamp lamp);
void Lamp_flash(Lamp lamp, uint16_t period);
void Lamp_oneShot(Lamp lamp, uint32_t duration);

#endif /* LAMP_H_ */
