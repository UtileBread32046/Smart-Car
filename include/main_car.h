#ifndef MAIN_CAR_H
#define MAIN_CAR_H

typedef struct CarStatus {
  int maxSpeed; // 记录小车最大速度
  int finalLeft; // 小车最终左轮速度
  int finalRight; // 小车最终右轮速度
  bool isRunning; // 记录小车休眠状态
} CarStatus;

extern CarStatus car_status; // 将在.cpp文件中分配的变量导入头文件中, 供其他所有文件使用

// OLED后台任务, 使屏幕刷新逻辑在另外一个核心上运行, 保证一直开启
void OLEDTask();


#endif