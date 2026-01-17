#include "gyroscope.h"
#include "move.h"
#include "main_car.h"

MPU6050 mpu6050(Wire); // 创建传感器实例
// float targetAngle = 0; // 记录目标角度
// int turnSpeed = 80; // 设置旋转速度
// 引入外部变量, 记录当前左右轮速度


// 初始化函数
void init_gyroscope() {
  Wire.begin(I2C_SDA, I2C_SCL); // 初始化I2C
  mpu6050.begin(); // 启动传感器
  Serial.println("正在校准陀螺仪, 请保持静止...");
  mpu6050.calcGyroOffsets(true); // calculate gyroscope offsets: 计算陀螺仪偏移量; 启用自动校准(校准过程应保证小车静止)
  Serial.println("\n校准完成!");
}
// 移动指定旋转角度函数
void turnToTarget(double turnAngle) {
  // 首先要获取旋转以前的角度参数, 并计算出绝对目标角度
  mpu6050.update();
  double startAngle = mpu6050.getAngleZ();
  double targetAngle = startAngle + turnAngle;

  while (true) { // 不断进行循环, 直到满足旋转条件
    mpu6050.update(); // 更新状态
    double currentAngle = mpu6050.getAngleZ(); // 获取旋转轴方向的角度
    double error = targetAngle - currentAngle; // 计算误差
    error = -error; // 以顺时针为正
    Serial.printf("当前误差为: %f\n", error);

    if (abs(error) < 2) { // 当误差绝对值小于2时, 认为满足条件
      move(0, 0); // 静止
      break;
    }

    int turnSpeed = abs(error) * 2; // turnSpeed = error * 比例系数, 将旋转速度设置为误差的n倍, 使小车移动由快变慢
    turnSpeed = constrain(turnSpeed, 40, 200); // 限制最小和最大转向速度; 设置下限: 防止电机速度太小导致无法移动, 设置一个速度下限
    if (error > 0) { // 判断顺/逆时针旋转
      car_status.finalLeft = turnSpeed;
      car_status.finalRight = -turnSpeed;
    } else {
      car_status.finalLeft = -turnSpeed;
      car_status.finalRight = turnSpeed;
    }
    move(car_status.finalLeft, car_status.finalRight);
    delay(10); // 为处理器处理和电机旋转预留时间
  }
}