/*
 * hsi.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */


#include "hsi.h"
#include "log.h"

#include <stdint.h>

#include <avr/io.h>
#include <HardwareSerial.h>

uint8_t HSI_readPin(HSI_dio_struct io)
{
	volatile uint8_t result=37;
	result = *io.port & _BV(io.pin);

/*
  Logln(PSTR("ReadPin "));
  LogPort(&io);
  Serial.print("\n");
  Serial.println(result);
*/  
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

void HSI_toString(HSI_dio_struct* io, char buffer[6])
{
  buffer[0] = '(';
  buffer[1] = 'x';
//  if (io->port == &PORTA)
//    buffer[1] = 'A';
  if (io->port == &PORTB)
    buffer[1] = 'B';
  if (io->port == &PORTC)
    buffer[1] = 'C';
  if (io->port == &PORTD)
    buffer[1] = 'D';
//  if (io->port == &PORTE)
//    buffer[1] = 'E';
//  if (io->port == &PORTF)
//    buffer[1] = 'F';
  buffer[2] = ',';
  buffer[3] = ' ';
  buffer[4] = '0' + io->pin;
  buffer[5] = ')';
  
}

