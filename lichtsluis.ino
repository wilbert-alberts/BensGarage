
#include "log.h"
#include "garage.h"
#include "debouncer.h"
#include "timer.h"

static void initIO(Garage_io_struct* io);
static void installTimer();


void setup() {
  // Note actual baud rate is 4800 (16 times slower)
  //   Internal clock prescaler is set to 8 (factor 8)
  //   Regular arduino runs 16 Mhz but this on 8. (another factor 2)
  Serial.begin(76800);
 
  // put your setup code here, to run once:
  Garage_io_struct io;

  initIO(&io);

  Garage_construct(&io);
  installTimer();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
}

static void installTimer()
{
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 10000; // prescaler 1, clockfreq 1 Mhz, compare 10000 implies delay 10ms.
  TIMSK1 |= _BV(OCIE1A); // enable interrupt on compare match
  TCCR1B |= (_BV(WGM12)); // set clear counter on timer compare
  TCCR1A &= 0x00;
  
  TCCR1B |= _BV(CS10); // Start timer (by setting prescaler to 1)
}

ISR(TIMER1_COMPA_vect)
{
  Debouncer_sampleAllDebouncers();
  Timer_tickAllTimers();
}

static void initIO(Garage_io_struct* io) {
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
}
