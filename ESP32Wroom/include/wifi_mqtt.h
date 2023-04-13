#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

/**
 * @brief 通过WiFi连接MQTT服务器,使用natapp透传出公网
 *  参考https://blog.csdn.net/Aspiration_H/article/details/107429231
 * 
 */

int esp_mqtt_setup(void);
int esp_mqtt_task(void);


#endif // WIFI_MQTT_H
