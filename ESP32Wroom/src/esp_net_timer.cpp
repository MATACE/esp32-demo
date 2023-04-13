#include <Arduino.h>
#include <WiFi.h>

#include "esp_net_timer.h"

static const char *ssid = "ACE";    //你的网络名称
static const char *password = "12345678"; //你的网络密码

static const char *ntp_server = "pool.ntp.org";
static const long gmt_offset_sec = 8 * 3600;
static const int day_light_offset_sec = 0;

void PrintLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%F %T %A"); // 格式化输出
}

int esp_net_timer_setup(void)
{
    unsigned int ret = 0;

    // 设置串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    // WiFi连接
    WiFi.begin(ssid, password);
    while ((ret = WiFi.status()) != WL_CONNECTED) {
        delay(500);
        Serial.printf("WiFi connect failed errno %d\r\n", ret);
    }

    // 显示WiFi的ip地址
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // 从网络时间服务器上获取并设置时间
    // 获取成功后芯片会使用RTC时钟保持时间的更新
    configTime(gmt_offset_sec, day_light_offset_sec, ntp_server);
    PrintLocalTime();

    // RTC自动更新时间
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi disconnected!");

    return 0;
}

int esp_net_timer_task(void)
{
    delay(1000);
    PrintLocalTime();
    return 0;
}