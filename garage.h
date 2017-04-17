/*
 * garage.h
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifndef GARAGE_H_
#define GARAGE_H_

#include "hsi.h"

typedef void* Garage;

typedef struct
{
	HSI_dio_struct leftOpenedSensor;
	HSI_dio_struct leftClosedSensor;
	HSI_dio_struct leftGateSensor;
	HSI_dio_struct leftRemote;

	HSI_dio_struct rightOpenedSensor;
	HSI_dio_struct rightClosedSensor;
	HSI_dio_struct rightGateSensor;
	HSI_dio_struct rightRemote;

	HSI_dio_struct trafficRed;
	HSI_dio_struct trafficYellow;
	HSI_dio_struct trafficGreen;

	HSI_dio_struct ambientLight;

} Garage_io_struct;

Garage* Garage_construct(Garage_io_struct* io);

#endif /* GARAGE_H_ */
