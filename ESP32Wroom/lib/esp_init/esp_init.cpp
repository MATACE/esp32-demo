#include "esp_init.h"

/**
 * @brief Construct a new esp init::esp init object
 * 
 * @param name 
 */
ESP_INIT::ESP_INIT(string name)
    : name_(name)
{
    cout << name << " esp hardware init." << endl;
}

ESP_INIT::~ESP_INIT()
{

}

/**
 * @brief 设置esp硬件初始化函数
 * 
 * @param *op esp setup的初始化函数指针
 */
void ESP_INIT::set_hardware_setup(HARDWARE op)
{
    if (p_hardware_setup_ == nullptr && op != nullptr) {
        p_hardware_setup_ = op;
    }
    return;
}

/**
 * @brief 执行esp硬件初始化函数
 * 
 * @return int 
 */
int ESP_INIT::run_hardware_setup(void)
{
    if (p_hardware_setup_) {
        int ret = p_hardware_setup_();
        return ret;
    } else {
        return -1;
    }
    
}

void ESP_INIT::set_task(TASK task)
{
    if (p_task_ == nullptr && task != nullptr) {
        p_task_ = task;
    }
    return;
}

int ESP_INIT::run_task(void)
{
    if (p_task_) {
        int ret = p_task_();
        return ret;
    } else {
        return -1;
    }
}

ESP_INIT::ESP_INIT(const ESP_INIT& esp)
{
    name_ = esp.name_;
    p_hardware_setup_ = esp.p_hardware_setup_;
    p_task_ = esp.p_task_;
}

ESP_INIT::ESP_INIT(ESP_INIT&& esp) noexcept
{
    name_ = esp.name_;
    p_hardware_setup_ = esp.p_hardware_setup_;
    p_task_ = esp.p_task_;
    // 移动构造清除
    esp.name_ = "";
    esp.p_hardware_setup_ = nullptr;
    esp.p_task_ = nullptr;
}

ESP_INIT& ESP_INIT::operator=(const ESP_INIT& esp)
{
    if (this != &esp) {
        //有空间时释放当前的空间
        name_ = esp.name_;
        p_hardware_setup_ = esp.p_hardware_setup_;
        p_task_ = esp.p_task_;
    }
    return *this;
}

ESP_INIT& ESP_INIT::operator=(ESP_INIT&& esp) noexcept
{
    if (this != &esp) {
        //有空间时释放当前的空间
        name_ = esp.name_;
        p_hardware_setup_ = esp.p_hardware_setup_;
        p_task_ = esp.p_task_;
        // 移动构造清除
        esp.name_ = "";
        esp.p_hardware_setup_ = nullptr;
        esp.p_task_ = nullptr;
    }
    return *this;

}








