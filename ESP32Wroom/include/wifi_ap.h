#ifndef WIFI_AP_H
#define WIFI_AP_H

/**
 * @brief ESP32通过网页控制LED
 * 
 *  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.
 *   Steps:
 * 1. Connect to the access point "yourAp"
 * 2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
 *    OR Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port
 *   
 * 
 * 
 */

int esp_ap_setup(void);
int esp_ap_task(void);


#endif // WIFI_AP_H

