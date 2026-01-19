#ifndef OPTICAL_H
#define OPTICAL_H
#include <Arduino.h>
#define FLOW_RX 16 // ESP32的接收端, 接光流传感器的输出接口TX
#define FLOW_TX 17 // ESP32的发送端, 接光流传感器的接收接口RX
// 使用UART(串口)协议, Universal Asynchronous Receiver/Transmitter: 通用异步收发器
// 没有时钟同步信号, 通过提前约定好的波特率进行通信

// 光流传感器数据结构体
struct FlowData {
  int16_t flow_x;            // X像素点累计时间内累加位移 (原弧度值radians*10000)
  int16_t flow_y;            // Y像素点累计时间内累加位移 (原弧度值radians*10000) 
  uint32_t integration_time; // 时间间隔(上一次到本次发送光流数据的累计时间) (微秒:us)
  uint16_t distance;         // 激光测距距离 (毫米:mm)
  uint8_t valid;             // 状态值, 判断光流数据是否可用
  uint8_t confidence;        // 激光测距的置信度
};

// 导入外部变量, 供其他文件调用和修改
extern FlowData current_flow;
// extern PositionVelocity pv_data;
extern HardwareSerial Optical_Serial; // 光流传感器所占用的串口

// 初始化函数
void init_optical();
// 放在loop()里的总处理函数
void processOptical();



// // 位置和速度结构体
// typedef struct PositionVelocity {
//   float x_pos = 0;         // X轴位置 (累计位移 cm)
//   float y_pos = 0;         // Y轴位置 (累计位移 cm) 
//   float z_pos = 0;         // Z轴位置 (当前高度 m)
//   float x_vel = 0;         // X轴速度 (cm/s)
//   float y_vel = 0;         // Y轴速度 (cm/s)
//   float z_vel = 0;         // Z轴速度 (m/s)
// } PositionVelocity;

#endif