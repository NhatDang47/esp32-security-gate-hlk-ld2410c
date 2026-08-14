#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Includes modules
#include "app_config.h"
#include "app_wifi.h"
#include "app_mqtt.h"
#include "app_action.h"
#include "app_lcd.h"
#include "app_radar.h"

static const char *TAG = "MAIN_APP";

void app_main(void) {
    ESP_LOGI(TAG, "Starting Security Gate System...");

    // 1. Khởi tạo NVS và cấu hình
    config_init();

    // 2. Khởi tạo Action Queue và GPIOs (Relay, Buzzer)
    action_app_init();

    // 3. Khởi tạo LCD
    lcd_app_init();

    // 4. Khởi tạo Mạng WiFi (Và Web Server)
    wifi_app_init();

    // Chờ 2s để module WiFi ổn định trước khi mở MQTT
    vTaskDelay(pdMS_TO_TICKS(2000));

    // 5. Khởi tạo MQTT Client
    mqtt_app_init();

    // 6. Khởi tạo Radar UART Task
    radar_app_init();

    ESP_LOGI(TAG, "System Initialization Completed.");
}