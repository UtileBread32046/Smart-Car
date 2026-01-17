#ifndef ROCKER_H
#define ROCKER_H
#include <Arduino.h>

// 摇杆引脚
const int VRX = 33;
const int VRY = 32;
const int SW = 25;

extern int lastStableX; // 记录上一次读取到的稳定摇杆X值
extern int lastStableY; // 记录上一次读取到的稳定摇杆Y值
extern int currentX; // 记录摇杆当前X轴状态
extern int currentY; // 记录摇杆当前Y轴状态
extern int lastButtonStatus; // 记录上一次按钮状态
extern unsigned long lastReadRocker; // 记录上一次读取摇杆的时间

// 初始化摇杆
void init_rocker();
// 读取摇杆信息
bool readRocker(bool isRunning, int currentSpeed);

#endif