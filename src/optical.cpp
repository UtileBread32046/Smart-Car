#include "optical.h"
#include "main_car.h"

/*-----全局变量-----*/
FlowData current_flow; // 存储当前光流数据包
// PositionVelocity pv_data; // 存储当前位置与速度数据
HardwareSerial Optical_Serial(2); // 使用串口2接收来自光流传感器的数据
double dx_optical = 0; // 一段时间内光流传感器测出的位移

// 有限状态机变量, 封装在cpp文件中, 避免污染全局命名空间
enum ParseState {
  WAIT_HEADER, // 0: 等待头部, 接收数据包的起始标识(帧头)
  WAIT_LENGTH, // 1: 等待长度, 接收后续长度字段
  PARSE_DATA   // 2: 解析数据, 按照指定长度解析数据内容
};

// 使用静态变量static, 使变量仅在该cpp文件中可见, 防止与其他文件变量名冲突和被外部修改
static ParseState state = WAIT_HEADER; // 设置初始状态
// 串口通讯(UART)中, 数据是以流式运输, 按字节进入内存; 1.3.1.1 光流+TOF版本协议
static uint8_t buffer[14]; // 设置一定大小的连续内存空间, 用于存放从光流传感器中接收到的原始数据
static uint8_t data_index = 0; // 数据计数器, 指向buffer中下一个存放数据的位置; 初始状态下为0, 表示缓冲区为空

// 用于稳定数据的变量
int16_t last_stable_flow_x = 0;
int16_t last_stable_flow_y = 0;
uint16_t last_stable_distance = 0;
/*------------------*/


/*-----内部函数声明-----*/
static bool verifyChecksum(); // 数据检查函数, 进行校验和检验
static bool receiveData(); // 数据接收函数, 将数据流按位存储在缓冲区
static void restoreData(); // 数据拼装函数, 进行字节拼接, 将单独字节拼回完整的数据
static void calculatePV(); // 计算位置和速度函数, 进行单位换算和积分运算
/*--------------------*/


/*-----函数实现区-----*/
// 初始化函数
void init_optical() {
  Optical_Serial.begin(115200, SERIAL_8N1, FLOW_RX, FLOW_TX); // 使用Optical_Serial, 传感器与ESP32进行串口通信
  // Serial.println("光流传感器正在运行中..."); // Serial为芯片与电脑端的串口
}
// 放在loop()里的总处理函数
void processOptical() {
  // 解析传感器数据
  while (receiveData()) { // 当接收到完整的数据流时
    calculatePV(); // 计算
  } 
  // else {
  //   // Serial.println("[Error]光流传感器数据接收异常(○´･д･)ﾉ");
  // }
}


// 接收数据流函数
bool receiveData() {
  while (Optical_Serial.available()) { // 当串口有数据传入时
    uint8_t ch = Optical_Serial.read(); // 每次读取一个字节
    // Serial.printf("当前接收到的数据为: ");
    // Serial.println(ch);
    switch (state) { // 判断当前状态
      case WAIT_HEADER: // 等待数据包头
        if (ch == 0xFE) { // 当前读取字节为帧头(0xFE)
          buffer[0] = ch; // 将帧头存入缓冲区第一位
          data_index = 1; // 数据索引指向下一位置
          state = WAIT_LENGTH; // 进入下一状态(等待长度)
        }
        break;
        
      case WAIT_LENGTH: // 等待数据包长度
        if (ch == 0x0A) { // 当前读取字节为数据包字节数(0x0A)
          buffer[1] = ch; // 存入缓冲区[1]位置
          data_index = 2; // 继续移动索引
          state = PARSE_DATA; // 进入下一状态(等待数据)
        } else {
          state = WAIT_HEADER; // 如果不是长度字节, 退回到等待包头状态
        }
        break;
        
      case PARSE_DATA: // 等待后续数据
        buffer[data_index++] = ch; // 数据索引不断累加存入后续数据
        
        if (data_index >= 14) { // 当超出缓冲区大小时
          if (buffer[13] == 0x55 && verifyChecksum()) { // 如果最后一个字节恰为包尾结束标识(0x55), 并且由第[13]位的校验值检查无误
            restoreData(); // 执行"拆包"操作, 将字节变为数字, 还原数据
            state = WAIT_HEADER; // 状态回到等待包头阶段
            return true; // 成功解析一帧数据, 返回true
          }
          state = WAIT_HEADER; // 数据包未结束或校验失败, 回到等待包头状态重新接收
        }
        break;
    }
  }
  return false; // 接收/解析失败, 返回false
}

// 校验和检验函数
bool verifyChecksum() {
  uint8_t checksum = 0;
  // 算法: 第3~12字节进行异或([2]~[11]存储的字节)
  for (int i = 2; i < 12; i++) {
    checksum ^= buffer[i];
  }
  return checksum == buffer[12]; // 与校验值进行比较, 返回结果
}

// 数据拼装函数
void restoreData() {
  // 根据文档说明, 前一位为数据的低字节, 后一位为高字节
  // 拼接方法: [3]位置的字节左移8位(高字节), 与[2]位置的字节进行按位或, 拼接成一个完整的16位整数
  current_flow.flow_x = (int16_t)((buffer[3] << 8) | buffer[2]);
  current_flow.flow_y = (int16_t)((buffer[5] << 8) | buffer[4]);
  current_flow.integration_time = (uint16_t)((buffer[7] << 8) | buffer[6]);
  current_flow.distance = (uint16_t)((buffer[9] << 8) | buffer[8]);
  current_flow.valid = buffer[10];
  current_flow.confidence = buffer[11];
  // Serial.printf("光流传感器数据: flow_x:%d, flow_y:%d, intergration_time:%u, distance:%u\n", current_flow.flow_x, current_flow.flow_y, current_flow.integration_time, current_flow.distance);

  // int16_t raw_flow_x = (int16_t)((buffer[3] << 8) | buffer[2]);
  // int16_t raw_flow_y = (int16_t)((buffer[5] << 8) | buffer[4]);
  // uint16_t raw_distance = (uint16_t)((buffer[9] << 8) | buffer[8]);

  // last_stable_flow_x = (3*last_stable_flow_x + raw_flow_x) >> 2;
  // last_stable_flow_y = (3*last_stable_flow_y + raw_flow_y) >> 2;
  // last_stable_distance = (3*last_stable_distance + raw_distance) >> 2;

  // current_flow.flow_x = last_stable_flow_x;
  // current_flow.flow_y = last_stable_flow_y;
  // current_flow.distance = last_stable_distance;
}

// 计算实际位置和速度函数
void calculatePV() {
  if (current_flow.valid == 0xF5) { // 当光流数据可用时(0xF5)
    // 根据文档说明, 实际位移(mm) = 数据流数据/10000*高度(mm); 以激光测距得到的距离作为高度
    double flow_x_actual = current_flow.flow_x / 10000.0f * current_flow.distance;
    double flow_y_actual = current_flow.flow_y / 10000.0f * current_flow.distance;
    // 累加位置变化算出小车的实际位置
    car_status.posX += flow_x_actual;
    car_status.posY += flow_y_actual;

    dx_optical = flow_y_actual; // 记录竖直方向上的一段位移

    // 实际位移/间隔时间 = 实际速度
    car_status.speedX = flow_x_actual / (current_flow.integration_time/1000000.0); // 注意将微妙us转为秒s
    car_status.speedY = flow_y_actual / (current_flow.integration_time/1000000.0); // 注意将微妙us转为秒s
    // 速度限幅
    car_status.speedX = constrain(car_status.speedX, -1000.0f, 1000.0f);
    car_status.speedY = constrain(car_status.speedY, -1000.0f, 1000.0f);
  }
}
/*------------------*/