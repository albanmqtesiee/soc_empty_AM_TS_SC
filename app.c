/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_common.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "app.h"
#include "app_log.h"
#include "temperature.h"
#include "irradiance.h"
#include "gatt_db.h"
#include "sl_sleeptimer.h"
#include "sl_simple_led_instances.h"





void cb_fonction(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  handle =handle;
  data =data;
  app_log_info("coucou");
}
// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;
sl_sleeptimer_timer_handle_t my_handle;//adresse du timer pour callbackNotify
sl_sleeptimer_timer_handle_t my_handle2;//adresse du timer pour irradNotify
int16_t temp;//temperature convertie
uint16_t sent_len;//taille de la temperature convertie
uint16_t sent_len2;//taille de l irradiance covnertie
uint8_t* data;//donnee recue
uint8_t taille;//taille de la donnee
float irradiance;//l irradiance recu

uint16_t characteristictemp;//caracteristique de la temperature apres chaque notification
uint16_t characteristiclum;//caracteristique de l irradiance apres chaque notification
uint8_t connectiontemp;//connexion utilisateur de la temperature apres chaque notification
uint8_t connectionlum;//connexion utilisateur de l irradiance apres chaque notification
/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  app_log_info("%s\n",__FUNCTION__);

  /////////////////////////////////////////////////////////////////////////////
}



/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;


  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      app_assert_status(sc);
      // Start advertising and enable connections.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_legacy_advertiser_connectable);
      app_assert_status(sc);
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      //log pour dire que déco
      app_log_info("%s: connection_opened\n",__FUNCTION__);
      sl_simple_led_init_instances();
      app_log_info("%s: led init\n",__FUNCTION__);
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:

      app_log_info("%s: not init\n",__FUNCTION__);
      //log pour dire que déco
      app_log_info("%s: connection_closed\n",__FUNCTION__);
      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Restart advertising after client has disconnected.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_legacy_advertiser_connectable);
      app_assert_status(sc);
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_gatt_server_user_read_request_id://demande de lecture de l'utilisateur

      //on verifie que l acces en lecture concerne bien la temperature
      if(evt->data.evt_gatt_server_user_read_request.characteristic == gattdb_temperature){
          app_log_info("%s: temperature reading...\n",__FUNCTION__);
          app_log_info("Condition de lecture vérifiée\n");
          temp = convertir_temp();//conversion de la temperature
          app_log_info("temperature is : %d C\n",temp);//affichage de la temperature dans les logs
          sl_bt_gatt_server_send_user_read_response(evt->data.evt_gatt_server_user_read_request.connection
                                                    ,gattdb_temperature,0,sizeof(temp),(const uint8_t*)&temp,& sent_len);//envoie de la temperature a l utilisateur
          app_log_info("temperature envoyee \n");

      }
      if(evt->data.evt_gatt_server_user_read_request.characteristic == gattdb_irradiance_0){//si l acces en lecture concerne l irradiance
          app_log_info("%s: irradiance reading...\n",__FUNCTION__);
           irradiance = irrad();//conversion de l irradiance
           sl_bt_gatt_server_send_user_read_response(evt->data.evt_gatt_server_user_read_request.connection
                                                               ,gattdb_irradiance_0,0,sizeof(irradiance),(const uint8_t*)&irradiance,& sent_len2);//envoie de l irradiance a l utilisateur
                     app_log_info("irradiance envoyee \n");

      }

    break;

    case sl_bt_evt_gatt_server_characteristic_status_id://verification de la notification recue
        //app_log_info("La valeur recue est %d \n", evt->data.evt_gatt_server_characteristic_status.client_config_flags);

         if(evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temperature){//la notification concerne la temperature
             if(evt->data.evt_gatt_server_characteristic_status.status_flags == 0x01){//changement detecte (appui de la cloche dans l app)
                 if(evt->data.evt_gatt_server_characteristic_status.client_config_flags){//on demande la notification
                     app_log_info("Coucou je suis temperature\n");
                     sl_sleeptimer_start_periodic_timer_ms(&my_handle,
                                                           1000,
                                                           callbackNotify,//fonction de notification de temperature.c
                                                           NULL,
                                                           0,
                                                           0);


                     app_log_info("la carte est notify par temperature \n");
                     connectiontemp = evt->data.evt_gatt_server_characteristic_status.connection;//sauvegarde de la connexion utilisateur
                     characteristictemp = evt->data.evt_gatt_server_characteristic_status.characteristic;//sauvegarde de la caracteristique
                     //app_log_info("La valeur recue est %d \n", evt->data.evt_gatt_server_characteristic_status.client_config_flags);
                 }
                 //
                 else{
                     sl_sleeptimer_stop_timer(&my_handle);// fin des notifications
                 }
                 //app_log_info("La valeur recue est %d \n", evt->data.evt_gatt_server_characteristic_status.client_config_flags);

             }

         }

         if(evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_irradiance_0){//notification vient d irradiance
                      if(evt->data.evt_gatt_server_characteristic_status.status_flags == 0x01){//changement detecte (appui sur la cloche dans l app)
                          if(evt->data.evt_gatt_server_characteristic_status.client_config_flags){//demande de notification
                              app_log_info("Coucou je suis irradiance\n");
                              sl_sleeptimer_start_periodic_timer_ms(&my_handle2,
                                                                    1000,
                                                                    irradNotify,//fonction de notification dans irradiance.c
                                                                    NULL,
                                                                    0,
                                                                    0);


                              app_log_info("la carte est notify par irradiance \n");
                              connectionlum = evt->data.evt_gatt_server_characteristic_status.connection;//sauvegarde de la connexion utilisateur
                              characteristiclum = evt->data.evt_gatt_server_characteristic_status.characteristic;//sauvegarde de la caracteristique
                              //app_log_info("La valeur recue est %d \n", evt->data.evt_gatt_server_characteristic_status.client_config_flags);
                          }
                          //
                          else{
                              sl_sleeptimer_stop_timer(&my_handle2);//fin des notifications
                          }
                          //app_log_info("La valeur recue est %d \n", evt->data.evt_gatt_server_characteristic_status.client_config_flags);

                      }

                  }

         break;
//Ajout de la notification
    case sl_bt_evt_system_external_signal_id:
      if(evt->data.evt_system_external_signal.extsignals == TEMPERATURE_TIMER_SIGNAL){//Signal de temperature recu
          temp = convertir_temp();
          app_log_info("temperature is : %d C\n",temp);
          sl_bt_gatt_server_send_notification(connectiontemp
                                                    ,gattdb_temperature,sizeof(temp),(const uint8_t*)&temp);//envoi de la temperature periodiquement
      }
      else if(evt->data.evt_system_external_signal.extsignals == IRRADIANCE_TIMER_SIGNAL){//Signal d'irradiance recu
                irradiance = irrad();
                app_log_info("irradiance is : %d \n",(int)( 100*irradiance));
                sl_bt_gatt_server_send_notification(connectionlum
                                                          ,gattdb_irradiance_0,sizeof(irradiance),(const uint8_t*)&irradiance);//envoi de l irradiance periodiquement
            }
      break;
//Demande d'écriture
    case sl_bt_evt_gatt_server_user_write_request_id:

      taille = evt->data.evt_gatt_server_user_write_request.value.len;//taille de la valeur ecrite
      data = evt->data.evt_gatt_server_user_write_request.value.data;//donnee ecrite
      for(int i = 0; i < taille; i++){
          if(data[i] == 1){
              sl_led_led0.turn_on(sl_led_led0.context);//allume la led
          }
          else if(data[i] == 0){
              sl_led_led0.turn_off(sl_led_led0.context);//eteint la led
          }
          app_log_info("Donnee : %d \n", data[i]);
          app_log_info("coucou la valeur c'est %d \n", evt->data.evt_gatt_server_user_write_request.att_opcode);
      }
      app_log_info("Coucou on ecrit !! \n");
      if(evt->data.evt_gatt_server_user_write_request.att_opcode == sl_bt_gatt_write_request ){//regarde si ecrit avec reponse
          app_log_info("Response is requested");
          sl_bt_gatt_server_send_user_write_response(evt->data.evt_gatt_server_user_read_request.connection,
                                                          evt->data.evt_gatt_server_user_read_request.characteristic,
                                                                      0);//envoi la reponse pour pas que ca crash

      }
      else if(evt->data.evt_gatt_server_user_write_request.att_opcode == sl_bt_gatt_write_command ){//regarde si ecrit sans reponse
          app_log_info("No response required");
      }

      break;



    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}
