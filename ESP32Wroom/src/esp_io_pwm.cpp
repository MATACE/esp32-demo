#include <Arduino.h>
#include "esp_io_pwm.h"

#define IO_INTERUPT 0
#define IO_PWM 1
 
int esp_io_pwm_setup(void)
{
    // 设置串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

#if IO_INTERUPT

    // 设置PIN14
    pinMode(14, OUTPUT);
    digitalWrite(14, LOW);
    
    // 设置PIN12上拉
    pinMode(12, INPUT_PULLUP);
    // 设置PIN12回调函数
    attachInterrupt(12, [](){ Serial.println("interrput."); }, CHANGE);

    // detachInterrupt(12); //失能中断
#endif

#if IO_PWM
    // 0~15 0~7 80Mhz 8~151Mhz
    // 通道号 频率 占空比的范围(2^(0~20))
    ledcSetup(8, 1, 10); // 8通道 1频率 2^10 最大1023
    ledcAttachPin(14, 8);
    
    pinMode(12, INPUT_PULLDOWN);

    for (int i = 0; i < 5; i++){
        ledcWrite(8, 200 * i); //设置输出PWM
        for (int j = 0; j < 100; j++) {
            delay(10);
            Serial.println(digitalRead(12));
        }
    }
#endif

    return 0;
}

int esp_io_pwm_task(void)
{

#if IO_INTERUPT
    delay(1000);
    digitalWrite(14, 1 ^ digitalRead(14));
#endif

    return 0;
}