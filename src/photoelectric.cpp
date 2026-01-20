#include <Arduino.h>
#include "photoelectric.h"

static unsigned long lastPhotoelectricTime = 0; // 上次光电码盘的更新时间

// 光电码盘初始化函数
void init_photoelectric() {
  // 将光电码盘的A,B引脚设置为输入模式
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
}
// 光电码盘数据更新函数
void updatePhotoelectric() {
  if (millis() - lastPhotoelectricTime > 100) {
    
  }
}