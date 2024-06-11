/*
 * irradiance.h
 *
 *  Created on: 11 juin 2024
 *      Author: Alban
 */
#include "sl_sleeptimer.h"


#define IRRADIANCE_TIMER_SIGNAL (1<<1)

#ifndef IRRADIANCE_H_
#define IRRADIANCE_H_

float irrad();

void  irradNotify(sl_sleeptimer_timer_handle_t *handle, void *data);

#endif /* IRRADIANCE_H_ */
