#pragma once

// 三路红外寻迹传感器引脚
const int pin_L = 33;
const int pin_C = 32;
const int pin_R = 25;

// 初始化函数
void init_tracker();
// 状态更新函数
void update_tracker();