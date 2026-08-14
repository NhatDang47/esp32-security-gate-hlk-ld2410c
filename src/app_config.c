#include "app_config.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "APP_CONFIG";

void config_init(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS...");
        nvs_flash_erase();
        nvs_flash_init();
    }
}

void config_load(AppConfig_t *cfg) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &handle);
    
    // Default values
    strcpy(cfg->wifi_ssid, "");
    strcpy(cfg->wifi_password, "");
    strcpy(cfg->mqtt_uri, "mqtt://192.168.1.3:1884");

    if (err == ESP_OK) {
        size_t len = MAX_SSID_LEN;
        if (nvs_get_str(handle, "ssid", cfg->wifi_ssid, &len) != ESP_OK) {
            strcpy(cfg->wifi_ssid, "");
        }
        
        len = MAX_PASS_LEN;
        if (nvs_get_str(handle, "pass", cfg->wifi_password, &len) != ESP_OK) {
            strcpy(cfg->wifi_password, "");
        }
        
        len = MAX_URI_LEN;
        if (nvs_get_str(handle, "mqtt_uri", cfg->mqtt_uri, &len) != ESP_OK) {
            strcpy(cfg->mqtt_uri, "mqtt://192.168.1.3:1884");
        }
        
        nvs_close(handle);
        ESP_LOGI(TAG, "Config loaded: SSID=%s, MQTT=%s", cfg->wifi_ssid, cfg->mqtt_uri);
    } else {
        ESP_LOGW(TAG, "NVS open failed, using default config");
    }
}

void config_save(const AppConfig_t *cfg) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        nvs_set_str(handle, "ssid", cfg->wifi_ssid);
        nvs_set_str(handle, "pass", cfg->wifi_password);
        nvs_set_str(handle, "mqtt_uri", cfg->mqtt_uri);
        nvs_commit(handle);
        nvs_close(handle);
        ESP_LOGI(TAG, "Config saved!");
    } else {
        ESP_LOGE(TAG, "Failed to open NVS for saving!");
    }
}
