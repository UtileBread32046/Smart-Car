#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H
#include <Arduino.h>

// 电位器引脚
// ADC(Analog to Digital Converter): 模数转换器, 将持续变化的模拟信号转换成离散的数字值
// ESP32内部有两个模数转换器(ADC1, ADC2), 由于WiFi模块共用, 必须绑到ADC1上, 防止analogRead()引发冲突
// ADC1: GPIO32~39 ; ADC2: GPIO0,2,4,12~15,25~27 ; GPIO: General Purpose Input/Output, 通用输入输出 
const int pot = 34; // pot(potentiometer): 电位器
extern unsigned long lastReadPotentiometer; // 记录上次读取电位器数据时间戳, 实现降低采样频率
extern int lastStableADC; // 存储上一次读到的认定为稳定的ADC值

// 初始化电位器
void init_potentiometer();
// 电位器读取
int readPotentiometer();

#endif