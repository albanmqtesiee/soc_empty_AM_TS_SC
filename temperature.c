/*
 * temperature.c
 *
 *  Created on: 30 mai 2024
 *      Author: Alban
 */
#include "temperature.h"
#include "sl_sensor_rht.h"
#include "app_log.h"
#include "sl_sleeptimer.h"
#include "sl_bt_api.h"

#define TEMPERATURE_TIMER_SIGNAL (1<<0)

int32_t temperature;
uint32_t humidite;

int32_t temp_recup;
int16_t temp_conv;

int convertir_temp(){
  sl_sensor_rht_init();
  temp_recup = sl_sensor_rht_get(&humidite, &temperature);
  sl_sensor_rht_deinit();
  temp_conv = temperature * 0.1;
  return temp_conv;
}


void  callbackNotify(sl_sleeptimer_timer_handle_t *handle, void *data){
handle = handle;
data = data;
  //uint16_t count = 0;
  sl_bt_external_signal(TEMPERATURE_TIMER_SIGNAL);
  static uint16_t step = 0;
  app_log_info("Timer step %u\n", step);
  step = step + 1;

}
