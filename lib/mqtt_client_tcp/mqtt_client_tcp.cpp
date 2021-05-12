/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <string>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
// #include "protocol_examples_common.h"
#include <iostream>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"

#include "mqtt_client_tcp.h"

#ifdef MQTT_EXAMPLE_LOCAL
    static std::string Distributeur_id = "1001";
#endif

static bool mqtt_service_connected = false;
static const char *TAG = "MQTT_EXAMPLE";

QueueHandle_t semaphore_mqtt_event = NULL;

// char *former_topic(char* prefixe, char* dist, char* topic){
//     char *topicSubscribe_prefixe = strcat(prefixe, dist);
//     char *topicSubscribe = strcat(topicSubscribe_prefixe, topic);
//     return topicSubscribe;
// }
// static const char* topicSubscribe_all = "imeds/database/#";
// static const char* topicSubscribe_User = former_topic("imeds/database/", Distributeur_id, "/user/#");
// static const char* topicSubscribe_Product = former_topic("imeds/database/", Distributeur_id, "/product/#");
// static const char* topicSubscribe_Hw = former_topic("imeds/database/", Distributeur_id, "/hw/#");
// static const char* topicSubscribe_App = former_topic("imeds/database/", Distributeur_id, "/app/#");
// static const char* topicSubscribe_Mtime = former_topic("imeds/database/", Distributeur_id, "/mtime/#");


static const char* topicSubscribe_all = "imeds/database/#";
std::string topicSubscribe_User = "imeds/database/" + Distributeur_id + "/user/#";
std::string topicSubscribe_Product = "imeds/database/" + Distributeur_id + "/product/#";
std::string topicSubscribe_Hw = "imeds/database/" + Distributeur_id + "/hw/#";
std::string topicSubscribe_App = "imeds/database/" + Distributeur_id + "/app/#";
std::string topicSubscribe_Mtime = "imeds/database/" + Distributeur_id + "/mtime/#";

static std::vector<std::string> manual_split_topic (std::string context, char split_sign){
    std::vector<std::string> topic_splited;
    std::string res = context;
    int pos;
    std::string topic_slot;
    while (res!="")
    {
        pos = res.find(split_sign);
        if(pos==-1){
            topic_slot=res;
            res = "";
        }else{
            topic_slot = res.substr(0,pos);
            res = res.substr(pos+1,res.length());
        }
        topic_splited.push_back(topic_slot);
    }
    return topic_splited;
}

static void on_connect_subscribe(esp_mqtt_client_handle_t client){
    int msg_id;

    // sd_card_dlog_comm(strcat("subscribe to topic",topicSubscribe_User));
    msg_id = esp_mqtt_client_subscribe(client, topicSubscribe_User.c_str(), 1);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d, topic=%s", 
                            msg_id, topicSubscribe_User.c_str());

    // sd_card_dlog_comm(strcat("subscribe to topic",topicSubscribe_Product));
    msg_id = esp_mqtt_client_subscribe(client, topicSubscribe_Product.c_str(), 1);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d, topic=%s", 
                            msg_id, topicSubscribe_Product.c_str());

    // sd_card_dlog_comm(strcat("subscribe to topic",topicSubscribe_Hw));
    msg_id = esp_mqtt_client_subscribe(client, topicSubscribe_Hw.c_str(), 1);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d, topic=%s", 
                            msg_id, topicSubscribe_Hw.c_str());

    // sd_card_dlog_comm(strcat("subscribe to topic",topicSubscribe_App));
    msg_id = esp_mqtt_client_subscribe(client, topicSubscribe_App.c_str(), 1);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d, topic=%s", 
                            msg_id, topicSubscribe_App.c_str());

    // sd_card_dlog_comm(strcat("subscribe to topic",topicSubscribe_Mtime));
    msg_id = esp_mqtt_client_subscribe(client, topicSubscribe_Mtime.c_str(), 1);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d, topic=%s", 
                            msg_id, topicSubscribe_Mtime.c_str());

}

esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            mqtt_service_connected= true;  
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

            on_connect_subscribe(client);
            break;
        case MQTT_EVENT_DISCONNECTED:
            // sd_card_dlog_comm("MQTT_EVENT_DISCONNECTED");
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            mqtt_service_connected = false;  
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb((esp_mqtt_event_handle_t)event_data);
}

void manageReceivedMsg(char* topic, char* msg){
    std::string topic_str = topic;
}

void publish_test(esp_mqtt_client_handle_t client, std::string topic, std::string data){
    int msg_id;
    msg_id = esp_mqtt_client_publish(client, topic.c_str(), data.c_str(), 0, 1, 0);
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d, topic=%s", msg_id,topic.c_str());
}

void mqtt_app_start(void)
{
#ifdef MQTT_EXAMPLE_LOCAL
    esp_mqtt_client_config_t mqtt_cfg = {
        .host = mqtt_server_local_host,
        .port = mqtt_server_local_port,
        .keepalive = keepalive_delay,
    };
#elif defined(MQTT_URI)
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
    };
#elif defined(MQTT_CONF_FILE)
    //get conf info from json conf file
#endif
#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0) {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128) {
            int c = fgetc(stdin);
            if (c == '\n') {
                line[count] = '\0';
                break;
            } else if (c > 0 && c < 127) {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.uri = line;
        printf("Broker url: %s\n", line);
    } else {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
    publish_test(client, "imeds/database/1001/user/id", "xiao");

}

bool mqtt_is_connected(){
    return mqtt_service_connected;
}

void mqtt_trytostart(void)
{
    ESP_LOGI(TAG, "[MQTT] Startup..");
    ESP_LOGI(TAG, "[MQTT] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[MQTT] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    semaphore_mqtt_event = xSemaphoreCreateBinary();

    // ESP_ERROR_CHECK(nvs_flash_init());
    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    // ESP_ERROR_CHECK(example_connect());

    mqtt_app_start();
}
