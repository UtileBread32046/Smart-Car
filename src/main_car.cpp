#include <Arduino.h>
#include "main_car.h"
#include "ESP-NOW.h"
#include "MCP.h"
#include "OLED.h"
#include "ultrasonic.h"
#include "move.h"
#include "gyroscope.h"


/*----全局变量区----*/
CarStatus car_status = {0, 0, 0, true}; // 存储小车全局信息
extern message receiver_data;
extern message02 receiver02_data;
String command;
double distance = 0.0; // 单位:cm
/*-------------------*/


/*-----自定义函数区-----*/
// 此处放一些不放在这里就容易出现变量丢失的函数
// 接收回调函数, 接收数据包信息并检测接收状态
void ondataRecv(const uint8_t *mac, const uint8_t *incoming, int len) {
  memcpy(&receiver_data, incoming, sizeof(receiver_data)); // 将接收到的数据包拷贝到本地
  car_status.maxSpeed = receiver_data.maxSpeed; // 将数据包中的数据提取出来
  // Serial.printf("数据包接收成功! 速度: %d\n", car_status.maxSpeed);
}

void ondataRecv02(const uint8_t *mac, const uint8_t *incoming, int len) {
  command = String((char*)incoming);
  Serial.printf("%s\n", command.c_str());
  Serial.printf("上位机数据包接收成功!\n");
}

// OLED后台任务, 使屏幕刷新逻辑在另外一个核心上运行, 保证一直开启
void OLEDTask(void * pvParameters) {
  while (1) { // 任务函数需要一个永不退出的死循环, 否则会因为找不到出口而崩溃(或需要手动调用vTaskDelete)
    // Serial.printf(car_status.isRunning ? "OLED显示运行中...\n" : "OLED显示休眠中...\n");
    screenDisplay(car_status.isRunning, distance, car_status.finalLeft, car_status.finalRight);
    vTaskDelay(100 / portTICK_PERIOD_MS); // 任务延迟, 释放CPU执行其他任务; portTICK_PERIOD_MS: 系统节拍(ms), 使当前任务进入阻塞(Blocked)状态
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
  esp_now_register_recv_cb(ondataRecv); // 注册接收回调函数
  esp_now_register_recv_cb(ondataRecv02); // 注册接收回调函数
  init_OLED(); // OLED屏幕初始化
  init_MCP(); // MCP初始化
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
}
/*-------------------*/


/*-----循环函数区-----*/
void loop() {
  distance = getDistance(); // 单位:cm
  Serial.printf("距离: %fcm\n", distance);
  car_status.isRunning = receiver_data.isRunning;
  // 差速控制小车运动
  differentialSpeedControl(distance, receiver_data.throttle, receiver_data.steering);

  // 接收上位机通过WiFi-NOW发来的指令
  // String command = receiver02_data.json;
  processCommand(command);
  command.clear();

  // 测试MCP
  if (MCP_Serial.available()) {
    String mcp_line = MCP_Serial.readStringUntil('\n');
    processCommand(mcp_line);
    delay(1000);
  }
  // 测试电脑指令
  if (Serial.available()) {
    String pc_line = Serial.readStringUntil('\n');
    processCommand(pc_line);
    delay(1000);
  }
  // 测试蓝牙指令
  if (BT_Serial.available()) {
    String bt_line = BT_Serial.readStringUntil('\n');
    processCommand(bt_line);
    delay(1000);
  }
}
/*-------------------*/