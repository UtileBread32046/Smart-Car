#include "ESP-NOW.h"

/*-----全局变量-----*/
// 小车 MAC 地址: 48:E7:29:15:84:68
// 上位机 MAC 地址: EC:64:C9:92:36:2C
// uint8_t: 无符号的8位整数类型 -> 可移植性：确保代码在不同平台(单片机)上行为一致
uint8_t SmartCarAddress[] = {0x48, 0xE7, 0x29, 0x15, 0x84, 0x68}; // 广播目的地址(小车的MAC地址)
uint8_t testReceiverAddress[] = {}; // 测试接收板的MAC地址
unsigned long lastSendTime = 0; // 记录上次发送数据包时间戳, 实现非阻塞控制
unsigned long lastSendTestTime = 0;
message sender_data;
message receiver_data = {true}; // 小车默认开启
message02 receiver02_data;
message_test sender_test;
message_test receiver_test;
/*-----------------*/


// WiFi 和 ESP-NOW初始化
void init_WiFi_ESP_NOW() {
  WiFi.mode(WIFI_STA); // 设置为WiFi Station模式
  // 初始化ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!"); // 检测是否成功
    return;
  }
}

// 遥控器端注册函数
void registerRemote() {
  // 设定发送配置
  esp_now_register_send_cb(ondataSent); // 注册发送回调函数
  esp_now_peer_info_t peerInfo = {}; // 注册通信对端(小车), 同时使用 = {} 初始化为0, 防止脏数据导致发送失败
  memcpy(peerInfo.peer_addr, SmartCarAddress, 6); // 将小车MAC地址传入通信对端结构体对象中
  peerInfo.channel = 0; // 设置WiFi通信通道, 发送方和接收方需匹配
  peerInfo.encrypt = false; // 禁用加密传输

  // 注册对端设备, 将配置好的对端设备信息(peerInfo)添加到ESP-NOW通信系统中
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("ESP-NOW add peer failed!"); // 检测是否成功
    return;
  }
}

// 小车端注册发送函数
void registerSmartCar() {
  // 设定发送配置
  esp_now_register_send_cb(ondataSent); // 注册发送回调函数
  esp_now_peer_info_t peerInfo = {}; // 注册通信对端(小车), 同时使用 = {} 初始化为0, 防止脏数据导致发送失败
  memcpy(peerInfo.peer_addr, testReceiverAddress, 6); // 将测试板的MAC地址传入通信对端结构体对象中
  peerInfo.channel = 1; // 设置WiFi通信通道, 发送方和接收方需匹配
  peerInfo.encrypt = false; // 禁用加密传输

  // 注册对端设备, 将配置好的对端设备信息(peerInfo)添加到ESP-NOW通信系统中
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("ESP-NOW add peer failed!"); // 检测是否成功
    return;
  }
}

// 发送回调函数, 检测发送状态
void ondataSent(const uint8_t *address, esp_now_send_status_t status) {
  Serial.print("检测发送状态: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "成功awa" : "失败qwq"); 
}


void messageSend(message& sender_data, int currentSpeed, int currentX, int currentY, bool isRunning) {
  // 数据包赋值
  sender_data.maxSpeed = currentSpeed;
  sender_data.steering = currentX;
  sender_data.throttle = currentY;
  sender_data.isRunning = isRunning;

  esp_err_t result = esp_now_send(SmartCarAddress, (uint8_t*)&sender_data, sizeof(sender_data)); // esp芯片发送数据包并记录结果
  lastSendTime = millis();
  if (result == ESP_OK) {
    Serial.printf("发送成功! (MAX速度:%d)\n", currentSpeed);
  } else {
    Serial.println("发送失败qwq");
  }
}

// 小车端发往测试接收端函数
void testMessageSend(message_test& sender_data) {
  sender_data.car_status = car_status;
  esp_err_t result = esp_now_send(testReceiverAddress, (uint8_t*)&sender_data, sizeof(sender_data));
  lastSendTestTime = millis();
}

// 小车状态更新函数
void updateCarStatusFromRemote() {
  // 将数据包中的数据提取出来
  car_status.maxSpeed = receiver_data.maxSpeed;
  car_status.isRunning = receiver_data.isRunning;
  // 差速控制: 左轮 = 油门+转向 / 右轮 = 油门-转向
  int leftBase = receiver_data.throttle + receiver_data.steering;
  int rightBase = receiver_data.throttle - receiver_data.steering;
  // 限幅, 防止计算结果超过设定的最大速度
  car_status.finalLeft = constrain(leftBase, -car_status.maxSpeed, car_status.maxSpeed);
  car_status.finalRight = constrain(rightBase, -car_status.maxSpeed, car_status.maxSpeed);
}