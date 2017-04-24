/*
 * simulator.c
 *
 *  Created on: Apr 18, 2017
 *      Author: walberts
 */

#include "hsi.h"

#include "simulator.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIM_GARAGE_INFILE ("garage.in.txt")
#define SIM_GARAGE_OUTFILE ("garage.out.txt")
#define SIM_SIZE_OF_MAP (sizeof(SIM_currentValues)/sizeof(SIM_map))

typedef struct {
	HSI_dio_struct io;
	uint8_t value;
	const char* ioname;
} SIM_map;

static SIM_map SIM_currentValues[12];
static uint32_t SIM_time = 0;
static FILE* SIM_infile;
static FILE* SIM_outfile;

static void SIM_readNewState();
static void SIM_writeCurrentState();
static int SIM_findMapEntry(const HSI_dio_struct*);
static int SIM_readNewLine(int* time, int* nextValues, int nrValues);
static int SIM_compareState(int nrValues, int* v1, const SIM_map* map);


void SIM_initialize(Garage_io_struct* io) {
	SIM_currentValues[io->leftOpenedSensor.pin].io = io->leftOpenedSensor;
	SIM_currentValues[io->leftClosedSensor.pin].io = io->leftClosedSensor;
	SIM_currentValues[io->leftGateSensor.pin].io = io->leftGateSensor;
	SIM_currentValues[io->leftRemote.pin].io = io->leftRemote;
	SIM_currentValues[io->rightOpenedSensor.pin].io = io->rightOpenedSensor;
	SIM_currentValues[io->rightClosedSensor.pin].io = io->rightClosedSensor;
	SIM_currentValues[io->rightGateSensor.pin].io = io->rightGateSensor;
	SIM_currentValues[io->rightRemote.pin].io = io->rightRemote;
	SIM_currentValues[io->trafficRed.pin].io = io->trafficRed;
	SIM_currentValues[io->trafficYellow.pin].io = io->trafficYellow;
	SIM_currentValues[io->trafficGreen.pin].io = io->trafficGreen;
	SIM_currentValues[io->ambientLight.pin].io = io->ambientLight;

	SIM_currentValues[io->leftOpenedSensor.pin].ioname = "leftOpenedSensor";
	SIM_currentValues[io->leftClosedSensor.pin].ioname = "leftClosedSensor";
	SIM_currentValues[io->leftGateSensor.pin].ioname = "leftGateSensor";
	SIM_currentValues[io->leftRemote.pin].ioname = "leftRemote";
	SIM_currentValues[io->rightOpenedSensor.pin].ioname = "rightOpenedSensor";
	SIM_currentValues[io->rightClosedSensor.pin].ioname = "rightClosedSensor";
	SIM_currentValues[io->rightGateSensor.pin].ioname = "rightGateSensor";
	SIM_currentValues[io->rightRemote.pin].ioname = "rightRemote";
	SIM_currentValues[io->trafficRed.pin].ioname = "trafficRed";
	SIM_currentValues[io->trafficYellow.pin].ioname = "trafficYellow";
	SIM_currentValues[io->trafficGreen.pin].ioname = "trafficGreen";
	SIM_currentValues[io->ambientLight.pin].ioname = "ambientLight";

#ifdef SIMULATOR
	SIM_time = 0;
	SIM_infile = fopen(SIM_GARAGE_INFILE, "r");
	SIM_outfile = fopen(SIM_GARAGE_OUTFILE, "a");
	SIM_readNewState();
#endif

	fprintf(SIM_outfile, "time\t");
	for (int i=0; i<SIM_SIZE_OF_MAP; i++) {
		fprintf(SIM_outfile, "%s\t", SIM_currentValues[i].ioname);
	}
	fprintf(SIM_outfile, "\n");
	fclose(SIM_outfile);
}

void SIM_advanceTime() {
	SIM_writeCurrentState();
	SIM_time++;
	printf("Time: %lu\n", (long unsigned int)SIM_time);
	SIM_readNewState();
}

uint8_t SIM_readPin(HSI_dio_struct io) {
	int idx = SIM_findMapEntry(&io);
	if (idx >= 0) {
		return SIM_currentValues[idx].value;
	}
	// TODO: return error.
	return 0;
}

void SIM_writePin(HSI_dio_struct io, uint8_t value) {
	int idx = SIM_findMapEntry(&io);
	if (idx >= 0) {
		SIM_currentValues[idx].value = value;
	}
}

int SIM_findMapEntry(const HSI_dio_struct* io) {
	int i;
	for (i = 0; i < SIM_SIZE_OF_MAP; i++) {
		if ((io->pin == SIM_currentValues[i].io.pin)
				&& (io->port == SIM_currentValues[i].io.port)) {
			return i;
		}
	}
	return -1;
}

void SIM_writeCurrentState() {
	static int previousState[SIM_SIZE_OF_MAP] = { 0 };

	if (SIM_compareState(SIM_SIZE_OF_MAP, previousState, SIM_currentValues)
			!= 0) {

		SIM_outfile = fopen(SIM_GARAGE_OUTFILE, "a");
		Log("Writing new values");

		int i;
		fprintf(SIM_outfile, "%lu\t", (long unsigned int)SIM_time);
		for (i = 0; i < SIM_SIZE_OF_MAP; i++) {
			fprintf(SIM_outfile, "%d\t", SIM_currentValues[i].value);
			previousState[i] = SIM_currentValues[i].value;
		}
		fprintf(SIM_outfile, "\n");
		fclose(SIM_outfile);
	}
}

int SIM_compareState(int nrValues, int* v1, const SIM_map* map) {
	int i = 0;

	for (i = 0; i < nrValues; i++) {
		if (v1[i] != map[i].value)
			return -1;
	}
	return 0;
}

void SIM_readNewState() {
	int i;
	static int eof = 0;
	static int nextEvent = 0;
	static int nextValues[SIM_SIZE_OF_MAP] = { 0 };

	while ((SIM_time >= nextEvent) && (eof >= 0)) {
		Log("Getting new values");
		for (i = 0; i < SIM_SIZE_OF_MAP; i++) {
			SIM_currentValues[i].value = nextValues[i];
		}
		eof = SIM_readNewLine(&nextEvent, nextValues, SIM_SIZE_OF_MAP);
	}
}

int SIM_readNewLine(int* time, int* nextValues, int nrValues) {
	char* line = NULL;
	size_t lineLength;
	int r;
	char* value;
	int idx = 0;

	r = getline(&line, &lineLength, SIM_infile);
	if (r >= 0) {
		value = strtok(line, ",\t\n");
		if (value != NULL) {
			*time = atoi(value);
			value = strtok(NULL, ",\t\n");
		}
		while (idx < nrValues && value != NULL) {
			nextValues[idx] = atoi(value);
			idx++;
			value = strtok(NULL, ",\t\n");
		}
	}
	free(line);

	return r;
}
