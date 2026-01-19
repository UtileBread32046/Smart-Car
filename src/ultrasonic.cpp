#include "ultrasonic.h"
#include "main_car.h"

// 超声波初始化
void init_ultrasonic() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

// 超声波测距函数
void getDistance() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  long time = pulseIn(echo, HIGH, 30000); // 30ms超时限制 (单位:微秒μs), 若超过最大时长, 则返回值为0
  // 注意下方.0应当放在1000后, 否则time和1000做整数除法, 会直接省去小数精度
  double distance = time/1000.0 * 340 / 2; // 距离 = 时间(μs->ms)*速度(v声 = 340m/s)->总路程/2 (单位:mm)
  distance /= 10; // 单位: mm->cm

  car_status.distance = (distance == 0) ? 40 : distance; // 若超时, 则返回一个长距离(视为无障碍)
}