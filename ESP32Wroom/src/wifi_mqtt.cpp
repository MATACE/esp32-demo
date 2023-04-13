#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "wifi_mqtt.h"

static const char *ssid = "ACE";          // Change this to your WiFi SSID
static const char *password = "12345678"; // Change this to your WiFi password

// 使用natapp进行透传
static const char *mqtt_server_ip = "server.natappfree.cc";    // MQTT的IP地址或者域名
static const int mqtt_server_port = 42169;                   // MQTT的IP地址的端口
static const char *mqtt_user = "admin";                      // MQTT用户名
static const char *mqtt_pwsd = "7075431h";                   // MQTT密码

static WiFiClient esp_clinet;
static PubSubClient mqtt_client(esp_clinet);

void mqtt_callback(char *topic, uint8_t* info, unsigned int length)
{
    Serial.printf("Topci: %s\r\n", topic);
    Serial.print("Info:");
    for (unsigned int i = 0; i < length; i++) {
        Serial.printf("%c", info[i]);
    }
    Serial.printf("\r\n");
    // 返回topic给pc
    mqtt_client.publish("/esp/data", "data from esp32.");
}

int esp_mqtt_setup(void)
{
    // 设置串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    
    // WiFi连接
    unsigned int ret = 0;
    WiFi.begin(ssid, password);
    while ((ret = WiFi.status()) != WL_CONNECTED) {
        delay(500);
        Serial.printf("WiFi connect failed errno %d\r\n", ret);
    }

    // 设置mqtt Client连接MQTT服务器
    mqtt_client.setServer(mqtt_server_ip, mqtt_server_port);
    // 设置mqtt Client回调函数
    mqtt_client.setCallback(mqtt_callback);
    while (!mqtt_client.connected()) {
        if (mqtt_client.connect("esq32_dev", mqtt_user, mqtt_pwsd)) {
            Serial.printf("Mqtt Connect successful.\r\n");
        } else {
            Serial.printf("Retry Connect. errno %d\r\n", mqtt_client.state());
            delay(2000);
        }
    }

    mqtt_client.subscribe("hello/good");
    mqtt_client.publish("/esp/data", "data from esp32.");

    return 0;
}

int esp_mqtt_task(void)
{
    mqtt_client.loop();
    return 0;
}


