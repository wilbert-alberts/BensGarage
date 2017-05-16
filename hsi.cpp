/*
 * hsi.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */


#include "hsi.h"


#include <stdint.h>

#include <avr/io.h>

uint8_t HSI_readPin(HSI_dio_struct io)
{
	uint8_t result;
	result = *io.port & _BV(io.pin);

	if (result != 0)
		result = 1;
	return result;
}

void HSI_writePin(HSI_dio_struct io, uint8_t value)
{
	if (value == 0) {
		*io.port &= ((*io.port) & (~_BV(io.pin)));
	}
	else {
		*io.port |= ((*io.port) | _BV(io.pin));
	}
}

