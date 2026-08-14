#ifndef SHARED_DEFS_H
#define SHARED_DEFS_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


// UART2 (Radar HLK-LD2410C)
#define UART_NUM       UART_NUM_2
#define TXD_PIN        17
#define RXD_PIN        16
#define RX_BUF_SIZE    1024

// I2C (LCD 20x4)
#define I2C_PORT       I2C_NUM_0
#define I2C_SDA_PIN    21
#define I2C_SCL_PIN    22

// Digital IO
#define RELAY_PIN      18   // Active High
#define BUZZER_PIN     14  // Active High
#define RADAR_IO_PIN   5 // High khi có người (chống kẹp)

// Cấu trúc dữ liệu
typedef enum {
    EVENT_NONE,
    EVENT_IN,
    EVENT_OUT,
    EVENT_STATIC_ALARM
} EventType_t;

typedef struct {
    EventType_t event;
    int distance;
    int energy;
    int count_in;
    int count_out;
} RadarEvent_t;

// Global Variables (Extern)
extern QueueHandle_t action_queue;
extern QueueHandle_t lcd_queue;

extern volatile int g_count_in;
extern volatile int g_count_out;

// Network Status
extern volatile bool g_wifi_connected;
extern volatile bool g_mqtt_connected;
extern volatile bool g_ap_mode_active;
extern char g_device_ip[20];

#endif // SHARED_DEFS_H
