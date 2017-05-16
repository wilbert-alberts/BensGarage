/*
 * callback.c
 *
 *  Created on: 17 apr. 2017
 *      Author: wilbert
 */


#include <stdio.h>
#include "callback.h"


void CB_register(CB_callbackClient* cb, CB_cb fptr, void* obj, void* context)
{
	cb->callback = fptr;
	cb->obj = obj;
	cb->context = context;
}

void CB_unregister(CB_callbackClient* cb)
{
	cb->callback = NULL;
	cb->obj=NULL;
	cb->context = NULL;
}

uint8_t CB_isRegistered(const CB_callbackClient* cb)
{
	if (cb->callback == NULL) {
		return 0;
	}
	return 1;
}

void CB_notify(const CB_callbackClient* cb)
{
	if (cb->callback != NULL) {
		cb->callback(cb->obj, cb->context);
	}
}



