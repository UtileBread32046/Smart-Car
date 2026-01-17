#include <ArduinoJson.h> // 处理JSON库
#include <HardwareSerial.h> // 使用ESP32硬件串口
#include "MCP.h"
#include "main_car.h"

HardwareSerial MCP_Serial(2); // 使用串口2, 通过显示指定, 和USB串口0同时工作互不干扰
HardwareSerial BT_Serial(1); // 蓝牙串口1
const char* MY_NAME = "smart_car"; // 定义小车的名字字符串, 用于匹配指令

// MCP初始化
void init_MCP() {
  // SERIAL_8N1: 帧格式配置(数据位为8位, 无奇偶校验No parity, 1为停止位)
  // MCP_RX_PIN: 负责接收, 连接外部设备的发送端
  // MCP_TX_PIN: 负责发送, 连接外部设备的接收端
  MCP_Serial.begin(115200, SERIAL_8N1, MCP_RX_PIN, MCP_TX_PIN);
  BT_Serial.begin(9600, SERIAL_8N1, -1, BT_TX_PIN);
}

// 处理Json指令
String handleMCP() {
  if (MCP_Serial.available()) { // 当MCP串口可用时
    String line = MCP_Serial.readStringUntil('\n'); // 捕获: 以换行符\n作为末尾字符, 读取json信息; 在嵌入式开发串口通信中, 有"单行约定(Single-line Convention)", 即将整条指令在同一行发出, 末尾加上\n; 若发送停止, 会在超时timeout后将已接收的部分返回
    return line;
  }
}

// 运行读取到的json指令
void processCommand(String line) {
  StaticJsonDocument<512> doc; // 准备: 在栈Stack上分配<n>个字节大小的内存空间, 存储原始字符串和解析后的对象树(一种树状分层数据结构, 常用 Map/Dictionary:映射/字典 进行实现)
  DeserializationError error = deserializeJson(doc, line); // 反序列化: 扫描line字符串, 检查括号匹配和键值对, 将结果填入doc中

  if (!error) { // 校验: 检测返回码error, 是否存在非法错误
    // const char* targetName = doc["name"] | "all"; // 提取json中的名称字符串, 如果没有则默认为all
    // if (strcmp(targetName, MY_NAME) != 0) {
    //   return; // 不是当前小车的指令, 直接返回
    // }
    // // 当名称字符串匹配时, 才运行下列指令
    const char* cmd = doc["cmd"]; // 提取doc里键cmd所对应的值(指令字符串, 使用char*指针, 同时使用const进行保护)
    JsonObject params = doc["params"]; // 提取params对应的值(参数, 由于可能存在嵌套, 故使用JsonObject对象进行存储)

    if (strcmp(cmd, "move") == 0) { // 比较提取出的字符串和预先设置好的指令字符串; 使用strcmp比较char*指针指向的两个字符串(以\0结尾), 返回结果为0时表示两字符串一样大, 即完全匹配
      // 将json数据中指定字符串所对应的值提取到本地
      // "|": 由ArduinoJson库重载的运算符, 表示"或", 用于避免字段缺失/类型错误/空值处理所引发的错误
      int throttle = params["throttle"] | 0;
      int steering = params["steering"] | 0;

      // 差速控制
      car_status.finalLeft = throttle + steering;
      car_status.finalRight = throttle - steering;

      // 限幅
      car_status.finalLeft = constrain(car_status.finalLeft, -255, 255);
      car_status.finalRight = constrain(car_status.finalRight, -255, 255);

      // 驱动电机
      move(car_status.finalLeft, car_status.finalRight);
      Serial.printf("MCP指令: move (%d, %d)\n", throttle, steering);
    }
    else if (strcmp(cmd, "turn") == 0) {
      double targetAngle = params["turnAngle"] | 0.0;
      Serial.printf("旋转: %f°\n", targetAngle);
      targetAngle = -targetAngle; // 以顺时针为正, 此处调和为减号
      turnToTarget(targetAngle);
    }
    else if (strcmp(cmd, "run") == 0) {
      const char* direction = params["direction"] | "null"; // 方向
      const int time = params["time"] | 10; // 时间(单位:ms)
      int speed = params["speed"] | 50; // 速度
      speed = constrain(speed, 50, 255); // 设置速度上下限
      if (strcmp(direction, "forward") == 0) {
        car_status.finalLeft = speed;
        car_status.finalRight = speed;
      } else if (strcmp(direction, "backward") == 0) {
        car_status.finalLeft = -speed;
        car_status.finalRight = -speed;
      } else {
        return;
      }

      // Serial.printf("接收到run指令, 移动速度: (%d, %d)\n", car_status.finalLeft, car_status.finalRight);
      
      int startTime = millis();
      while (millis() - startTime < time) { // 读取当前时间作差, 判断是否到达设定时长
        move(car_status.finalLeft, car_status.finalRight);
        vTaskDelay(10 / portTICK_PERIOD_MS); // 使用vTaskDelay而非delay, 在FreeRTOS(Real-Time Operating System, 实时操作系统)下让出CPU, 防止一直占用核心; 同时防止看门狗WDT(Watchdog Timer)重启系统
      }
      move(0, 0); // 显式停止
    }
  }
}