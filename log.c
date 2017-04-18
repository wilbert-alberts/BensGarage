/*
 * log.h
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <stdlib.h>

void Log(const char* msg) {
#ifndef SIMULATOR
#else
	fprintf(stderr, "%s\n", msg);
#endif
}

void Log_entry(const char* f, const char* msg) {
#ifndef SIMULATOR
#else
	fprintf(stderr, "%s >(%s)\n", f, msg);
#endif
}

void Log_exit(const char* f, const char* msg) {
#ifndef SIMULATOR
#else
	fprintf(stderr, "%s <(%s)\n", f, msg);
#endif
}

void Log_error(const char* f, const char* msg) {
#ifndef SIMULATOR
#else
	fprintf(stderr, "Error in %s: %s\n", f, msg);
	exit(0);
#endif
}

#endif /* LOG_H_ */
