#include <WiFi.h>

#include "wifi_tcp.h"

#define TCP_CLIENT 0
#define WEB_CLIENT 0
#define TCP_SERVER 1

static const char *ssid = "ACE";          
static const char *password = "12345678";

static const char *host = "server.natappfree.cc";
static const char *web_host = "www.baidu.com";
// static const IPAddress server_ip(112, 74, 89, 58); // Tcp服务器ip地址
static unsigned int server_port = 39648;  // Tcp服务器端口

static WiFiClient client; // 客户端
static WiFiServer server; // 服务端

int wifi_tcp_setup(void)
{
    unsigned int ret = 0;

    // 设置串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    // 设置wifi的模式
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false); // 关闭wifi休眠
    WiFi.begin(ssid, password);
    
    while ((ret = WiFi.status()) != WL_CONNECTED) {
        delay(500);
        Serial.printf("WiFi connect failed errno %d\r\n", ret);
    }

    // 显示WiFi的ip地址
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

#if TCP_SERVER
    // 监听端口
    // 同一局域网下
    server.begin(52000);
#endif

    return 0;
}

int wifi_tcp_task(void)
{

#if TCP_CLIENT // TCP client
    // 访问服务器的目标和端口
    if (client.connect(host, server_port)) {
        // 向服务器发送数据
        client.print("Hello World!");
        while (client.connected() || client.available()) {
            // 保持连接
            if (client.available()) {
                // 客户端收到数据
                String data = client.readStringUntil('\r');
                Serial.print("data:");
                Serial.println(data);
            }
        }

        // 关闭连接
        Serial.println("Close");
        client.stop();
    } else {
        // 关闭连接
        client.stop();
    }
    delay(5000);
#endif

#if WEB_CLIENT   // WEB Client
    // 访问服务器的目标和端口
    Serial.println(web_host);
    if (client.connect(web_host, 80)) {
        // 向Web服务器发送http请求
        client.print(String("GET /") + " HTTP/1.1\r\n" +
                     "Host: " + web_host + "\r\n" +
                     "Connection: close\r\n" +
                     "\r\n");
        while (client.connected() || client.available()) {
            // 保持连接
            if (client.available()) {
                // 客户端收到数据
                String data = client.readStringUntil('\n');
                Serial.println(data);
            }
        }

        // 关闭连接
        Serial.println("Close");
        client.stop();
    } else {
        // 关闭连接
        client.stop();
    }
    delay(10000);
#endif

#if TCP_SERVER
    // 监听TCP Sever端口
    client = server.available();
    String recv_buf = "";
    if (client) {
        Serial.println("Client Connect.");
        // 有客户端连接
        while (client.connected()) {
            if (client.available()) {
                recv_buf = client.readStringUntil('\r');
                client.print(recv_buf);
                Serial.println("buf: " + recv_buf);
            }
        }
        client.stop();
    }
    
#endif

    return 0;
}



