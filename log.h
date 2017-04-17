/*
 * log.h
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifndef LOG_H_
#define LOG_H_

void Log(const char* msg);
void Log_entry(const char* f, const char* msg);
void Log_exit(const char* f, const char* msg);

void Log_error(const char* f, const char* msg);

#endif /* LOG_H_ */
