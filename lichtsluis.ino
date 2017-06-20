
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
  Serial.begin(115200);

  Logln(PSTR("Lichtsluis starting up"));
 
  Garage_io_struct io;

  initIO(&io);

  Garage_construct(&io);
  installTimer();

  Logln(PSTR("Lichtsluis started"));
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
}

static void installTimer()
{
  Logln(PSTR("Installing timer"));
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // Set timer ticks to 10ms 

  OCR1A = 6250; // prescaler 256, clockfreq 16 Mhz, compare 625 implies delay 10ms.
  TIMSK1 |= _BV(OCIE1A); // enable interrupt on compare match
  TCCR1B |= (_BV(WGM12)); // set clear counter on timer compare
  TCCR1A &= 0x00;
  
  TCCR1B |= _BV(CS12); // Start timer (by setting prescaler to 256)
  Logln(PSTR("Timer started"));
}

ISR(TIMER1_COMPA_vect)
{
  Debouncer_sampleAllDebouncers();
  Timer_tickAllTimers();
}

static void initIO(Garage_io_struct* io) {
  Logln(PSTR("Configure IO"));
  io->ambientLight.port = &PORTC;
  io->ambientLight.pin = 3;

  io->leftOpenedSensor.port = &PIND;
  io->leftOpenedSensor.pin = 3;

  io->leftClosedSensor.port = &PIND;
  io->leftClosedSensor.pin = 2;

  io->leftGateSensor.port = &PIND;
  io->leftGateSensor.pin = 4;

  io->leftRemote.port = &PORTC;
  io->leftRemote.pin = 4;

  io->rightOpenedSensor.port = &PIND;
  io->rightOpenedSensor.pin = 6;

  io->rightClosedSensor.port = &PIND;
  io->rightClosedSensor.pin = 5;

  io->rightGateSensor.port = &PIND;
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
  Logln(PSTR("IO Configured"));
}
