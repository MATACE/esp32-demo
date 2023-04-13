#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include "wifi_sta.h"

using std::string;

// https://thingspeak.com/上传显示数据
static const char *ssid = "ACE";          // Change this to your WiFi SSID
static const char *password = "12345678"; // Change this to your WiFi password

static const char *host = "api.thingspeak.com";         // This should not be changed
static const int http_port = 80;                        // This should not be changed
static const String channel_id = "2092480";             // Change this to your channel ID
static const String write_api_key = "1YU0IGAIJERHNH9F"; // Change this to your Write API key
static const String read_api_key = "CGCM14UE2580UZ3E";  // Change this to your Read API key

// The default example accepts one data filed named "field1"
// For your own server you can ofcourse create more of them.
static int field1 = 0;

static int number_of_results = 2; // Number of results to be read
static int field_number = 1;      // Field number which will be read out

static void ReadResponse(WiFiClient *client)
{
    // 获取板子启动到现在执行的时间millis->ms  micros->us
    unsigned long timeout = millis();
    while (client->available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client->stop();
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    while (client->available()) {
        String line = client->readStringUntil('\r');
        Serial.print(line);
    }

    Serial.printf("\nClosing connection\n\n");
}

int wifi_sta_setup(void)
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

    return 0;
}

int wifi_sta_task(void)
{
    // 创建Wifi的client
    WiFiClient client;
    String footer = String(" HTTP/1.1\r\n") + "Host: " + String(host) + "\r\n" +
                    "Connection: close\r\n\r\n";
    // Write
    if (!client.connect(host, http_port)) {
        return -1;
    }

    client.print("GET https://api.thingspeak.com/update?api_key=" + write_api_key + "&field1=" + field1 + footer);
    ReadResponse(&client);

    // READ
    String read_request = "GET https://api.thingspeak.com/channels/" + channel_id + "/fields/" + field_number +
                          ".json?api_key=" + read_api_key + "&results=" + number_of_results +
                          footer;

    if (!client.connect(host, http_port)) {
        return -1;
    }

    client.print(read_request);
    ReadResponse(&client);

    ++field1;
    delay(10000);

    return 0;
}

#if 0
// 设置Wifi Sta的参数
/*
 Example of connection using Static IP
 by Evandro Luis Copercini
 Public domain - 2017
*/

#include <WiFi.h>

const char *ssid = "your_network_name";
const char *password = "your_network_password";
const char *host = "example.com";
const char *url = "/index.html";

IPAddress local_IP(192, 168, 31, 115);
IPAddress gateway(192, 168, 31, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

void setup()
{
    Serial.begin(115200);

    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
    {
        Serial.println("STA Failed to configure");
    }

    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("ESP Mac Address: ");
    Serial.println(WiFi.macAddress());
    Serial.print("Subnet Mask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("DNS: ");
    Serial.println(WiFi.dnsIP());
}

void loop()
{
    delay(5000);

    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort))
    {
        Serial.println("connection failed");
        return;
    }

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0)
    {
        if (millis() - timeout > 5000)
        {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    while (client.available())
    {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }

    Serial.println();
    Serial.println("closing connection");
}

#endif
