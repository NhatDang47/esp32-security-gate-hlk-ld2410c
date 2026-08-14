#include "app_action.h"
#include "shared_defs.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/task.h"

static const char *TAG = "ACTION_APP";

QueueHandle_t action_queue;
QueueHandle_t lcd_queue;
volatile int g_count_in = 0;
volatile int g_count_out = 0;

static void IRAM_ATTR gpio5_isr_handler(void *arg) {
    gpio_set_level(RELAY_PIN, 1);
}

static void Action_Task(void *pvParameters) {
    RadarEvent_t ev;
    while (1) {
        if (xQueueReceive(action_queue, &ev, portMAX_DELAY)) {
            // Bíp 200ms cho mọi sự kiện quan trọng
            if (ev.event == EVENT_IN || ev.event == EVENT_OUT || ev.event == EVENT_STATIC_ALARM) {
                gpio_set_level(BUZZER_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(200));
                gpio_set_level(BUZZER_PIN, 0);
            }
            // Thả relay khi mất người hoàn toàn
            if (ev.event == EVENT_NONE && ev.distance == 0) {
                gpio_set_level(RELAY_PIN, 0);
            }
        }
    }
}

void action_app_init(void) {
    action_queue = xQueueCreate(10, sizeof(RadarEvent_t));
    lcd_queue = xQueueCreate(10, sizeof(RadarEvent_t));
    
    // Cấu hình GPIO Output (Relay, Buzzer)
    gpio_config_t io_out_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << RELAY_PIN) | (1ULL << BUZZER_PIN),
    };
    gpio_config(&io_out_conf);
    gpio_set_level(RELAY_PIN, 0);
    gpio_set_level(BUZZER_PIN, 0);
    
    // Cấu hình GPIO Input (Radar IO )
    gpio_config_t io_in_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << RADAR_IO_PIN),
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    gpio_config(&io_in_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(RADAR_IO_PIN, gpio5_isr_handler, NULL);
    
    xTaskCreatePinnedToCore(Action_Task, "Action_Task", 3072, NULL, 5, NULL, 1);
    ESP_LOGI(TAG, "Action Task initialized.");
}
