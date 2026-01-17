#ifndef MOVE_H
#define MOVE_H
#include <Arduino.h>

// 马达引脚
const int motorLeftDir = 13;
const int motorLeftSpd = 14;
const int motorRightDir = 15;
const int motorRightSpd = 16;


void init_motor();
// 小车移动函数实现
void move(int leftSpd, int rightSpd);
void differentialSpeedControl(double distance, int throttle, int steering); // 差速控制

#endif