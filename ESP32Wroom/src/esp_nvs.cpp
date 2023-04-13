#include "esp_nvs.h"

int esp_nvs_setup(void)
{
    return 0;
}

int esp_nvs_task(void)
{
    return 0;
}

#if 0
// C++ MAP
#include <Preferences.h>

void setup() {
    Serial.begin(115200);
    Serial.println();
    delay(2000);

    Preferences prefs; // 声明Preferences对象
    prefs.begin("mynamespace"); // 打开命名空间mynamespace
    uint32_t count = prefs.getUInt("count", 0); // 获取当前命名空间中的键名为"count"的值
                                                // 如果没有该元素则返回默认值0
    count++; // 累加计数
    Serial.printf("这是系统第 %u 次启动\n", count);
    prefs.putUInt("count", count); // 将数据保存到当前命名空间的"count"键中
    prefs.end(); // 关闭当前命名空间

    delay(5000);
    ESP.restart(); // 重启系统
}

void loop() {}
#endif 

#if 0
// MAP查询
#include <Preferences.h>

void setup() {
    Serial.begin(115200);
    Serial.println();
    delay(2000);

    Preferences prefs;
    prefs.begin("mynamespace");
    if(prefs.isKey("naisu")) { // 如果当前命名空间中有键名为"naisu"的元素
        Serial.printf("naisu: %s\n\n", prefs.getString("naisu"));
        while (1) {}     
    } else {
        String naisu = prefs.getString("naisu", "555"); // 获取当前命名空间中的键名为"naisu"的值
                                                        // 如果没有该元素则返回默认值"555"
        Serial.printf("naisu: %s\n\n", naisu);
        prefs.putString("naisu", "233");
        prefs.end();
        delay(5000);
        ESP.restart();
    }
}

void loop() {}
#endif

#if 0
// 删除MAP
#include <Preferences.h>

void setup() {
    Serial.begin(115200);
    Serial.println();
    delay(2000);

    Preferences prefs;
    prefs.begin("mynamespace");

    prefs.putString("naisu", "233");
    Serial.printf("naisu: %s\n\n", prefs.getString("naisu", "not found"));
    prefs.remove("naisu"); // 删除当前命名空间中键名为"naisu"的元素
    Serial.printf("naisu: %s\n\n", prefs.getString("naisu", "not found"));

    prefs.putString("naisu", "233");
    Serial.printf("naisu: %s\n\n", prefs.getString("naisu", "not found"));
    prefs.clear(); // 删除当前命名空间中的所有元素
    Serial.printf("naisu: %s\n\n", prefs.getString("naisu", "not found"));

    prefs.end();
}

void loop() {}
#endif 

#if 0
// 获取剩下的MAP
#include <Preferences.h>

void setup() {
    Serial.begin(115200);
    Serial.println();
    delay(2000);

    Preferences prefs;
    prefs.begin("mynamespace");

    Serial.println(prefs.freeEntries());

    prefs.putString("string", "22333");
    Serial.println(prefs.freeEntries());

    prefs.putInt("int", 1234567890);
    Serial.println(prefs.freeEntries());

    prefs.putChar("char", 127);
    Serial.println(prefs.freeEntries());

    uint8_t buf[5] = {1, 2, 3, 4, 5};
    prefs.putBytes("byte", buf, 5);
    Serial.println(prefs.freeEntries());

    prefs.end();
}

void loop() {}
#endif