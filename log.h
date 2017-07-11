/*
 * log.h
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifndef LOG_H_
#define LOG_H_

#include <avr/pgmspace.h>

#include "hsi.h"

void Log(const char* msg);
void Log_P(const char* msg);
void LogChar(char c);
void LogInt(int i);
void Logln(const char* msg);
void Logln_P(const char* msg);
void LogPort(const HSI_dio_struct* p);

void Log_entry_P(const char* f);
void Log_exit_P(const char* f);

void Log_error_PP(const char* f, const char* msg);

#endif /* LOG_H_ */
