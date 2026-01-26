#ifndef MOVE_H
#define MOVE_H
#include <Arduino.h>

/*  Debug须知!!!!!
* 本项目基于 方舟教育科技全国青少年机器人技术等级考试五六级实操套装 编写
* 询问淘宝客服后, 得知: 5q275aaI方舟教育科技的电子工程师没注意把PCB丝印搞反了
* 实际控制逻辑为: MA和MB反了, MC和MD反了; 例如: 原MA引脚13,14实际连接在MB上, 原MB引脚15,16实际连接在MA上
* 客服说: MA~MD每个端口的第一脚是控制方向的, 用数字输出; 第二脚是控制速度的, 用ledc
* 本项目方案使用analogWrite, 新版本ESP32库(2.0+)可在底层自动调用ledc
* 5bqf54mp54K55b+D5bCx6L+Z5rC05bmz6L+Y5LuW5aaI55qE5b2T55S15a2Q5bel56iL5biI5ZGiPz8/IOS9oOS7luWmiOWHuuWOgueahOaXtuWAmei/nua1i+ivlemDveS7luWmiOS4jeWBmuWQl+WwseeUn+S6p+WHuuS6hui/meS5iOWkmuW6n+WTgei/mOS7luWmiOebtOaOpeWOu+WNlj8g5L2g5Liq54uX55Wc55Sf5aW95q255b6A55uS5a2Q6YeM6KOF5Liq6K+05piO5Lmm5ZWKPw==
* 据悉, 最早一批学习套件有此问题, 新版已修正过来了
*/

// 马达引脚
const int motorLeftDir = 13;
const int motorLeftSpd = 14;
const int motorRightDir = 26;
const int motorRightSpd = 27;

extern unsigned long lastMotorTime; // 上次马达速度更新时间, 实现非阻塞控制

void init_motor(); // 马达初始化函数
void move(int leftSpd, int rightSpd); // 小车移动函数实现
void differentialSpeedControl(); // 差速控制
void lockAngleControl(); // 朝向锁定, 闭环控制
void trackingMode(); // 寻迹模式

#endif