#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <AsyncUDP.h>

#include "wifi_udp.h"

#if 0   // 同步的UDP

const char *ssid = "ACE";
const char *password = "12345678";

static WiFiUDP esp_udp;
unsigned int udp_port = 5025;

int esp_udp_setup(void)
{
    unsigned int ret = 0;

    // 初始化串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    // 设置Wifi
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(ssid, password);
    while ((ret = WiFi.status()) != WL_CONNECTED) {
        delay(500);
        Serial.printf("WiFi connect failed errno %d\r\n", ret);
    }
    Serial.print("Local IP: ");
    Serial.print(WiFi.localIP());

    // 开启UDP监听端口数据
    esp_udp.begin(udp_port);


    return 0;
}

int esp_udp_task(void)
{
    // 获取UDP包的长度
    unsigned int pack_len = esp_udp.parsePacket();
    char buf[pack_len] = {};
    if (pack_len) {
        esp_udp.read(buf, pack_len);
        Serial.println(buf);
        Serial.print("Client IP:");
        Serial.print(esp_udp.remoteIP());
        Serial.println("Client Port:");
        Serial.print(esp_udp.remotePort());

        //返回数据给udp
        esp_udp.beginPacket(esp_udp.remoteIP(), esp_udp.remotePort());
        esp_udp.write((const uint8_t*)buf, pack_len);
        esp_udp.endPacket();
    }


    return 0;
}

#endif

// 广播方式的异步UDP
static const char *ssid = "ACE";
static const char *password = "12345678";

static AsyncUDP esp_udp;
static unsigned int udp_port = 5025;

static unsigned int broad_cast_port = udp_port;
const char *broad_cast_data = "broadcat data";

// 广播回调函数
void BroadCastCall(AsyncUDPPacket& packet)
{
    //数据来源
    Serial.println(packet.isBroadcast() ? "Broadcast" : (packet.isMulticast() ? "Multicast" : "Unicast"));
    // 远端的IP和端口
    Serial.print("Remote IP: ");
    Serial.println(packet.remoteIP());
    Serial.print("Remote Port: ");
    Serial.println(packet.remotePort());
    // 本地的IP和端口
    Serial.print("Local IP: ");
    Serial.println(packet.localIP());
    Serial.print("Local Port: ");
    Serial.println(packet.localPort());
    Serial.print("Data Len: ");
    Serial.println(packet.length());
    Serial.print("Data: ");
    Serial.write(packet.data(), packet.length());
    Serial.println();

    packet.print("reply data");
    broad_cast_port = packet.remotePort();
}

int esp_udp_setup(void)
{   
    unsigned int ret = 0;

    // 初始化串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    // 设置Wifi
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(ssid, password);
    while ((ret = WiFi.status()) != WL_CONNECTED) {
        delay(500);
        Serial.printf("WiFi connect failed errno %d\r\n", ret);
    }
    Serial.print("Local IP: ");
    Serial.print(WiFi.localIP());

    // 设置UDP监听端口数据
    while (!esp_udp.listen(udp_port));
    
    // 注册广播接收数据回调函数
    esp_udp.onPacket(BroadCastCall);
    return 0;
}

int esp_udp_task(void)
{
    delay(5000);
    esp_udp.broadcastTo(broad_cast_data, broad_cast_port, TCPIP_ADAPTER_IF_STA); //可以使用该方法广播信息

    return 0;
}