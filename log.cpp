/*
   log.h

    Created on: 17 apr. 2017
        Author: wilbert
*/

#ifndef LOG_H_
#define LOG_H_

#include <avr/pgmspace.h>

#include <stdio.h>
#include <stdlib.h>

#include <HardwareSerial.h>

#include "hsi.h"

static char line[80];

void Log(const char* msg) {
  strcpy_P(line, msg);
  Serial.print(line);
  
}

void LogChar(char c) {
  Serial.print(c);
}

void Logln(const char* msg) {
  Log(msg);
  Serial.print("\r\n");
}

void LogPort(const HSI_dio_struct* p)
{
  static char bfr[7];

  HSI_toString(p, bfr);
  Serial.print(bfr);
}

void Log_entry(const char* f) {
  Log(f);
  Logln(PSTR(" >()"));
}

void Log_exit(const char* f) {
  Log(f);
  Logln(PSTR(" <()"));
}

void Log_error(const char* f, const char* msg) {
  Log(PSTR("Error in "));
  Log(f);
  Log(PSTR(": "));
  Logln(msg);
}

#endif /* LOG_H_ */
