#include "app_lcd.h"
#include "shared_defs.h"
#include "lcd_i2c.h"
#include "driver/i2c.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "LCD_APP";

static void LCD_Task(void *pvParameters) {
    RadarEvent_t ev;
    int current_dist = 0;
    char state_str[15] = "TRONG";
    char buf[32];
    uint32_t keep_state_until = 0;
    uint32_t blink_timer = 0;
    bool show_warning = false;

    // Khởi tạo I2C
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_PIN,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    i2c_param_config(I2C_PORT, &i2c_conf);
    i2c_driver_install(I2C_PORT, i2c_conf.mode, 0, 0, 0);
    lcd_init(I2C_PORT);

    while (1) {
        uint32_t now = xTaskGetTickCount();

        while (xQueueReceive(lcd_queue, &ev, 0) == pdTRUE) {
            current_dist = ev.distance;
            if (ev.event == EVENT_IN) {
                strcpy(state_str, "VAO");
                keep_state_until = now + pdMS_TO_TICKS(1500);
            } else if (ev.event == EVENT_OUT) {
                strcpy(state_str, "RA");
                keep_state_until = now + pdMS_TO_TICKS(1500);
            } else if (ev.event == EVENT_STATIC_ALARM) {
                strcpy(state_str, "DUNG IM");
                keep_state_until = now + pdMS_TO_TICKS(1500);
            } else if (ev.event == EVENT_NONE && now > keep_state_until) {
                strcpy(state_str, current_dist == 0 ? "TRONG" : "CO NGUOI");
            }
        }

        if (now > keep_state_until) {
            strcpy(state_str, current_dist == 0 ? "TRONG" : "CO NGUOI");
        }

        // Toggle blink state every 2 seconds
        if (now - blink_timer > pdMS_TO_TICKS(2000)) {
            show_warning = !show_warning;
            blink_timer = now;
        }

        // Dòng 1: Trạng thái
        snprintf(buf, sizeof(buf), "Trang thai: %-8s", state_str);
        lcd_set_cursor(I2C_PORT, 0, 0);
        lcd_print(I2C_PORT, buf);

        // Dòng 2: Khoảng cách
        snprintf(buf, sizeof(buf), "Khoang cach:%3d cm ", current_dist);
        lcd_set_cursor(I2C_PORT, 0, 1);
        lcd_print(I2C_PORT, buf);

        // Dòng 3: Khách vào
        snprintf(buf, sizeof(buf), "Khach vao:  %-7d", g_count_in);
        lcd_set_cursor(I2C_PORT, 0, 2);
        lcd_print(I2C_PORT, buf);

        // Dòng 4: Hiển thị Khách ra HOẶC Cảnh báo
        char line4[21];
        if (show_warning && !g_wifi_connected) {
            // Lỗi WiFi (Chớp dòng cảnh báo)
            snprintf(line4, sizeof(line4), "AP IP: 192.168.4.1  ");
        } else if (show_warning && g_wifi_connected && !g_mqtt_connected) {
            // Rớt MQTT
            snprintf(line4, sizeof(line4), "IP:%-17.17s", g_device_ip);
        } else {
            // Bình thường
            snprintf(line4, sizeof(line4), "Khach ra:   %-7d", g_count_out);
        }
        
        lcd_set_cursor(I2C_PORT, 0, 3);
        lcd_print(I2C_PORT, line4);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void lcd_app_init(void) {
    xTaskCreatePinnedToCore(LCD_Task, "LCD_Task", 4096, NULL, 2, NULL, 0);
    ESP_LOGI(TAG, "LCD Task initialized.");
}
