#ifndef MCP_HANDLER_H
#define MCP_HANDLER_H
// 使用头文件, 方便main_car.cpp在setup()阶段进行初始化
#include <Arduino.h>

/*-----串口配置-----*/
#define MCP_RX_PIN 20 // receive  接收指令引脚
#define MCP_TX_PIN 19 // transmit 发送响应引脚
// 暂时不需要MCP串口和蓝牙串口, 故注释掉
// #define BT_TX_PIN 16 // 蓝牙发送引脚(esp32芯片接收) bluetooth receive
// extern HardwareSerial MCP_Serial; // 使用串口2, 通过显示指定, 和USB串口0同时工作互不干扰
// extern HardwareSerial BT_Serial; // 蓝牙串口1
/*-----------------*/


/*-----外部引用-----*/
// 导入外部.cpp中的函数
extern void move(int leftSpd, int rightSpd);
extern void turnToTarget(double targetAngle);
/*-----------------*/

// 初始化MCP通信串口
void init_MCP();
// // 检查并读取串口发来的json指令
// String handleMCP();
// 运行读取到的json指令
void processCommand(String line);


#endif