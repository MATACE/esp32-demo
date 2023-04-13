#include <Arduino.h>

#include "esp_init.h"
#include "esp_net_timer.h"

static ESP_INIT esp_pro("wifi");

void setup() {
    // put your setup code here, to run once:

    /// 设置esp硬件
    int ret = 0;
    esp_pro.set_hardware_setup(esp_net_timer_setup);
    ret = esp_pro.run_hardware_setup();
    if (ret < 0) {
        //TODO：输出错误
        Serial.printf("%s->%d esp run_hardware_setup failed.",__func__, __LINE__);
    }
    /// 设置esp工程任务
    esp_pro.set_task(esp_net_timer_task);
    
}

void loop() {
    // put your main code here, to run repeatedly:

    int ret = 0;
    ret = esp_pro.run_task();
    if (ret < 0) {
        //TODO：输出错误
        Serial.printf("%s->%d esp run_task failed.",__func__, __LINE__);
    }


#if 0
  Serial.println("Scanning...");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("Scan done!");
  if (n == 0) {
    Serial.println("No networks found.");
  } else {
    Serial.println();
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000);
#endif

}