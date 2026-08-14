#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64
#define MAX_URI_LEN  128

typedef struct {
    char wifi_ssid[MAX_SSID_LEN];
    char wifi_password[MAX_PASS_LEN];
    char mqtt_uri[MAX_URI_LEN];
} AppConfig_t;

void config_init(void);
void config_load(AppConfig_t *cfg);
void config_save(const AppConfig_t *cfg);

#endif // APP_CONFIG_H
