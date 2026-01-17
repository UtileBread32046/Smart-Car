#include <Arduino.h>
#include "ESP-NOW.h"
#include "segDisplay.h"
#include "rocker.h"
#include "potentiometer.h"


/*----全局变量区----*/
int currentSpeed = 0; // 记录当前设定的小车速度
bool isRunning = true; // 默认为启动状态
extern message sender_data;
/*-------------------*/


/*-----初始化函数区-----*/
void setup() {
  Serial.begin(115200);
  init_potentiometer(); // 初始化电位器
  init_segDisplay(); // 初始化数码管
  init_rocker(); // 初始化摇杆
  init_WiFi_ESP_NOW(); // 初始化WiFi和ESP-NOW
  registerRemote(); // 注册遥控器端

  Serial.println("遥控器开始通讯...");
}
/*-------------------*/


/*-----循环函数区-----*/
void loop() {
  // 摇杆逻辑
  if (millis() - lastReadRocker > 20) {
    isRunning = readRocker(isRunning, currentSpeed); // 读取摇杆, 同时更新运行状态
  }
  // 电位器逻辑
  if (millis() - lastReadPotentiometer > 20) {
    currentSpeed = readPotentiometer();
  }
  // 数码管逻辑
  if (isRunning) {
    displayNum(currentSpeed); // 数码管显示速度
  } else {
    displayNum(0000);
  }
  // 发送数据包, 同时记录结果
  // 使用时间戳进行非阻塞控制, 从而实现并发
  if (millis() - lastSendTime > 500) { // millis(): millisenconds, 获取当前启动后的总时间
    messageSend(sender_data, currentSpeed, currentX, currentY, isRunning);
  }
  // delay(50); // 阻塞控制, 间隔50ms发送
}
/*-------------------*/