#include "move.h"
#include "main_car.h"
#include "gyroscope.h"

unsigned long lastMotorTime = 0; // 上次马达速度更新时间, 实现非阻塞控制
static unsigned long lastLockTime = 0; // 上一次闭环控制更新的时间

void init_motor() {
  // 马达初始化
  pinMode(motorLeftSpd, OUTPUT);
  pinMode(motorLeftDir, OUTPUT);
  pinMode(motorRightSpd, OUTPUT);
  pinMode(motorRightDir, OUTPUT);
  digitalWrite(motorLeftSpd, LOW);
  digitalWrite(motorLeftDir, LOW);
  digitalWrite(motorRightSpd, LOW);
  digitalWrite(motorRightDir, LOW);
}

// 小车移动函数实现
void move(int leftSpd, int rightSpd) {
  // 传给PWM的值必须为正数
  if (leftSpd >= 0) {
    digitalWrite(motorLeftDir, HIGH);
    analogWrite(motorLeftSpd, leftSpd);
  } else {
    digitalWrite(motorLeftDir, LOW);
    analogWrite(motorLeftSpd, -leftSpd);
  }

  if (rightSpd >= 0) {
    digitalWrite(motorRightDir, LOW);
    analogWrite(motorRightSpd, rightSpd);
  } else {
    digitalWrite(motorRightDir, HIGH);
    analogWrite(motorRightSpd, -rightSpd);
  }
}

void differentialSpeedControl() {
  // 运行逻辑
  if (millis() - lastMotorTime > 100) { // 非阻塞控制
    if (car_status.isRunning) {
      if (car_status.distance < 20) { // 避障实现
        // Serial.printf("避障...\n");
        move(car_status.maxSpeed, -car_status.maxSpeed); // 右转
        // delay(500);
      } else {
        // Serial.printf("移动...\n");
        move(car_status.finalLeft, car_status.finalRight);
        // Serial.printf("当前速度: %d : %d\n", car_status.finalLeft, car_status.finalRight);
        // delay(50); // 前进时加快响应速度
      }
    } else {
      move(0, 0);
      // delay(100);
    }
    lastMotorTime = millis(); // 更新马达速度变更时间
  }
}

// 闭环控制模式
void lockAngleControl() {
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

// 寻迹模式
void trackingMode() {
  
}