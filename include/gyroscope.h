#ifndef GYROSCOPE_H
#define GYROSCOPE_H
#include <MPU6050_tockn.h> // 引入陀螺仪专用处理库
#include <Wire.h> // 用于实现I2C通信协议

const int I2C_SDA = 21; // 数据引脚
const int I2C_SCL = 22; // 时钟引脚

extern MPU6050 mpu6050; // 创建传感器实例
// extern float targetAngle; // 记录目标角度
extern int turnSpeed; // 设置旋转速度

// 初始化函数
void init_gyroscope();
// 移动指定旋转角度函数
void turnToTarget();

#endif