#include <Arduino.h>
#include <EEPROM.h>

#include "wifi_eep.h"

static const String ssid = "ESP32WiFi";
static const String password = "12345678";

static const int wifi_len = ssid.length() + password.length();

int esp_eeprom_setup(void)
{
    // 设置串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

#if 0  // 向epprom写入数据
    // 向epprom申请写入空间资源
    // Size-1 4096-1
    uint8_t data = 0;
    EEPROM.begin(wifi_len+1);
    for (unsigned int addr = 0, len = 0; addr < wifi_len+1; len++, addr++) {
        if (len < ssid.length()) {
            // 写入ssid的值
            data = ssid[len];
        } else {
            // 写入password的值
            data = password[len-ssid.length()];
        }
        // 写入数据
        EEPROM.write(addr, data);
    }
    // 刷新数据到EEPROM上
    EEPROM.commit();

    Serial.println("EPPROM End Write.");
#endif

#if 1   // 读取向epprom的数据
    // 向epprom申请读取空间资源
    // Size+1 4095+1
    String eeprom_data ="";
    EEPROM.begin(wifi_len+1);
    for (unsigned int addr = 0, len = 0; addr < wifi_len+1; len++, addr++) {
        eeprom_data += static_cast<char>(EEPROM.read(addr));
        delay(2);
    }
    
    Serial.println(eeprom_data.substring(0, ssid.length()));
    Serial.print(eeprom_data.substring(ssid.length(), ssid.length()+password.length()));
    

#endif

    return 0;
}

int esp_eeprom_task(void)
{

    return 0;
}