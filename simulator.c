/*
 * simulator.c
 *
 *  Created on: Apr 18, 2017
 *      Author: walberts
 */

#include "hsi.h"

#include "simulator.h"
#include "log.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIM_GARAGE_INFILE ("garage.in.txt")
#define SIM_GARAGE_OUTFILE ("garage.out.txt")
#define SIM_SIZE_OF_MAP (sizeof(SIM_currentValues)/sizeof(SIM_map))

#define SIM_NR_INPUTS (6)

typedef struct {
	HSI_dio_struct io;
	uint8_t value;
	const char* ioname;
} SIM_map;

static SIM_map SIM_currentValues[12];
static uint32_t SIM_time = 0;
static FILE* SIM_infile;
static FILE* SIM_outfile;

static uint8_t SIM_nextValues[12];
static uint32_t SIM_nextEvent;

static void SIM_readNewState();
static void SIM_writeCurrentState();
static int SIM_findMapEntry(const HSI_dio_struct*);
static int SIM_readNewLine(uint32_t* time, uint8_t* nextValues, int nrValues);
static int SIM_compareState(int nrValues, int* v1, const SIM_map* map);


void SIM_initialize(Garage_io_struct* io) {
	int i;

	// Note: first SIM_NR_INPUTS must be inputs!

	SIM_currentValues[0].io = io->leftOpenedSensor;
	SIM_currentValues[1].io = io->leftClosedSensor;
	SIM_currentValues[2].io = io->leftGateSensor;
	SIM_currentValues[3].io = io->rightOpenedSensor;
	SIM_currentValues[4].io = io->rightClosedSensor;
	SIM_currentValues[5].io = io->rightGateSensor;

	SIM_currentValues[6].io = io->trafficRed;
	SIM_currentValues[7].io = io->trafficYellow;
	SIM_currentValues[8].io = io->trafficGreen;
	SIM_currentValues[9].io = io->ambientLight;

	SIM_currentValues[10].io = io->leftRemote;
	SIM_currentValues[11].io = io->rightRemote;


	SIM_currentValues[0].ioname = "leftOpenedSensor";
	SIM_currentValues[1].ioname = "leftClosedSensor";
	SIM_currentValues[2].ioname = "leftGateSensor";
	SIM_currentValues[3].ioname = "rightOpenedSensor";
	SIM_currentValues[4].ioname = "rightClosedSensor";
	SIM_currentValues[5].ioname = "rightGateSensor";

	SIM_currentValues[6].ioname = "trafficRed";
	SIM_currentValues[7].ioname = "trafficYellow";
	SIM_currentValues[8].ioname = "trafficGreen";
	SIM_currentValues[9].ioname = "ambientLight";

	SIM_currentValues[10].ioname = "leftRemote";
	SIM_currentValues[11].ioname = "rightRemote";

	// Set initial state:
	//   All doors closed, no gate blocked all lights off.
	for (i=0; i<11; i++) {
		SIM_currentValues[i].value = 0;
		SIM_nextValues[i] = 0;
	}
	SIM_currentValues[1].value = 1;
	SIM_currentValues[4].value = 1;
	SIM_nextValues[1] = 1;
	SIM_nextValues[4] = 1;

	SIM_time = 0;
	SIM_outfile = fopen(SIM_GARAGE_OUTFILE, "a");

	fprintf(SIM_outfile, "time\t");
	for (int i=0; i<SIM_SIZE_OF_MAP; i++) {
		fprintf(SIM_outfile, "%s\t", SIM_currentValues[i].ioname);
	}
	fprintf(SIM_outfile, "\n");
	fclose(SIM_outfile);

	SIM_writeCurrentState();

	SIM_infile = fopen(SIM_GARAGE_INFILE, "r");
	SIM_readNewState();

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
		int i;
		SIM_outfile = fopen(SIM_GARAGE_OUTFILE, "a");
		Log("Writing new values");

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

	while ((SIM_infile != NULL) && (SIM_time >= SIM_nextEvent) && (eof >= 0)) {
		Log("Getting new values");
		for (i = 0; i < SIM_SIZE_OF_MAP; i++) {
			SIM_currentValues[i].value = SIM_nextValues[i];
		}
		eof = SIM_readNewLine(&SIM_nextEvent, SIM_nextValues, SIM_NR_INPUTS);
	}
}

int SIM_readNewLine(uint32_t* time, uint8_t* nextValues, int nrValues) {
	char* line = NULL;
	size_t lineLength;
	int r;
	char* value;
	int idx = 0;
	int v;

	r = getline(&line, &lineLength, SIM_infile);
	while (r >=0 && line[0] == '#') {
		r = getline(&line, &lineLength, SIM_infile);

	}
	if (r >= 0) {
		value = strtok(line, ",\t\n");
		if (value != NULL) {
			v = atoi(value);
			*time = (uint32_t)v;
			value = strtok(NULL, ",\t\n");
		}
		while (idx < nrValues && value != NULL) {
			v = atoi(value);
			nextValues[idx] = (uint8_t)v;

			idx++;
			value = strtok(NULL, ",\t\n");
		}

	}
	free(line);

	return r;
}
