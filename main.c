/*
 * main.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#include <unistd.h>

#ifdef __AVR__
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
	SIM_initialize(&io);

	garage = Garage_construct(&io);

	installTimer();

#ifdef __AVR__
	while (1);


#else
	while (1) {
		Log("tick");
		SIM_advanceTime();
		Debouncer_sampleAllDebouncers();
		Timer_tickAllTimers();

		usleep(1000000);
	}
#endif
}

static void installTimer()
{

#ifdef __AVR__
	OCR1A = 125; // prescaler 64, clockfreq 8 Mhz, compare 125 implies delay 1ms.
	TIMSK1 |= _BV(OCIE1A); // enable interrupt on compare match
	TCCR1B |= _BV(WGM12); // set clear counter on timer compare

	TCCR1B |= (_BV(CS11) | _BV(CS10)); // Start timer (by setting prescaler)
#endif
}

#ifdef __AVR__

ISR(TIMER1_COMPA_vect)
{
	Debouncer_sampleAllDebouncers();
	Timer_tickAllTimers();
}

#endif


static void initIO(Garage_io_struct* io) {
#ifdef __AVR__
	io->ambientLight.port = &PORTB;
	io->ambientLight.pin = 1;

	io->leftOpenedSensor.port = &PORTB;
	io->leftOpenedSensor.pin = 1;

	io->leftClosedSensor.port = &PORTB;
	io->leftClosedSensor.pin = 1;

	io->leftGateSensor.port = &PORTB;
	io->leftGateSensor.pin = 1;

	io->leftRemote.port = &PORTB;
	io->leftRemote.pin = 1;

	io->rightOpenedSensor.port = &PORTB;
	io->rightOpenedSensor.pin = 1;

	io->rightClosedSensor.port = &PORTB;
	io->rightClosedSensor.pin = 1;

	io->rightGateSensor.port = &PORTB;
	io->rightGateSensor.pin = 1;

	io->rightRemote.port = &PORTB;
	io->rightRemote.pin = 1;

	io->trafficRed.port = &PORTB;
	io->trafficRed.pin = 1;

	io->trafficYellow.port = &PORTB;
	io->trafficYellow.pin = 1;

	io->trafficGreen.port = &PORTB;
	io->trafficGreen.pin = 1;
#endif

}
