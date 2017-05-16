/*
 * main.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifdef SIMULATOR
#include <unistd.h>
#endif

#ifndef SIMULATOR
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

#include "log.h"
#include "garage.h"
#include "debouncer.h"
#include "timer.h"
#include "simulator.h"

static void initIO(Garage_io_struct* io);
#ifndef SIMULATOR
static void installTimer();
#endif

int main(int argc, char* argv[]) {
	Garage_io_struct io;

	initIO(&io);

	Garage_construct(&io);

#ifndef SIMULATOR

	installTimer();
	while (1);

#else

	SIM_initialize(&io);
	while (1) {
		Debouncer_sampleAllDebouncers();
		Timer_tickAllTimers();
		SIM_advanceTime();

		usleep(1000);
	}

#endif
}

#ifndef SIMULATOR
static void installTimer()
{

	OCR1A = 1250; // prescaler 64, clockfreq 8 Mhz, compare 1250 implies delay 10ms.
	TIMSK1 |= _BV(OCIE1A); // enable interrupt on compare match
	TCCR1B |= _BV(WGM12); // set clear counter on timer compare

	TCCR1B |= (_BV(CS11) | _BV(CS10)); // Start timer (by setting prescaler)
}

ISR(TIMER1_COMPA_vect)
{
	Debouncer_sampleAllDebouncers();
	Timer_tickAllTimers();
}

#endif


static void initIO(Garage_io_struct* io) {
#ifndef SIMULATOR
	  io->ambientLight.port = &PORTC;
	  io->ambientLight.pin = 3;

	  io->leftOpenedSensor.port = &PORTD;
	  io->leftOpenedSensor.pin = 3;

	  io->leftClosedSensor.port = &PORTD;
	  io->leftClosedSensor.pin = 2;

	  io->leftGateSensor.port = &PORTD;
	  io->leftGateSensor.pin = 4;

	  io->leftRemote.port = &PORTC;
	  io->leftRemote.pin = 4;

	  io->rightOpenedSensor.port = &PORTD;
	  io->rightOpenedSensor.pin = 6;

	  io->rightClosedSensor.port = &PORTD;
	  io->rightClosedSensor.pin = 5;

	  io->rightGateSensor.port = &PORTD;
	  io->rightGateSensor.pin = 7;

	  io->rightRemote.port = &PORTC;
	  io->rightRemote.pin = 5;

	  io->trafficRed.port = &PORTC;
	  io->trafficRed.pin = 0;

	  io->trafficYellow.port = &PORTC;
	  io->trafficYellow.pin = 1;

	  io->trafficGreen.port = &PORTC;
	  io->trafficGreen.pin = 2;

	  // All outputs are connected to port C
	  DDRC = 0b00111111;

#else
	io->ambientLight.port = NULL;
	io->ambientLight.pin = 0;

	io->leftOpenedSensor.port = NULL;
	io->leftOpenedSensor.pin = 1;

	io->leftClosedSensor.port = NULL;
	io->leftClosedSensor.pin = 2;


	io->leftGateSensor.port = NULL;
	io->leftGateSensor.pin = 3;

	io->leftRemote.port = NULL;
	io->leftRemote.pin = 4;

	io->rightOpenedSensor.port = NULL;
	io->rightOpenedSensor.pin = 5;

	io->rightClosedSensor.port = NULL;
	io->rightClosedSensor.pin = 6;

	io->rightGateSensor.port = NULL;
	io->rightGateSensor.pin = 7;

	io->rightRemote.port = NULL;
	io->rightRemote.pin = 8;

	io->trafficRed.port = NULL;
	io->trafficRed.pin = 9;

	io->trafficYellow.port = NULL;
	io->trafficYellow.pin = 10;

	io->trafficGreen.port = NULL;
	io->trafficGreen.pin = 11;
#endif

}
