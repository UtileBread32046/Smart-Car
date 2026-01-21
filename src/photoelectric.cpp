#include <Arduino.h>
#include "photoelectric.h"
#include "main_car.h"

// 设置转换系数: 轮径67mm, 一周120脉冲 => (67 * 3.14159) / 120 = 1.754056
const double conversion = 1.754056;

// 静态变量, 用于记录状态和时间
static unsigned long lastCalcTime = 0;
static unsigned long intervalTime = 100; // (ms)
static bool lastA = true;
static bool lastB = true;

// 一个周期内累计的脉冲数
static long pulseSumA = 0;
static long pulseSumB = 0;


// 初始化函数
void init_photoelectric() {
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  lastA = digitalRead(pinA);
  lastB = digitalRead(pinB);
}

// 电位器处理函数
void processPhotoelectric() {
  // 1. 高频采样: 捕捉左右轮每一个跳变沿
  bool curA = digitalRead(pinA);
  bool curB = digitalRead(pinB);

  // 判断状态, 若不同则根据当前小车运动方向自增或自减
  if (curA != lastA) {
    if (car_status.finalLeft >= 0) {
      pulseSumA++;
    } else {
      pulseSumA--;
    }
    lastA = curA; // 更新状态
  }
  if (curB != lastB) {
    if (car_status.finalRight >= 0) {
      pulseSumB++;
    } else {
      pulseSumB--;
    }
    lastB = curB;
  }

  // 每100ms计算一次间隔时间速度
  if (millis() - lastCalcTime >= 100) {
    // 分别计算左右轮的真实物理速度 (mm/s)
    // 速度 = (脉冲数 * 单步距离) / 时间
    double vLeft = 1.0*pulseSumA * conversion * 1000 / intervalTime;
    double vRight = 1.0*pulseSumB * conversion * 1000 / intervalTime;

    // 重置计数器, 开启下一个周期
    pulseSumA = 0;
    pulseSumB = 0;

    // Serial.printf("当前计数:(%d,%d)\n", pulseSumL, pulseSumR);

    lastCalcTime = millis(); // 更新上次计算时间
  }
}