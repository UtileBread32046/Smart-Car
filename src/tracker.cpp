#include <Arduino.h>
#include "tracker.h"
#include "main_car.h"

unsigned long lastTrackerTime = 0;

// 初始化函数
void init_tracker() {
  pinMode(pin_L, INPUT);
  pinMode(pin_C, INPUT);
  pinMode(pin_R, INPUT);
}
// 状态更新函数
void update_tracker() {
  if (millis() - lastTrackerTime) {
    car_status.trackL = digitalRead(pin_L);
    car_status.trackC = digitalRead(pin_C);
    car_status.trackR = digitalRead(pin_R);
    lastTrackerTime = millis();
  }
}