#include "segDisplay.h"

// 初始化数码管
void init_segDisplay() {
  pinMode(SCLK, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(DIO, OUTPUT);
}

// 数码管显示函数
// 数码管亮度 = 点亮时间 / (点亮时间 + 熄灭时间) (即占总时间的占比)
void displayNum(int num) {
  // 将num的四位数字保存在数组中
  int digits[4] = {};
  digits[0] = num / 1000;
  digits[1] = num/100 % 10;
  digits[2] = num/10 % 10;
  digits[3] = num % 10;
  for (int i = 0; i < 4; i++) {
    // RCLK: 上升沿触发, 从LOW到HIGH的一瞬间传递数据
    digitalWrite(RCLK, LOW); // 准备传输数据, 先锁定当前画面
    // shiftOut(dataPin, clockPin, bitOrder, value): 将value上的数值自动转成8个二进制位(a~g, dp), 自动调整DIO和SCLK的电平
    // MSBFIRST(Most Segnificant Fisrt): 最高位优先(最左边一位优先发出去)
    shiftOut(DIO, SCLK, MSBFIRST, 0x00); // 消隐位选芯片
    shiftOut(DIO, SCLK, MSBFIRST, 0xFF); // 消隐数码管所有的段, 防止产生重影
    digitalWrite(RCLK, HIGH);

    digitalWrite(RCLK, LOW);
    // 由于芯片串联, 段码和位码数据有空间顺序限制, 此处必须先传位码, 再传段码
    shiftOut(DIO, SCLK, MSBFIRST, posCode[i]); // 位码, 选择显示的位置
    shiftOut(DIO, SCLK, MSBFIRST, segCode[digits[i]]); // 段码, 选择显示的数字

    digitalWrite(RCLK, HIGH); // 高电平推送数据, 刷新数码管
    delayMicroseconds(1000); // 设置扫描间隔(时间太长会闪烁, 太短会重影); microsecond:微秒μs, millisecond:毫秒ms
  }
  // 设置尾部消隐, 防止最后一位亮度过高
  digitalWrite(RCLK, LOW);
  shiftOut(DIO, SCLK, MSBFIRST, 0x00); // 位选, 关闭所有位
  shiftOut(DIO, SCLK, MSBFIRST, 0xFF); // 段选, 关闭所有段
  digitalWrite(RCLK, HIGH);
}