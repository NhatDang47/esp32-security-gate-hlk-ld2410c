#include "app_radar.h"
#include "shared_defs.h"
#include "app_mqtt.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/task.h"

static const char *TAG = "RADAR_APP";

static void Radar_UART_Task(void *pvParameters) {
    uint8_t data[256];
    int state_machine = 0;
    int D_start = 0, D_last = 0;
    int Max_Energy = 0;
    
    while (1) {
        int len = uart_read_bytes(UART_NUM, data, sizeof(data), pdMS_TO_TICKS(50));
        if (len >= 15) {
            for (int i = 0; i <= len - 15; i++) {
                // Header check: 0xF4 0xF3 0xF2 0xF1
                if (data[i] != 0xF4 || data[i + 1] != 0xF3 || data[i + 2] != 0xF2 || data[i + 3] != 0xF1) {
                    continue;
                }
                
                uint8_t target_status = data[i + 8];
                int move_dist = data[i + 9] | (data[i + 10] << 8);
                uint8_t move_energy = data[i + 11];
                int static_dist = data[i + 12] | (data[i + 13] << 8);
                uint8_t static_energy = data[i + 14];
                
                bool has_static = (target_status & 0x02) != 0;
                bool has_moving = (target_status & 0x01) != 0;
                
                // --- Logic báo động mục tiêu tĩnh ---
                static uint32_t static_timer_start = 0;
                // Mục tiêu tĩnh phải có năng lượng > 65
                if (has_static && static_energy > 80) {
                    uint32_t now = xTaskGetTickCount();
                    if (static_timer_start == 0) {
                        static_timer_start = now;
                    } else if ((now - static_timer_start) > pdMS_TO_TICKS(10000)) {
                        static uint32_t last_alarm_send = 0;
                        if (now - last_alarm_send > pdMS_TO_TICKS(500)) {
                            RadarEvent_t ev_alarm = {
                                .event = EVENT_STATIC_ALARM,
                                .distance = static_dist,
                                .energy = static_energy,
                                .count_in = g_count_in,
                                .count_out = g_count_out
                            };
                            xQueueSend(action_queue, &ev_alarm, 0);
                            xQueueSend(lcd_queue, &ev_alarm, 0);
                            mqtt_publish_event("ALARM_STATIC", static_dist, static_energy, g_count_in, g_count_out);
                            last_alarm_send = now;
                        }
                    }
                } else {
                    static_timer_start = 0;
                }
                
                // --- State Machine Vào/Ra (Dựa vào mục tiêu động) ---
                if (!has_moving) {
                    // Trạng thái 3: Mất người di chuyển
                    if (state_machine != 0) {
                        int delta = D_start - D_last;
                        RadarEvent_t ev = {
                            .distance = D_last,
                            .energy = Max_Energy,
                            .count_in = g_count_in,
                            .count_out = g_count_out
                        };
                        
                        if (delta > 50 && Max_Energy > 80) {
                            ev.event = EVENT_IN;
                            g_count_in++;
                            ev.count_in = g_count_in;
                            mqtt_publish_event("VAO", D_last, Max_Energy, g_count_in, g_count_out);
                        } else if (delta < -50 && Max_Energy > 80) {
                            ev.event = EVENT_OUT;
                            g_count_out++;
                            ev.count_out = g_count_out;
                            mqtt_publish_event("RA", D_last, Max_Energy, g_count_in, g_count_out);
                        } else {
                            ev.event = EVENT_NONE;
                        }
                        
                        if (ev.event != EVENT_NONE) {
                            xQueueSend(action_queue, &ev, 0);
                            xQueueSend(lcd_queue, &ev, 0);
                        }
                    }
                    
                    state_machine = 0;
                    D_start = 0;
                    D_last = 0;
                    Max_Energy = 0;
                    
                } else {
                    // Có người di chuyển
                    if (state_machine == 0) {
                        state_machine = 1;
                        D_start = move_dist;
                        D_last = move_dist;
                        Max_Energy = move_energy;
                    } else {
                        state_machine = 2;
                        D_last = move_dist;
                        if (move_energy > Max_Energy) {
                            Max_Energy = move_energy;
                        }
                    }
                }
                
                // --- Xử lý LCD / Relay khi có hoặc mất người hoàn toàn ---
                static bool was_present = false;
                bool is_present = (target_status != 0);
                
                if (is_present) {
                    static uint32_t last_send_time = 0;
                    uint32_t now2 = xTaskGetTickCount();
                    if (now2 - last_send_time > pdMS_TO_TICKS(200)) {
                        int rep_dist = has_moving ? move_dist : static_dist;
                        int rep_energy = has_moving ? move_energy : static_energy;
                        RadarEvent_t ev = {EVENT_NONE, rep_dist, rep_energy, g_count_in, g_count_out};
                        xQueueSend(lcd_queue, &ev, 0);
                        last_send_time = now2;
                    }
                } else if (was_present) {
                    // Tín hiệu mất người hoàn toàn (Cả động và tĩnh)
                    RadarEvent_t ev_lost = {EVENT_NONE, 0, 0, g_count_in, g_count_out};
                    xQueueSend(action_queue, &ev_lost, 0);
                    xQueueSend(lcd_queue, &ev_lost, 0);
                }
                was_present = is_present;
                
                i += 10;
            }
        }
    }
}

void radar_app_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 256000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    
    xTaskCreatePinnedToCore(Radar_UART_Task, "Radar_UART", 6144, NULL, 10, NULL, 1);
    ESP_LOGI(TAG, "Radar UART Task initialized.");
}
