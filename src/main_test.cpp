#include "ESP-NOW.h"

static unsigned long lastPrintTime = 0;
static unsigned long lastReceiveTime = 0;

// 小车状态打印函数
void printCarStatue() {
  if (millis() - lastPrintTime > 1000) {
    Serial.printf("=====当前小车状态=====\n");
    Serial.printf("运行状态: %s", receiver_test.car_status.isRunning ? "运行中...\n" : "休眠中...\n");
    Serial.printf("最大速度: %d\n", receiver_test.car_status.maxSpeed);
    Serial.printf("左轮速度: %d\n", receiver_test.car_status.finalLeft);
    Serial.printf("右轮速度: %d\n", receiver_test.car_status.finalRight);
    Serial.printf("距离前方: %.2fmm\n", receiver_test.car_status.distance);
    Serial.printf("X轴坐标: %.2fmm\n", receiver_test.car_status.posX);
    Serial.printf("Y轴坐标: %.2fmm\n", receiver_test.car_status.posY);
    Serial.printf("X轴速度: %.2fmm/s\n", receiver_test.car_status.speedX);
    Serial.printf("Y轴速度: %.2fmm/s\n", receiver_test.car_status.speedY);
    Serial.printf("======================\n\n");
    lastPrintTime = millis();
  }
}

// 接收回调函数, 接收数据包信息并检测接收状态
// ESP-NOW不可同时注册多个回调函数, 故使用统一的回调函数, 此处通过数据包长度进行区分
void testDataRecv(const uint8_t *mac, const uint8_t *incoming, int len) {
  if (len == sizeof(message_test)) {
    memcpy(&receiver_test, incoming, sizeof(receiver_test)); // 将接收到的数据包拷贝到本地
    lastReceiveTime = millis(); // 更新收到数据包的时间
  }
}

void init_esp_now() { // 初始化函数
  init_WiFi_ESP_NOW(); // 初始化WiFi和ESP-NOW
  esp_now_register_recv_cb(testDataRecv); // 注册接收回调函数
}

void setup() {
  init_esp_now(); // 进行初始化
}

void loop() {
  if (millis() - lastPrintTime > 1000) {
    printCarStatue();
    lastPrintTime = millis();
  }
}