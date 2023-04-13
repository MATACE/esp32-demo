#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

#include "esp_sys_file.h"

int esp_spiffs_setup(void)
{
    // 设置串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    //挂载文件系统
    if (SPIFFS.begin(true)) {
        Serial.println("SPIFFS system mount.");
    }

    //打开/建立 并写入数据
    File file = SPIFFS.open("/test.txt", FILE_WRITE);
    if (file) {
        Serial.println("Open/create the test.txt file in the root directory.");
    }

    char data[] = "hello world\r\n";
    file.write((uint8_t *)data, strlen(data));
    file.close();

    //重命名文件
    if (SPIFFS.rename("/test.txt", "/retest.txt")) {
        Serial.println("test.txt rename retest.txt");
    }

    //读取文件数据
    file = SPIFFS.open("/retest.txt", FILE_READ);
    if (file) {
        Serial.print("context data: ");
        while (file.available()) {
          Serial.print((char)file.read());
        }
    }

    //打印SPIFFS文件系统信息
    Serial.printf("SPIFFS total size %d Byte \n", SPIFFS.totalBytes());
    Serial.printf("SPIFFS use size %d Byte \n", SPIFFS.usedBytes());

    return 0;
}

int esp_spiffs_task(void)
{
    return 0;
}
