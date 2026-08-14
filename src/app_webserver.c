#include "app_webserver.h"
#include "app_config.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_system.h"
#include <string.h>
#include <ctype.h>

static const char *TAG = "WEB_SERVER";
static httpd_handle_t server = NULL;

static const char *html_page =
    "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<title>Cài Đặt Cổng An Ninh</title>"
    "<style>"
    "body{font-family:Arial,sans-serif;background-color:#f4f4f9;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;}"
    ".container{background:#fff;padding:20px;border-radius:10px;box-shadow:0 4px 6px rgba(0,0,0,0.1);width:100%;max-width:400px;}"
    "h2{text-align:center;color:#333;}"
    "label{display:block;margin-bottom:8px;font-weight:bold;color:#555;}"
    "input[type=\"text\"],input[type=\"password\"]{width:100%;padding:10px;margin-bottom:15px;border:1px solid #ccc;border-radius:5px;box-sizing:border-box;}"
    "input[type=\"submit\"]{width:100%;padding:12px;background-color:#007bff;border:none;border-radius:5px;color:#fff;font-size:16px;cursor:pointer;transition:background-color 0.3s;}"
    "input[type=\"submit\"]:hover{background-color:#0056b3;}"
    "</style></head><body>"
    "<div class=\"container\"><h2>⚙️ Cài Đặt Hệ Thống</h2>"
    "<form action=\"/save\" method=\"GET\">"
    "<label>Tên WiFi (SSID):</label>"
    "<input type=\"text\" name=\"ssid\" required>"
    "<label>Mật khẩu WiFi:</label>"
    "<input type=\"password\" name=\"pass\">"
    "<label>MQTT Broker URI:</label>"
    "<input type=\"text\" name=\"mqtt\" placeholder=\"mqtt://192.168.1.x:1884\" required>"
    "<input type=\"submit\" value=\"Lưu Cấu Hình & Khởi Động Lại\">"
    "</form></div></body></html>";

static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Utility to url-decode
static void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit((unsigned char)a) && isxdigit((unsigned char)b))) {
            if (a >= 'a') a -= 'a' - 'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';
            if (b >= 'a') b -= 'a' - 'A';
            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';
            *dst++ = 16 * a + b;
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

static esp_err_t save_get_handler(httpd_req_t *req) {
    char buf[512];
    char ssid_raw[MAX_SSID_LEN] = {0};
    char pass_raw[MAX_PASS_LEN] = {0};
    char mqtt_raw[MAX_URI_LEN] = {0};
    
    AppConfig_t cfg;
    config_load(&cfg); // Load current in case something is empty

    if (httpd_req_get_url_query_str(req, buf, sizeof(buf)) == ESP_OK) {
        if (httpd_query_key_value(buf, "ssid", ssid_raw, sizeof(ssid_raw)) == ESP_OK) {
            url_decode(cfg.wifi_ssid, ssid_raw);
        }
        if (httpd_query_key_value(buf, "pass", pass_raw, sizeof(pass_raw)) == ESP_OK) {
            url_decode(cfg.wifi_password, pass_raw);
        }
        if (httpd_query_key_value(buf, "mqtt", mqtt_raw, sizeof(mqtt_raw)) == ESP_OK) {
            url_decode(cfg.mqtt_uri, mqtt_raw);
        }

        config_save(&cfg);
        
        const char *resp = "<!DOCTYPE html><html><body style=\"text-align:center;font-family:sans-serif;margin-top:50px;\"><h2>Đã lưu cấu hình thành công!</h2><p>Hệ thống đang khởi động lại...</p></body></html>";
        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
        
        ESP_LOGI(TAG, "New Config Saved. Restarting in 2s...");
        vTaskDelay(pdMS_TO_TICKS(2000));
        esp_restart();
    }
    
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

static httpd_uri_t uri_root = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = root_get_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_save = {
    .uri      = "/save",
    .method   = HTTP_GET,
    .handler  = save_get_handler,
    .user_ctx = NULL
};

void webserver_start(void) {
    if (server == NULL) {
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        // Allow multiple connections if needed
        config.max_open_sockets = 5;
        
        ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
        if (httpd_start(&server, &config) == ESP_OK) {
            httpd_register_uri_handler(server, &uri_root);
            httpd_register_uri_handler(server, &uri_save);
            ESP_LOGI(TAG, "Web server started.");
        } else {
            ESP_LOGE(TAG, "Failed to start web server!");
        }
    }
}

void webserver_stop(void) {
    if (server) {
        httpd_stop(server);
        server = NULL;
    }
}
