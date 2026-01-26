#include "gyroscope.h"
#include "move.h"
#include "main_car.h"

MPU6050 mpu6050(Wire); // 创建传感器实例
// float targetAngle = 0; // 记录目标角度
// int turnSpeed = 80; // 设置旋转速度
// 引入外部变量, 记录当前左右轮速度

extern TaskHandle_t oled_handle; // 引入OLED屏幕任务句柄handle
static unsigned long lastAngleTime = 0; // 记录上次获取角度的时间

// 初始化函数
void init_gyroscope() {
  Wire.begin(I2C_SDA, I2C_SCL); // 初始化I2C
  mpu6050.begin(); // 启动传感器
  Serial.println("正在校准陀螺仪, 请保持静止...");
  mpu6050.calcGyroOffsets(true); // calculate gyroscope offsets: 计算陀螺仪偏移量; 启用自动校准(校准过程应保证小车静止)
  Serial.println("\n校准完成!");
}
// 返回当前角度函数, 供外部调用
double getCurrentAngle() {
  mpu6050.update();
  return mpu6050.getAngleZ();
}
// 移动指定旋转角度函数
void turnToTarget(double turnAngle) {
  if (abs(turnAngle) < 2) { // 当误差绝对值小于2时, 认为满足条件
    car_status.finalLeft = car_status.baseLeft;
    car_status.finalRight = car_status.baseRight;
    move(car_status.finalLeft, car_status.finalRight);
    return;
  }

  if (oled_handle != NULL) {
    vTaskSuspend(oled_handle); // 在操作I2C之前, 让OLED刷新任务暂停, 避免冲突
  }
  int Kp = 2; // 补偿速度比例系数
  int offsetSpeed = abs(turnAngle) * Kp; // offsetSpeed = error * 比例系数, 将旋转速度设置为误差的n倍, 使小车移动由快变慢
  offsetSpeed = constrain(offsetSpeed, 0, 255); // 设置偏移速度的上下限
  if (turnAngle > 0) { // 判断顺/逆时针旋转
    car_status.finalLeft = car_status.baseLeft - offsetSpeed;
    car_status.finalRight = car_status.baseRight + offsetSpeed;
  } else {
    car_status.finalLeft = car_status.baseLeft + offsetSpeed;
    car_status.finalRight = car_status.baseRight - offsetSpeed;
  }
  car_status.finalLeft = constrain(car_status.finalLeft, -255, 255);
  car_status.finalRight = constrain(car_status.finalRight, -255, 255);
  move(car_status.finalLeft, car_status.finalRight);

  if (oled_handle != NULL) {
    vTaskResume(oled_handle); // 陀螺仪处理完成, 恢复OLED刷新任务
  }
}