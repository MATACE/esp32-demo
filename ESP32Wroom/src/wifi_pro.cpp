#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <BluetoothSerial.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <string>

#include "wifi_pro.h"

using std::string;

#define LED_CTR 1

typedef struct tp_vals {
    string name;
    unsigned int id;
}TP_VALS;

TP_VALS pic_val = {"touch one", 1};

static unsigned int mcont = 0;
static hw_timer_t *mtimer = NULL;

static BluetoothSerial SerialBT;

void ARDUINO_ISR_ATTR t0_hardware(void *args)
{
    TP_VALS *p = (TP_VALS *)args;
    Serial.printf("Get T0 Name %s, T0 Id %d\n", p->name.c_str(), p->id);
}

// 低功耗蓝牙
/*
Create a BLE server that, once we receive a connection, will send periodic notifications.
The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE" 
Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

The design of creating the BLE server is:
1. Create a BLE Server
2. Create a BLE Service
3. Create a BLE Characteristic on the Service
4. Create a BLE Descriptor on the characteristic
5. Start the service.
6. Start advertising.

In this example rxValue is the data received (only accessible inside that function).
And txValue is the data to be sent, in this example just a byte incremented every second. 
*/
static BLEServer *pServer = NULL;
static BLECharacteristic * pTxCharacteristic;
static bool deviceConnected = false;
static bool oldDeviceConnected = false;
static uint8_t txValue = 0;

//  低功耗蓝牙的UUID,相当于一个动作的key-value
#define SERVICE_UUID           "6130bf0a-829f-47af-b7e2-f13c31e0ed05" // UART service UUID
#define CHARACTERISTIC_UUID_RX "42024bb3-98d5-45f1-a4a2-81a1d99fe4b1"
#define CHARACTERISTIC_UUID_TX "6de2d66a-0285-46c2-a26b-7c30d3057a3f"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};

void free_task_one(void *data)
{
    while (1) {
        Serial.printf("Task one runing");
        delay(500);
    }
}

void free_task_two(void *data)
{
    while (1) {
        Serial.printf("Task two runing");
        delay(1000);
    }
}

void ARDUINO_ISR_ATTR  key_hardware(void)
{
    Serial.printf("Get Key Event.\n");
}

void ARDUINO_ISR_ATTR time_handle(void)
{
    Serial.printf("Timer Value:%d\n", mcont++);
    if (mcont == 10) {
        mcont = 0;
    }
}

// wifi硬件的setup函数
int esp_wifi_setup(void)
{
    // // 设置串口的波特率
    // Serial.begin(115200);
    // // 串口初始化delay
    // delay(1000);

    // 设置touch的触发函数
    // touchAttachInterruptArg(T0, t0_hardware, (void *)&pic_val, 50);

#if 0
    // 设置中断触发方式
    // 设置上拉
    pinMode(0, INPUT_PULLUP);
    // 设置触发回调函数
    attachInterrupt(0, key_hardware, FALLING);
#endif

#if 0
   /*
    *  LEDC Chan to Group/Channel/Timer Mapping
    ** ledc: 0  => Group: 0, Channel: 0, Timer: 0
    ** ledc: 1  => Group: 0, Channel: 1, Timer: 0
    ** ledc: 2  => Group: 0, Channel: 2, Timer: 1
    ** ledc: 3  => Group: 0, Channel: 3, Timer: 1
    ** ledc: 4  => Group: 0, Channel: 4, Timer: 2
    ** ledc: 5  => Group: 0, Channel: 5, Timer: 2
    ** ledc: 6  => Group: 0, Channel: 6, Timer: 3
    ** ledc: 7  => Group: 0, Channel: 7, Timer: 3
    ** ledc: 8  => Group: 1, Channel: 0, Timer: 0
    ** ledc: 9  => Group: 1, Channel: 1, Timer: 0
    ** ledc: 10 => Group: 1, Channel: 2, Timer: 1
    ** ledc: 11 => Group: 1, Channel: 3, Timer: 1
    ** ledc: 12 => Group: 1, Channel: 4, Timer: 2
    ** ledc: 13 => Group: 1, Channel: 5, Timer: 2
    ** ledc: 14 => Group: 1, Channel: 6, Timer: 3
    ** ledc: 15 => Group: 1, Channel: 7, Timer: 3
    */

    // 设置PWM
    /* 设置GPIO */
    pinMode(2, OUTPUT);
    /* 设置PWM ledc编号, PWM的频率, 计数寄存器的范围2^10 (1-20) */
    ledcSetup(3, 1000, 10);
    /* 绑定GPIO端口号到PWM ledc编号 */
    ledcAttachPin(2, 3);
    ledcWrite(3, 10);
#endif

#if 0
    // Esp32 Time 80Mhz
    // 初始化定时器硬件设置分频 80/80=1Mhz
    mtimer = timerBegin(3, 80, true);

    // 设置定时器函数
    timerAttachInterrupt(mtimer, &time_handle, true);
    
    // 初始化设置计数值
    timerAlarmWrite(mtimer, 1000000, true);

    // 使能定时器
    timerAlarmEnable(mtimer);

    // 设置Touch的触发周期 1ms
    // touchSetCycles(0x2000, 0x2000);
#endif

    // Serial.begin(115200);
    // while(!Serial){delay(100);}

    // // We start by connecting to a WiFi network


    // Serial.print("Connecting to ");

    // WiFi.begin("TYHT", "38063806");
    // unsigned int ret = 0;

    // while ((ret = WiFi.status()) != WL_CONNECTED) {
    //     delay(5000);
    //     Serial.printf("ret: %d\n", ret);
    // }

    // Serial.println("");
    // Serial.println("WiFi connected");
    // Serial.println("IP address: ");
    // Serial.println(WiFi.localIP());

#if 0 == LED_CTR
    //设置GPIO2
    pinMode(2, OUTPUT);
    
#endif


#if 0
    Serial.begin(115200);
    delay(100);

    // We start by connecting to a WiFi network
    unsigned int ret = 0;
    Serial.print("Connecting to ");

    WiFi.mode(WIFI_STA);
    WiFi.begin("ACE", "12345678");


    while ((ret = WiFi.status()) != WL_CONNECTED) {
        delay(2500);
        Serial.printf("ret: %d\n", ret);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    pinMode(2, OUTPUT);
    /* 设置PWM ledc编号, PWM的频率, 计数寄存器的范围2^10 (1-20) */
    ledcSetup(3, 1000, 10);
    /* 绑定GPIO端口号到PWM ledc编号 */
    ledcAttachPin(2, 3);
    ledcWrite(3, 0);
#endif

#if 0

    // 经典蓝牙使用
    Serial.begin(115200);
    delay(100);

    SerialBT.begin("HXQ_BT");
    SerialBT.setPin("1234");


#endif

#if 0
    // 低功耗蓝牙设置
      Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("UART BLE");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
										BLECharacteristic::PROPERTY_NOTIFY
									);
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
#endif

#if 0
    // Free Rtos创建任务
    Serial.begin(115200);
    delay(100);
    xTaskCreate(free_task_one, "task1", 4096, NULL, 1, NULL);
    xTaskCreate(free_task_two, "task2", 4096, NULL, 2, NULL);

#endif

#if 1
    // esp32 core创建任务
    Serial.begin(115200);
    delay(100);
    xTaskCreatePinnedToCore(free_task_one, "task1", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(free_task_two, "task2", 4096, NULL, 2, NULL, 1);

#endif 

    return 0;

}

// wifi工程任务函数
int esp_wifi_task(void)
{
#if 0 == LED_CTR
    digitalWrite(2, HIGH);
    delay(1000);
    digitalWrite(2, LOW);
    delay(1000);
#endif

#if 0
    // 读取esp touch中断
    Serial.printf("Touch Vlaue: T0 %d T1 %d T2 %d T3 %d T4 %d T5 %d T6 %d T7 %d T8 %d T9 %d \r\n",
    touchRead(T0), touchRead(T1), touchRead(T2), touchRead(T3),
    touchRead(T4), touchRead(T5), touchRead(T6), touchRead(T7),
    touchRead(T8), touchRead(T9));

#endif

#if 0
    for (unsigned int i = 0; i <= 1024;) {
        delay(300);
        if (i == 1000) {
            i = 0;
        }
        ledcWrite(3, i);
        i += 50;
    }
#endif

#if 0
    if (Serial.available()) {
        // 串口获取数据
        SerialBT.write(Serial.read());
    }
    if (SerialBT.available()) {
        // 蓝牙有数据
        Serial.write(SerialBT.read());
    }

#endif

#if 0
    // 向低功耗蓝牙连接设备一直发消息
    if (deviceConnected) {
        pTxCharacteristic->setValue(&txValue, 1);
        pTxCharacteristic->notify();
        txValue++;
		delay(100); // bluetooth stack will go into congestion, if too many packets are sent
	}

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
		// do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
#endif

#if 0
    while (1) {
        Serial.printf("Main Runing\r\n");
        delay(2000);
    }
    
#endif


    return 0;

}




