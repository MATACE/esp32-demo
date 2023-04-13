#include <Arduino.h>
#include "esp_adc_dac.h"

int esp_adc_dac_setup(void)
{
    // 设置串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    // 通过导线将IO26和IO27连接起来
    // IO 25 26 DAC输出
    // value 0~255
    //IO26 DAC输出 100 * 3.3V / 255≈1.294V
    dacWrite(26, 100);
    int vtmp = analogRead(27); //IO27 ADC获取电压

    Serial.printf("adc value: %d\n", vtmp);
    Serial.printf("adc ec: %.3fV\n", vtmp * 3.9 / 4095);

    return 0;
}

int esp_adc_dac_task(void)
{
    return 0;
}