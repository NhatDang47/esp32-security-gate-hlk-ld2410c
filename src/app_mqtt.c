#include "app_mqtt.h"
#include "app_config.h"
#include "shared_defs.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "MQTT_APP";
static esp_mqtt_client_handle_t mqtt_client = NULL;
volatile bool g_mqtt_connected = false;

#define MQTT_TOPIC "gate/telemetry"

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            g_mqtt_connected = true;
            esp_mqtt_client_subscribe(mqtt_client, "gate/control", 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT Disconnected");
            g_mqtt_connected = false;
            break;
        case MQTT_EVENT_DATA:
            if (strncmp(event->topic, "gate/control", event->topic_len) == 0) {
                if (strncmp(event->data, "{\"cmd\":\"reset\"}", event->data_len) == 0 || 
                    strncmp(event->data, "{\"cmd\": \"reset\"}", event->data_len) == 0) {
                    g_count_in = 0;
                    g_count_out = 0;
                    ESP_LOGI(TAG, "Nhan lenh RESET, da xoa bo dem!");
                    mqtt_publish_event("RESET_DONE", 0, 0, 0, 0);
                }
            }
            break;
        default:
            break;
    }
}

void mqtt_app_init(void) {
    AppConfig_t cfg;
    config_load(&cfg);
    
    if (strlen(cfg.mqtt_uri) == 0) {
        ESP_LOGW(TAG, "No MQTT URI configured.");
        return;
    }

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = cfg.mqtt_uri,
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void mqtt_publish_event(const char *action, int distance, int energy, int count_in, int count_out) {
    if (mqtt_client == NULL || !g_mqtt_connected) return;
    
    char payload[160];
    snprintf(payload, sizeof(payload),
             "{\"action\":\"%s\",\"distance\":%d,\"energy\":%d,\"count_in\":%d,\"count_out\":%d}",
             action, distance, energy, count_in, count_out);
             
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC, payload, 0, 1, 0);
    ESP_LOGI(TAG, "MQTT Pub: %s", payload);
}
