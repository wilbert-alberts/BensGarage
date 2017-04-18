/*
 * main.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#include <unistd.h>

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
static void installTimer();


int main(int argc, char* argv[]) {
	Garage_io_struct io;
	Garage garage;

	initIO(&io);

	garage = Garage_construct(&io);

#ifndef SIMULATOR

	installTimer();
	while (1);

#else

	SIM_initialize(&io);
	while (1) {
		Log("tick");
		SIM_advanceTime();
		Debouncer_sampleAllDebouncers();
		Timer_tickAllTimers();

		usleep(1000000);
	}

#endif
}

#ifndef SIMULATOR
static void installTimer()
{

	OCR1A = 125; // prescaler 64, clockfreq 8 Mhz, compare 125 implies delay 1ms.
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
	io->ambientLight.port = &PORTB;
	io->ambientLight.pin = 1;

	io->leftOpenedSensor.port = &PORTB;
	io->leftOpenedSensor.pin = 2;

	io->leftClosedSensor.port = &PORTB;
	io->leftClosedSensor.pin = 3;

	io->leftGateSensor.port = &PORTB;
	io->leftGateSensor.pin = 4;

	io->leftRemote.port = &PORTB;
	io->leftRemote.pin = 5;

	io->rightOpenedSensor.port = &PORTB;
	io->rightOpenedSensor.pin = 6;

	io->rightClosedSensor.port = &PORTB;
	io->rightClosedSensor.pin = 7;

	io->rightGateSensor.port = &PORTB;
	io->rightGateSensor.pin = 8;

	io->rightRemote.port = &PORTB;
	io->rightRemote.pin = 9;

	io->trafficRed.port = &PORTB;
	io->trafficRed.pin = 10;

	io->trafficYellow.port = &PORTB;
	io->trafficYellow.pin = 11;

	io->trafficGreen.port = &PORTB;
	io->trafficGreen.pin = 12;
#else
	io->ambientLight.port = NULL;
	io->ambientLight.pin = 1;

	io->leftOpenedSensor.port = NULL;
	io->leftOpenedSensor.pin = 2;

	io->leftClosedSensor.port = NULL;
	io->leftClosedSensor.pin = 3;

	io->leftGateSensor.port = NULL;
	io->leftGateSensor.pin = 4;

	io->leftRemote.port = NULL;
	io->leftRemote.pin = 5;

	io->rightOpenedSensor.port = NULL;
	io->rightOpenedSensor.pin = 6;

	io->rightClosedSensor.port = NULL;
	io->rightClosedSensor.pin = 7;

	io->rightGateSensor.port = NULL;
	io->rightGateSensor.pin = 8;

	io->rightRemote.port = NULL;
	io->rightRemote.pin = 9;

	io->trafficRed.port = NULL;
	io->trafficRed.pin = 10;

	io->trafficYellow.port = NULL;
	io->trafficYellow.pin = 11;

	io->trafficGreen.port = NULL;
	io->trafficGreen.pin = 12;
#endif

}
