#include "app_wifi.h"
#include "app_config.h"
#include "shared_defs.h"
#include "app_webserver.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include <string.h>

static const char *TAG = "WIFI_APP";

volatile bool g_wifi_connected = false;
volatile bool g_ap_mode_active = false;
char g_device_ip[20] = "192.168.4.1";

static void wifi_retry_timer_cb(TimerHandle_t xTimer) {
    ESP_LOGI(TAG, "Executing WiFi retry...");
    esp_wifi_connect();
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "Station " MACSTR " joined, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "Station " MACSTR " left, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        g_wifi_connected = false;
        ESP_LOGW(TAG, "WiFi STA disconnected. Retrying in 10 seconds...");
        
        // Sử dụng FreeRTOS Timer để tránh kẹt Event Loop
        static TimerHandle_t wifi_retry_timer = NULL;
        if (wifi_retry_timer == NULL) {
            wifi_retry_timer = xTimerCreate("wifi_retry", pdMS_TO_TICKS(10000), pdFALSE, NULL, wifi_retry_timer_cb);
        }
        xTimerStart(wifi_retry_timer, 0);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        sprintf(g_device_ip, IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "STA Got IP: %s", g_device_ip);
        g_wifi_connected = true;
    }
}

void wifi_app_init(void) {
    AppConfig_t cfg;
    config_load(&cfg);

    esp_netif_init();
    esp_event_loop_create_default();
    
    // Create both AP and STA netifs
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&init_cfg);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);

    wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid = "ESP32_SecurityGate",
            .ssid_len = strlen("ESP32_SecurityGate"),
            .channel = 1,
            .password = "", // Open network for easy config
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN
        },
    };

    wifi_config_t wifi_sta_config = {0};
    strcpy((char *)wifi_sta_config.sta.ssid, cfg.wifi_ssid);
    strcpy((char *)wifi_sta_config.sta.password, cfg.wifi_password);
    
    esp_wifi_set_mode(WIFI_MODE_APSTA);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config);
    
    if (strlen(cfg.wifi_ssid) > 0) {
        esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config);
    }
    
    esp_wifi_start();
    g_ap_mode_active = true;
    
    ESP_LOGI(TAG, "WiFi AP+STA started. AP SSID: ESP32_SecurityGate, IP: 192.168.4.1");
    if (strlen(cfg.wifi_ssid) > 0) {
        ESP_LOGI(TAG, "Attempting to connect to STA: %s", cfg.wifi_ssid);
    } else {
        ESP_LOGW(TAG, "No STA SSID configured. Running AP only.");
    }

    // Bật Web Server sau khi WiFi đã khởi tạo
    webserver_start();
}
