#ifndef ULTRASONIC_H
#define ULTRASONIC_H
#include <Arduino.h>

// 超声波引脚
const int trig = 5;
const int echo = 18;

// 超声波初始化
void init_ultrasonic();
// 超声波测距函数
double getDistance();

#endif