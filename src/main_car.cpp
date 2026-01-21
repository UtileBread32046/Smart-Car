#include <Arduino.h>
#include "main_car.h"
#include "ESP-NOW.h"
#include "MCP.h"
#include "OLED.h"
#include "ultrasonic.h"
#include "move.h"
#include "gyroscope.h"
#include "optical.h"
#include "photoelectric.h"


/*----全局变量区----*/
CarStatus car_status = {true, false, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 存储小车全局信息
extern message receiver_data;
extern message02 receiver02_data;
String command;
static unsigned long lastStatusTime = 0; // 上次小车状态打印时间
static unsigned long lastPCCommand = 0; // 上一次读取电脑终端指令时间
static unsigned long lastLockTime = 0; // 上一次闭环控制更新的时间
static unsigned long lastRemoteTime = 0; // 上一次收到遥控器数据包的时间
static unsigned long lastRemote02Time = 0; // 上一次收到上位机数据包的时间
/*-------------------*/


/*-----自定义函数区-----*/
// 此处放一些不放在这里就容易出现变量丢失的函数

// 小车状态打印函数
void printCarStatue() {
  if (millis() - lastStatusTime > 1000) {
    Serial.printf("=====当前小车状态=====\n");
    Serial.printf("运行状态: %s", car_status.isRunning ? "运行中...\n" : "休眠中...\n");
    Serial.printf("最大速度: %d\n", car_status.maxSpeed);
    Serial.printf("左轮速度: %d\n", car_status.finalLeft);
    Serial.printf("右轮速度: %d\n", car_status.finalRight);
    Serial.printf("距离前方: %.2fmm\n", car_status.distance);
    Serial.printf("X轴坐标: %.2fmm\n", car_status.posX);
    Serial.printf("Y轴坐标: %.2fmm\n", car_status.posY);
    Serial.printf("X轴速度: %.2fmm/s\n", car_status.speedX);
    Serial.printf("Y轴速度: %.2fmm/s\n", car_status.speedY);
    Serial.printf("======================\n\n");
    lastStatusTime = millis();
  }
}

// 接收回调函数, 接收数据包信息并检测接收状态
// ESP-NOW不可同时注册多个回调函数, 故使用统一的回调函数, 此处通过数据包长度进行区分
void ondataRecv_Unified(const uint8_t *mac, const uint8_t *incoming, int len) {
  if (len == sizeof(message)) {
    memcpy(&receiver_data, incoming, sizeof(receiver_data)); // 将接收到的数据包拷贝到本地
    // Serial.printf("数据包接收成功! 速度: %d\n", car_status.maxSpeed);
    lastRemoteTime = millis(); // 更新收到数据包的时间
  }
  // else if (len == sizeof(String)) {
  else {
    command = String((char*)incoming);
    Serial.printf("%s\n", command.c_str());
    Serial.printf("上位机数据包接收成功!\n");
    lastRemote02Time = millis(); // 更新收到数据包的时间
  }
}

// OLED后台任务, 使屏幕刷新逻辑在另外一个核心上运行, 保证一直开启
void OLEDTask(void * pvParameters) {
  while (1) { // 任务函数需要一个永不退出的死循环, 否则会因为找不到出口而崩溃(或需要手动调用vTaskDelete)
    // Serial.printf(car_status.isRunning ? "OLED显示运行中...\n" : "OLED显示休眠中...\n");
    screenDisplay(car_status.isRunning, car_status.distance, car_status.finalLeft, car_status.finalRight);
    vTaskDelay(100 / portTICK_PERIOD_MS); // 任务延迟, 释放CPU执行其他任务; portTICK_PERIOD_MS: 系统节拍(ms), 使当前任务进入阻塞(Blocked)状态
  }
}

// 光流传感器后台任务
void opticalTask(void *pvParameters) {
  while (1) {
    processOptical();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
/*-------------------*/


/*-----初始化函数区-----*/
void setup() {
  Serial.begin(115200);
  init_gyroscope(); // 陀螺仪初始化
  init_motor(); // 马达初始化
  init_ultrasonic(); // 超声波初始化
  init_WiFi_ESP_NOW(); // WiFi 和 ESP-NOW初始化
  esp_now_register_recv_cb(ondataRecv_Unified); // 注册统一接收回调函数
  init_OLED(); // OLED屏幕初始化
  init_MCP(); // MCP初始化
  init_optical(); // 光流传感器初始化
  init_photoelectric(); // 光电码盘初始化
  Serial.println("小车, 启动!");

  // 使用双核进行多任务处理
  xTaskCreate(
    OLEDTask,      // 1. 任务函数名: 该任务要执行哪段代码
    "OLED_Task",   // 2. 任务名称: 方便调试，随便起
    4096,          // 3. 堆栈大小: 分配给这个任务的 RAM 空间(单位: 字节)
    NULL,          // 4. 参数: 传递给任务的参数，通常为 NULL
    1,             // 5. 优先级: 数字越大优先级越高
    NULL           // 6. 任务句柄: 用于以后删除或暂停任务，不用就填 NULL
  );

  xTaskCreatePinnedToCore(
    opticalTask,
    "optical_Task",
    4096,
    NULL,
    3,
    NULL,
    0                // Core ID: 将任务绑定到核心Core 0
  );
}
/*-------------------*/


/*-----循环函数区-----*/
void loop() {
  // 超声波传感器测距(单位:cm)
  getDistance();
  // 光流传感器进行工作
  processOptical();
  // 光电码盘进行工作
  processPhotoelectric();

  if (millis() - lastRemoteTime < 500) { // 当且仅当遥控器在线, 很快接收到数据包时, 才提取数据包中的状态
    updateCarStatusFromRemote(); // 更新小车状态
    // 差速控制小车运动
    differentialSpeedControl();
  } else if (millis() - lastRemote02Time < 5000) { // 当上位机在线时
    // 接收上位机通过WiFi-NOW发来的指令
    processCommand(command);
    command.clear();
  } else { // 当遥控器和上位机均离线时, 接收来自电脑端的指令
    // 测试电脑指令
    if (Serial.available()) {
      if (millis() - lastPCCommand > 1000) {
        String pc_line = Serial.readStringUntil('\n');
        processCommand(pc_line);
        lastPCCommand = millis();
      }
    }
  }

  // 闭环控制小车, 保持走直线
  if (car_status.angleLock) {
    if (millis() - lastLockTime > 10) {
      mpu6050.update();
      double currentAngle = mpu6050.getAngleZ();
      double error = car_status.lockAngle - currentAngle;
      if (abs(error) > 2) {
        turnToTarget(error);
      }
      lastLockTime = millis();
    }
  }

  // 打印小车状态
  // printCarStatue();

  // // 测试MCP
  // if (MCP_Serial.available()) {
  //   String mcp_line = MCP_Serial.readStringUntil('\n');
  //   processCommand(mcp_line);
  //   delay(1000);
  // }

  // // 测试蓝牙指令
  // if (BT_Serial.available()) {
  //   String bt_line = BT_Serial.readStringUntil('\n');
  //   processCommand(bt_line);
  //   delay(1000);
  // }
}
/*-------------------*/