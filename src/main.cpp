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
#include <wifi_example.h>
#include <tcp_client.h>

#ifdef __cplusplus 
extern "C"{
#endif


void app_main() {
    ten_blink();
    printf("Tring to connect to wifi......");
    wifi_example_connect();

    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);

}

#ifdef __cplusplus
}
#endif