/*
 * temperature.c
 *
 *  Created on: 30 mai 2024
 *      Author: Alban
 */
#include "temperature.h"
#include "sl_sensor_rht.h"
#include "app_log.h"
int32_t temperature;
uint32_t humidite;

int32_t temp_recup;
int16_t temp_conv;

int convertir_temp(){
  sl_sensor_rht_init();
  temp_recup = sl_sensor_rht_get(&humidite, &temperature);
  sl_sensor_rht_deinit();
  temp_conv = temperature*0.1;

  return temp_conv;
}


sl_sleeptimer_timer_callback_t callbackNotify(){
 static uint8_t count = 0;
  app_log_info("Timer step %u\n", count);
  count +=1;

}
