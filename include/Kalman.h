#ifndef KALMAN_H
#define KALMAN_H
#include "main_car.h"

struct KalmanFilter {
  double estimation; // 估计值
  double P; // 估计误差协方差
  double Q; // 过程噪声(预测模型的波动)
  double R; // 测量噪声(传感器波动)

  // 初始化函数
  KalmanFilter(double q, double r) : estimation(0.0), P(1.0), Q(q), R(r) {}
  // 更新函数, 输入传感器测量值, 输出最优估计值
  double updateKalman(double measurement);
};

#endif