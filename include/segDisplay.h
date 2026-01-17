#ifndef SEG_DISPLAY_H
#define SEG_DISPLAY_H
#include <Arduino.h>

// 数码管引脚
const int SCLK = 2; // Serial Clock, 串行时钟(移位时钟)信号 -> C(Clock): 时钟信号
const int RCLK = 15; // Register Clock, 寄存器时钟信号 -> L(Latch): 锁存信号
const int DIO = 4; // Data Input Output, 数据输入输出 -> D(Data): 数据信号

// 数码管0~9编码表(共阳极, 低电平有效)
// 使用byte类型节省内存
const byte segCode[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90}; // 数字编号(此处将a~g,dp的二进制位换算成十六进制)
const byte posCode[] = {0x01, 0x02, 0x04, 0x08}; // 位选编码

// 初始化数码管
void init_segDisplay();
// 数码管显示函数
void displayNum(int num);

#endif