#include <Arduino.h>
#include <Ticker.h>

#include "wifi_tick.h"

using std::bind;
using namespace std::placeholders;

// 记录时间的值
static unsigned long prev_time = 0; //ms
static const unsigned int inter_val = 1000; //时间间隔和获取的时间计数值关系
static unsigned long cur_time = 0;

// 定时器任务调度器
static Ticker ticker_1;
static Ticker ticker_2;
static Ticker ticker_3;

void ticker_1_callback_t(void)
{
    // 秒单位,周期任务
    Serial.println("Task one.");
}

void ticker_3_callback_t(int args)
{
    // 秒单位,周期任务
    Serial.println("Task Three.");
}

int wifi_tick_setup(void)
{
    // 设置串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    
    return 0;
}

int wifi_tick_task(void)
{

#if 0   // 返回值判断时间间隔
    // 时间间隔
    cur_time = millis();
    if (cur_time - prev_time >= inter_val) {
        prev_time = cur_time;
        Serial.println(millis());
    }
#endif

    // 系统定时任务调度器
    ticker_1.attach(2, ticker_1_callback_t);
    ticker_2.once(10, [](){ Serial.println("Task Two"); });
    ticker_3.attach(5, ticker_3_callback_t, 123);

    return 0;
}