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

#define LOGGING_ON


#ifdef LOGGING_ON

void Log_P(const char* msg) {
  strcpy_P(line, msg);
  Serial.print(line);
}

void Log(const char* msg) {
  Serial.print(msg);
}

void LogChar(char c) {
  Serial.print(c);
}

void LogInt(int i) {
  Serial.print(i);
}

void Logln_P(const char* msg) {
  Log_P(msg);
  Serial.print("\r\n");
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

void Log_entry_P(const char* f) {
  Log_P(f);
  Logln_P(PSTR(" >()"));
}

void Log_exit_P(const char* f) {
  Log_P(f);
  Logln_P(PSTR(" <()"));
}

void Log_error_PP(const char* f, const char* msg) {
  Log_P(PSTR("Error in "));
  Log_P(f);
  Log_P(PSTR(": "));
  Logln_P(msg);
}

#else

void Log(const char* msg) {}
void Log_P(const char* msg) {}
void LogChar(char c) {}
void LogInt(int i) {}
void Logln(const char* msg) {}
void Logln_P(const char* msg) {}
void LogPort(const HSI_dio_struct* p) {}

void Log_entry_P(const char* f) {}
void Log_exit_P(const char* f) {}

void Log_error_PP(const char* f, const char* msg) {}


#endif

#endif /* LOG_H_ */
