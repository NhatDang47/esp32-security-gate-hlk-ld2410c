#ifndef APP_MQTT_H
#define APP_MQTT_H

void mqtt_app_init(void);
void mqtt_publish_event(const char *action, int distance, int energy, int count_in, int count_out);

#endif // APP_MQTT_H
