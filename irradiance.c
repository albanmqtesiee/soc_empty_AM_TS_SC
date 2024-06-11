/*
 * irradiance.c
 *
 *  Created on: 11 juin 2024
 *      Author: Alban
 */

#include "irradiance.h"
#include "sl_sensor_light.h"
#include "app_log.h"
#include "sl_sleeptimer.h"
#include "sl_bt_api.h"

#define IRRADIANCE_TIMER_SIGNAL (1<<1)

float irrad_recup;
float irrad_conv;

float lux;
float uvi;



float irrad(){
  sl_sensor_light_init();
  irrad_recup = sl_sensor_light_get(&lux, &uvi);

  irrad_conv = lux * 0.1/683;
  sl_sensor_light_deinit();
  return irrad_conv;
}

void  irradNotify(sl_sleeptimer_timer_handle_t *handle, void *data){
handle = handle;
data = data;
  //uint16_t count = 0;
  sl_bt_external_signal(IRRADIANCE_TIMER_SIGNAL);
  static uint16_t step1 = 0;
  app_log_info("Timer step %u\n", step1);
  step1 = step1 + 1;

}
