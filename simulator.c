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


#define SIM_GARAGE_INFILE ("garage.in.txt")
#define SIM_GARAGE_OUTFILE ("garage.out.txt")
#define SIM_SIZE_OF_MAP (sizeof(map)/sizeof(SIM_map))

typedef struct
{
	HSI_dio_struct io;
	uint8_t        value;
} SIM_map;

static SIM_map map[12];
static uint32_t time = 0;
static FILE* infile;
static FILE* outfile;

static void SIM_readNewState();
static void SIM_writeCurrentState();
static int  SIM_findMapEntry(const HSI_dio_struct*);


void SIM_initialize(Garage_io_struct* io)
{
	map[0].io = io->leftOpenedSensor;
	map[1].io = io->leftClosedSensor;
	map[2].io = io->leftGateSensor;
	map[3].io = io->leftRemote;
	map[4].io = io->rightOpenedSensor;
	map[5].io = io->rightClosedSensor;
	map[6].io = io->rightGateSensor;
	map[7].io = io->rightRemote;
	map[8].io = io->trafficRed;
	map[9].io = io->trafficYellow;
	map[10].io = io->trafficGreen;
	map[11].io = io->ambientLight;

#ifdef SIMULATOR
	time = 0;
	infile = fopen(SIM_GARAGE_INFILE, "r");
	outfile = fopen(SIM_GARAGE_OUTFILE, "w");
#endif

	fprintf(outfile, "leftOpenedSensor\t");
	fprintf(outfile, "leftClosedSensor\t");
	fprintf(outfile, "leftGateSensor\t");
	fprintf(outfile, "leftRemote\t");
	fprintf(outfile, "rightOpenedSensor\t");
	fprintf(outfile, "rightClosedSensor\t");
	fprintf(outfile, "rightGateSensor\t");
	fprintf(outfile, "rightRemote\t");
	fprintf(outfile, "trafficRed\t");
	fprintf(outfile, "trafficYellow\t");
	fprintf(outfile, "trafficGreen\t");
	fprintf(outfile, "ambientLight\n");
	fsync(fileno(outfile));
}

void SIM_advanceTime()
{
	SIM_writeCurrentState();
	time++;
	SIM_readNewState();
}

uint8_t SIM_readPin(HSI_dio_struct io)
{
	int idx = SIM_findMapEntry(&io);
	if (idx>=0) {
		return map[idx].value;
	}
	// TODO: return error.
	return 0;
}

void SIM_writePin(HSI_dio_struct io, uint8_t value)
{
	int idx = SIM_findMapEntry(&io);
	if (idx>=0) {
		map[idx].value = value;
	}
	// TODO: return error.
}

int SIM_findMapEntry(const HSI_dio_struct* io) {
	int i;
	for (i=0; i<SIM_SIZE_OF_MAP; i++) {
		if ((io->pin == map[i].io.pin) &&
		    (io->port == map[i].io.port)) {
			return i;
		}
	}
	return -1;
}

void SIM_writeCurrentState()
{
	int i;
	fprintf(outfile, "%ud\t", time);
	for (i=0; i<SIM_SIZE_OF_MAP; i++) {
		fprintf(outfile, "%d\t", map[i].value);
	}
	fprintf(outfile, "\n");
	fsync(fileno(outfile));
}

void SIM_readNewState()
{
	int i;
	int eof = 0;
	static int nextEvent=0;
	static int nextValues[SIM_SIZE_OF_MAP] = {0};

	while ((time>=nextEvent) && (eof>=0)) {
		for (i=0; i<SIM_SIZE_OF_MAP; i++) {
			map[i].value = nextValues[i];
		}
		eof = fscanf(infile, "%d\t", &nextEvent);
		for (i=0; (eof>0) && (i<SIM_SIZE_OF_MAP); i++) {
			eof = fscanf(infile, "%d\t", &nextValues[i]);
		}
	}
}
