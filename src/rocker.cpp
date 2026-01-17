#include "rocker.h"

int lastStableX = 0; // 记录上一次读取到的稳定摇杆X值
int lastStableY = 0; // 记录上一次读取到的稳定摇杆Y值
int currentX = 0; // 记录摇杆当前X轴状态
int currentY = 0; // 记录摇杆当前Y轴状态
int lastButtonStatus = LOW; // 记录上一次按钮状态
unsigned long lastReadRocker = 0; // 记录上一次读取摇杆的时间

// 初始化摇杆
void init_rocker() {
  pinMode(SW, INPUT_PULLUP); // 设置按钮的上拉电阻(实现松手时电平为HIGH, 按下时为LOW)
}

bool readRocker(bool isRunning, int currentSpeed) {
  // 双轴摇杆读取
  int rawX = analogRead(VRX);
  int rawY = analogRead(VRY);
  lastStableX = (3*lastStableX + rawX) >> 2;
  lastStableY = (3*lastStableY + rawY) >> 2;
  // 将摇杆数值映射到当前最大速度范围
  currentX = map(lastStableX, 0, 4095, -currentSpeed, currentSpeed); // 正负号确定转向方向
  currentY = map(lastStableY, 0, 4095, currentSpeed, -currentSpeed);
  // 设置摇杆"死区"
  if (abs(currentX) < 50) {
    currentX = 0; // 当X偏移量较小时强制归零, 防止抖动
  }
  if (abs(currentY) < 50) {
    currentY = 0;
  }

  // 摇杆按钮读取
  int currentButtonStatus = digitalRead(SW);
  // 检测下降沿(抬起->按下)
  if (currentButtonStatus == LOW && lastButtonStatus == HIGH) {
    isRunning = !isRunning; // 反转运行状态
    Serial.printf("当前状态:%s\n", isRunning ? "运行中..." : "已暂停...");
  }
  lastButtonStatus = currentButtonStatus;

  lastReadRocker = millis();
  return isRunning;
}