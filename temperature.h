/*
 * temperature.h
 *
 *  Created on: 30 mai 2024
 *      Author: Alban
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#include "sl_sleeptimer.h"


//Réaliser la conversion de température

int convertir_temp();
void  callbackNotify(sl_sleeptimer_timer_handle_t *handle, void *data);

#endif /* TEMPERATURE_H_ */
