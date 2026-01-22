#ifndef ESP_NOW_H
#define ESP_NOW_H
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "main_car.h"

/*-----传输数据包区-----*/
// 数据包结构体(需要和发送端一致)
typedef struct message { // 用于遥控器的数据包
  bool isRunning; // 记录当前小车运行状态: true, 运行; false, 暂停
  int maxSpeed; // 电位器对应的最大速度
  int throttle; // 摇杆油门, 前后推力(Y轴)
  int steering; // 摇杆转向, 转向角度(X轴)
} message; // 遥控器信息

typedef struct message02 { // 用于接收上位机的json数据包
  String json;
} message02; // 上位机信息

struct message_test { // 用于发往测试机的数据包
  CarStatus car_status;
};
/*-------------------*/


/*-----外部变量-----*/
// 将全局变量放到.cpp文件内, 防止头文件被多次引用时申请多个同名变量
extern unsigned long lastSendTime;
extern message sender_data;
extern message receiver_data;
extern message_test sender_test;
extern message_test receiver_test;
/*-----------------*/


/*-----自定义函数区-----*/
void init_WiFi_ESP_NOW(); // WiFi 和 ESP-NOW初始化
void registerRemote(); // 遥控器端注册函数
void registerSmartCar(); // 小车端发送注册函数
void ondataSent(const uint8_t *address, esp_now_send_status_t status); // 发送回调函数, 检测发送状态
void ondataRecv(const uint8_t *mac, const uint8_t *incoming, int len); // 接收回调函数, 接收数据包信息并检测接收状态
void messageSend(message& sender_data, int currentSpeed, int currentX, int currentY, bool isRunning); // 遥控器发送数据包函数
void testMessageSend(message_test& sender_data); // 小车端发往测试接收端函数
void updateCarStatusFromRemote(); // 小车状态更新函数
/*-------------------*/

#endif