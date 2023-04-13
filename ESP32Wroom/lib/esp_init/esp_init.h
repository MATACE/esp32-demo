#ifndef ESP_INIT_H
#define ESP_INIT_H

#include <iostream>

using std::string;
using std::cout;
using std::endl;

typedef int (*HARDWARE)(void);
typedef int (*TASK)(void);

/**
 * @brief esp工程硬件初始化函数和TASK函数
 * 
 */
class ESP_INIT
{
public:
    ESP_INIT(string name = "demo");
    virtual ~ESP_INIT();

    /// 硬件初始化函数
    void set_hardware_setup(HARDWARE op);
    int run_hardware_setup(void);

    /// 任务执行函数
    void set_task(TASK task);
    int run_task(void);

    /// 拷贝构造和移动构造
    ESP_INIT(const ESP_INIT& esp);
    ESP_INIT(ESP_INIT&& esp) noexcept;
    
    /// 拷贝构造和移动构造
    ESP_INIT& operator=(const ESP_INIT& esp);
    ESP_INIT& operator=(ESP_INIT&& esp) noexcept;


private:
    string name_;
    HARDWARE p_hardware_setup_ = nullptr;
    TASK p_task_ = nullptr;
     
};



#endif // ESP_INIT_H


