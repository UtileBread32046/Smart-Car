#include <Arduino.h>
#include "OLED.h"
#include "icons.h"

Adafruit_SSD1306 screen(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);
unsigned long lastOLEDTime = 0; // 记录上次OLED刷新时间

// OLED屏幕初始化
void init_OLED() {
  if (!screen.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 使用3.3V形成高压驱动, 0x3C为屏幕的I2C地址
    Serial.println("SSD1306内存分配失败!");
    for(;;); // 若OLED屏幕初始化启动失败, 使用死循环拦截, 防止进入loop导致panic崩溃
  }
  screen.setRotation(1); // 设置屏幕旋转方向
  screen.clearDisplay(); // 清除缓冲区脏数据
  screen.display(); // 推送到屏幕(此时为全黑)
}

void  screenDisplay(bool isRunning, double distance, int finalLeft, int finalRight) {
  if (millis() - lastOLEDTime > 100) { // 设置刷新速率
    screen.clearDisplay(); // 清空屏幕
    
    // 在芯片内存RAM中开辟连续空间(帧缓冲区frame buffer), 调用drawBitmap修改内存数据(此时未显示到屏幕上)
    // drawBitmap(x, y, bitmap, width, height, color): 图像左上角x坐标, 左上角y坐标, 比特图像数组, 图像宽, 图像高, 颜色(点亮)
    if (!isRunning) {
      screen.drawBitmap(16, 48, icon_sleep, 32, 32, WHITE);
    } else if (distance < 20) {
      screen.drawBitmap(16, 48, icon_alert, 32, 32, WHITE);
    } else {
      if (finalLeft == 0 && finalRight == 0) {
        screen.drawBitmap(16, 48, icon_stop, 32, 32, WHITE);
      } 
      else if (finalLeft > 0) {
        if (finalRight > 0) {
          screen.drawBitmap(16, 48, icon_forward, 32, 32, WHITE);
        } else {
          screen.drawBitmap(16, 48, icon_left, 32, 32, WHITE);
        }
      }
      else {
        if (finalRight < 0) {
          screen.drawBitmap(16, 48, icon_backward, 32, 32, WHITE);
        } else {
          screen.drawBitmap(16, 48, icon_right, 32, 32, WHITE);
        }
      }
    }
    // 调用display()将缓冲区数据通过I2C总线打包发送给OLED屏幕进行显示
    screen.display();
    lastOLEDTime = millis(); // 更新OLED上次刷新时间
  }
}