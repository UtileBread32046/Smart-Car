#include "move.h"
#include "main_car.h"

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

void differentialSpeedControl(double distance, int throttle, int steering) {
  // 差速控制: 左轮 = 油门+转向 / 右轮 = 油门-转向
  int leftBase = throttle + steering;
  int rightBase = throttle - steering;
  // 限幅, 防止计算结果超过设定的最大速度
  car_status.finalLeft = constrain(leftBase, -car_status.maxSpeed, car_status.maxSpeed);
  car_status.finalRight = constrain(rightBase, -car_status.maxSpeed, car_status.maxSpeed);

  // 运行逻辑
  if (car_status.isRunning) {
    if (distance < 20) { // 避障实现
      Serial.printf("避障...\n");
      move(car_status.maxSpeed, -car_status.maxSpeed); // 右转
      delay(500);
    } else {
      Serial.printf("移动...\n");
      move(car_status.finalLeft, car_status.finalRight);
      Serial.printf("当前速度: %d : %d\n", car_status.finalLeft, car_status.finalRight);
      delay(50); // 前进时加快响应速度
    }
  } else {
    move(0, 0);
    delay(100);
  }
}