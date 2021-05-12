#ifndef MQTT_CLIENT_TCP_H
#define MQTT_CLIENT_TCP_H

#ifdef __cplusplus 
extern "C"{
#endif

// #include "sd_card_my.h"
#include "mqtt_client.h"


#define MQTT_EXAMPLE_LOCAL

#ifdef MQTT_EXAMPLE_LOCAL
    #define mqtt_server_local_host "192.168.1.88"
    #define mqtt_server_local_port 1883
    #define keepalive_delay 3600
    
#endif

esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void mqtt_app_start(void);
bool mqtt_is_connected();

#ifdef __cplusplus 
}
#endif

#endif