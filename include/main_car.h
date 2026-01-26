#ifndef MAIN_CAR_H
#define MAIN_CAR_H

enum MoveMode { // 记录小车移动模式;
  DIFF, // 差速控制
  ANGLE, // 角度朝向锁定
  TRACK // 寻迹模式
};

struct CarStatus {
  MoveMode mode = DIFF; // 记录小车默认运行状态
  bool isRunning = true; // 记录小车运行状态(默认开启)
  double lockAngle; // 记录小车闭环控制时的角度
  int maxSpeed; // 记录小车最大速度
  int finalLeft; // 小车最终左轮速度
  int finalRight; // 小车最终右轮速度
  double distance; // 记录小车距前方距离
  double posX; // 记录小车X轴方向位置(mm)
  double posY; // 记录小车Y轴方向位置(mm)
  double speedX; // 记录小车X轴方向速度(mm/s)
  double speedY; // 记录小车Y轴方向速度(mm/s)
  double finalSpeed; // 记录小车经过卡尔曼滤波后得到的最终瞬时速度
  bool trackL; // 记录小车寻迹左路状态
  bool trackC; // 记录小车寻迹中路状态
  bool trackR; // 记录小车寻迹右路状态
};

extern CarStatus car_status; // 将在.cpp文件中分配的变量导入头文件中, 供其他所有文件使用

// OLED后台任务, 使屏幕刷新逻辑在另外一个核心上运行, 保证一直开启
void OLEDTask();


#endif