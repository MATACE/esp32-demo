#ifndef WIFI_STA_H
#define WIFI_STA_H

/**
 * @brief eps32建立Wifi Sta连接网络通过get方式向服务器https://thingspeak.com/上传显示数据
 *        https的get用法
 * 
 */

int wifi_sta_setup(void);
int wifi_sta_task(void);


#endif // WIFI_STA_H

