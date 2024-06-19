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

#define TEMPERATURE_TIMER_SIGNAL (1<<0)//signal de temperature

int32_t temperature;//donnee du capteur
uint32_t humidite;//donnee du capteur

int32_t temp_recup;//temeprature recue
int16_t temp_conv;//temperature convertie

//Conversion de la temperature
int convertir_temp(){
  sl_sensor_rht_init();
  temp_recup = sl_sensor_rht_get(&humidite, &temperature);//lecture du capteur
  sl_sensor_rht_deinit();
  temp_conv = temperature * 0.1;//conversion
  return temp_conv;
}

//Fonction de notification 
void  callbackNotify(sl_sleeptimer_timer_handle_t *handle, void *data){
handle = handle;
data = data;
  //uint16_t count = 0;
  sl_bt_external_signal(TEMPERATURE_TIMER_SIGNAL);//generation du signal
  static uint16_t step = 0;
  app_log_info("Timer step %u\n", step);//For debug
  step = step + 1;

}
