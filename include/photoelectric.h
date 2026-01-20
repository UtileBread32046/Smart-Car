#ifndef PHOTOELECTRIC_H
#define PHOTOELECTRIC_H

// 光电码盘
// 透光时, 传感器输出低电平0
// 遮挡时, 传感器输出高电平1

// 绑定引脚
const int pinA = 34;
const int pinB = 35;

// 光电码盘初始化函数
void init_photoelectric();
// 光电码盘数据更新函数
void updatePhotoelectric();

#endif