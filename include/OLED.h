#ifndef OLED_H
#define OLED_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH 128 // OLED屏幕宽
#define OLED_HEIGHT 64 // OLED屏幕高
#define OLED_RESET -1 // OLED复位引脚 (-1: 不存在独立的硬件, 和esp32共用)

// 定义一个Adafruit_SSD1306类下的对象screen
// 构造函数中传入(屏幕宽, 屏幕高, 通信协议指针, 复位引脚)
// 在内存中开辟显存缓冲区buffer: 128*64bit = 8192bit = 1024B = 1KB RAM
extern Adafruit_SSD1306 screen;
extern unsigned long lastOLEDTime; // 记录上次OLED刷新时间
// OLED屏幕初始化
void init_OLED();
// 屏幕刷新函数
void screenDisplay(bool isRunning, double distance, int finalLeft, int finalRight);


#endif