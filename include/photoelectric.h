#ifndef PHOTOELECTRIC_H
#define PHOTOELECTRIC_H

// 光电码盘
// 透光时, 传感器输出低电平0
// 遮挡时, 传感器输出高电平1

// 绑定引脚
const int pinA = 35; // 左轮
const int pinB = 34; // 右轮

// 光电码盘初始化函数
void init_photoelectric();
// 光电码盘数据更新函数
void processPhotoelectric();

#endif