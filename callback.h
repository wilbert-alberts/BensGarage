/*
 * callback.h
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */

#ifndef CALLBACK_H_
#define CALLBACK_H_

#include <stdint.h>

typedef void (*CB_cb)(void* obj, void* context);

typedef struct
{
	void* obj;
	void* context;
	CB_cb callback;
} CB_callbackClient;

void CB_register(CB_callbackClient* cb, CB_cb fptr, void* obj, void* context);
void CB_unregister(CB_callbackClient* cb);
uint8_t CB_isRegistered(const CB_callbackClient* cb);
void CB_notify(const CB_callbackClient* cb);



#endif /* CALLBACK_H_ */
