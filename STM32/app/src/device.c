/*
 * device.c
 *
 *  Created on: 12 juin 2023
 *      Author: evan
 */
#include "device.h"

void APP_DEVICE_init(id_frame_t *device){
	device->address = DEFAULT_ADRESS;
	device->channel = DEFAULT_CHANNEL;
}
