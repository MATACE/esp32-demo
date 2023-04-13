#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#include "wifi_ap.h"

#define LED_PIN 2 // Set the GPIO pin where you connected your test LED

// Set these to your desired credentials.
static const char *ssid = "ESP32WiFi";
static const char *password = "12345678";
static WiFiServer ap_server(80);
static String html_text = "<table border=\"1\" width=\"400\" cellspacing=\"0\" cellpadding=\"10\" align=\"center\">\
        <tr>\
            <th>name1</th>\
            <th>name2</th>\
            <th>name3</th>\
        </tr>\
        <tr>\
            <td>内容1</td>\
            <td>内容2</td>\
            <td>内容3</td>\
        </tr>\
    </table>";

int esp_ap_setup(void)
{   
    // 设置led pin
    pinMode(LED_PIN, OUTPUT);
    
    // 设置串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    // 打开Wifi AP
    WiFi.setSleep(false);
    if (!WiFi.softAP(ssid, password)) {
        log_e("Soft AP creation failed.");
        while(1);
    }

    // 获取AP的地址
    IPAddress my_ip = WiFi.softAPIP();
    Serial.print("Ap Ip Address:");
    Serial.println(my_ip);

    //注册WebSocket函数

    // 启动AP服务器
    ap_server.begin();

    return 0;
}

int esp_ap_task(void)
{
    WiFiClient client = ap_server.available();   // listen for incoming clients
    if (client) { // get a client
        Serial.println("New Client Connect");
        String client_data = "";            // make a String to hold incoming data from the client
        while (client.connected()) {        // loop while the client's connected
            if (client.available()) {       // if there's bytes to read from the client,
                char c = client.read();     // read a byte, then
                Serial.write(c);            // print it out the serial monitor
                if (c == '\n') {            // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (client_data.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();
                        // the content of the HTTP response follows the header:
                        client.print("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>Title</title></head><body>");
                        client.print("Click <a href=\"/H\"><input type=\"button\" value=\"LED ON\"></a> to turn ON the LED.<br>");
                        client.print("Click <a href=\"/L\"><input type=\"button\" value=\"LED OFF\"></a> to turn OFF the LED.<br>");
                        client.print(html_text);
                        client.print("</body></html>");
                        client.println();   // The HTTP response ends with another blank line:                     
                        break;  // break out of the while loop:
                    } else {    // if you got a newline, then clear currentLine:
                        client_data = "";
                    }
                } else if (c != '\r') {     // if you got anything else but a carriage return character,
                    client_data += c;       // add it to the end of the currentLine
                }
                String temp = client_data;
                // Check to see if the client request was "GET /H" or "GET /L":
                if (client_data.endsWith("GET /H")) {
                    digitalWrite(LED_PIN, HIGH);               // GET /H turns the LED on
                }
                if (client_data.endsWith("GET /L")) {
                    digitalWrite(LED_PIN, LOW);                // GET /L turns the LED off
                }
            }
        }
        // close the connection:
        client.stop();
        Serial.println("Client Disconnected.");
    }

    return 0;
}


