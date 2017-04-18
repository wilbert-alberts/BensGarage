/*
 * hsi.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */


#include "hsi.h"

#ifndef __AVR__
#include "simulator.h"
#endif

#include <stdint.h>

#ifdef __AVR__
#include <avr/io.h>
#endif

uint8_t HSI_readPin(HSI_dio_struct io)
{
	uint8_t result;
#ifdef __AVR__
	result = *io.port & _BV(io.pin);

	if (result != 0)
		result = 1;
#else
	result = SIM_readPin(io);
#endif
	return result;
}

void HSI_writePin(HSI_dio_struct io, uint8_t value)
{
#ifdef __AVR__
	if (value == 0) {
		*io.port &= ((*io.port) & (~_BV(io.pin)));
	}
	else {
		*io.port |= ((*io.port) | _BV(io.pin));
	}
#else
	SIM_writePin(io, value);
#endif
}

