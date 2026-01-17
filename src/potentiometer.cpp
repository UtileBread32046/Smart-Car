#include "potentiometer.h"

unsigned long lastReadPotentiometer = 0; // 记录上次读取电位器数据时间戳, 实现降低采样频率
// ADC(Analog to Digital Converter): 模数转换器, 将持续变化的模拟信号转换成离散的数字值
int lastStableADC = 0; // 存储上一次读到的认定为稳定的ADC值


// 初始化电位器
void init_potentiometer() {
  pinMode(pot, INPUT);
}
// 电位器读取
int readPotentiometer() {
  int rawADC = analogRead(pot); // 读取此时的电位器数值(raw: 原始, 未加工)
  lastStableADC = (3*lastStableADC + rawADC) >> 2; // 采用加权平均[(3*old+new)/4], 避免变化量过大造成数值抖动; 使用位运算>>代替除法/, 节省时间
  lastReadPotentiometer = millis(); // 更新读取时间戳
  return map(lastStableADC, 0, 4095, 0, 255); // 使用map(), 将电位器的0~4095数值映射到马达的0~255数值
}
