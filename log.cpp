/*
   log.h

    Created on: 17 apr. 2017
        Author: wilbert
*/

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <stdlib.h>

#include <HardwareSerial.h>

void Log(const char* msg) {
  Serial.print(msg);
  Serial.print("\n");
}

void Log_entry(const char* f, const char* msg) {
  Serial.print(f);
  Serial.print(" >(");
  if (msg != NULL)
    Serial.print(msg);
  Serial.print(")\n");
}

void Log_exit(const char* f, const char* msg) {
  Serial.print(f);
  Serial.print(" <(");
  if (msg != NULL)
    Serial.print(msg);
  Serial.print(")\n");
}

void Log_error(const char* f, const char* msg) {
  Serial.print("Error in ");
  Serial.print(f);
  Serial.print(": ");
  Serial.print(msg);
  Serial.print("\n");
}

#endif /* LOG_H_ */
