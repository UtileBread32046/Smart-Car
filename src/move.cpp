#include "move.h"
#include "main_car.h"
#include "gyroscope.h"

static unsigned long lastMotorTime = 0; // 上次马达速度更新时间, 实现非阻塞控制
static unsigned long lastLockTime = 0; // 上一次闭环控制更新的时间
static unsigned long lastTrackTime = 0; // 上一次寻迹移动更新的时间

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
        car_status.finalLeft = car_status.baseLeft;
        car_status.finalRight = car_status.baseRight;
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
// 使用PD控制算法, 通过计算一个转向修正量, 叠加到基速度上, 实现更平滑的寻迹移动
double Kp = 40.0; // 比例增益系数
double Kd = 20.0; // 微分控制系数
static int last_error = 0; // 上一次的误差值
void trackLineMode() {
  if (millis() - lastTrackTime > 10) {
    int error = 0; // 本次偏差
    bool getLost = false; // 判断是否丢线

    // 进行误差映射(0为空地, 1为黑线)
    if (car_status.trackL == 1 && car_status.trackC == 0 && car_status.trackR == 0) error = -2;
    else if (car_status.trackL == 1 && car_status.trackC == 1 && car_status.trackR == 0) error = -1;
    else if (car_status.trackL == 0 && car_status.trackC == 1 && car_status.trackR == 0) error = 0;
    else if (car_status.trackL == 0 && car_status.trackC == 1 && car_status.trackR == 1) error = 1;
    else if (car_status.trackL == 0 && car_status.trackC == 0 && car_status.trackR == 1) error = 2;
    else if (car_status.trackL == 0 && car_status.trackC == 0 && car_status.trackR == 0) getLost = true;

    if (getLost) { // 丢线处理
      // 维持上一次的运动, 此处不做处理
    } else {
      double turnOffset = (Kp*error) + (Kd*(error-last_error)); // PD算法
      last_error = error; // 更新误差值
      // 计算最终速度
      int baseSpeed = car_status.maxSpeed;
      car_status.finalLeft = baseSpeed + turnOffset;
      car_status.finalRight = baseSpeed - turnOffset;
      // 别忘了限幅
      car_status.finalLeft = constrain(car_status.finalLeft, -255, 255);
      car_status.finalRight = constrain(car_status.finalRight, -255, 255);
    }
    move(car_status.finalLeft, car_status.finalRight); // 进行移动
    lastTrackTime = millis();
  }
}