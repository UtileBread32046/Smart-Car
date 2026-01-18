#ifndef MOVE_H
#define MOVE_H
#include <Arduino.h>

// 马达引脚
const int motorLeftDir = 26;
const int motorLeftSpd = 27;
const int motorRightDir = 13;
const int motorRightSpd = 14;


void init_motor(); // 马达初始化函数
void move(int leftSpd, int rightSpd); // 小车移动函数实现
void differentialSpeedControl(double distance, int throttle, int steering); // 差速控制

#endif