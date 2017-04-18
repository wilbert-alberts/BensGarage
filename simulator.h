/*
 * simulator.h
 *
 *  Created on: Apr 18, 2017
 *      Author: walberts
 */

#ifndef BENSGARAGE_SIMULATOR_H_
#define BENSGARAGE_SIMULATOR_H_

#include "garage.h"
#include "hsi.h"

#include <stdint.h>

void SIM_initialize(Garage_io_struct* io);
void SIM_advanceTime();
uint8_t SIM_readPin(HSI_dio_struct io);
void SIM_writePin(HSI_dio_struct io, uint8_t value);


#endif /* BENSGARAGE_SIMULATOR_H_ */
