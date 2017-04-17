/*
 * hsi.h
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifndef HSI_H_
#define HSI_H_

#include <stdint.h>

#define PORTTYPE volatile uint8_t *
#define PINTYPE	 uint8_t

typedef struct
{
	PORTTYPE port;
	PINTYPE  pin;
} HSI_dio_struct;

uint8_t HSI_readPin(HSI_dio_struct);
void HSI_writePin(HSI_dio_struct, uint8_t value);


#endif /* HSI_H_ */
