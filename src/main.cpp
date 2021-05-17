#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

#include <blink.h>
#include <tcp_client_socket.h>
#include "WiFi.h"
#include "mqtt_client_tcp.h"
#include "sd_card_my.h"

#ifdef __cplusplus 
extern "C"{
#endif

void restart_tcp_socket (){

}

void app_main() {

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ten_blink();
    printf("Tring to connect to wifi......");

    WiFi My_Wifi_connection;
    My_Wifi_connection.connectAP("BEENETIC_FIBRE","CairZigWatt2010");
    //xTaskCreate(tcp_client_task, "tcp_client_socket", 4096, NULL, 5, NULL);
    mqtt_app_start();

    sd_card_init();
    sd_card_dlog_comm("test");

}

#ifdef __cplusplus
}
#endif