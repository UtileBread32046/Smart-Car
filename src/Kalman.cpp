#include <Arduino.h>
#include "Kalman.h"
#include "main_car.h"

// unsigned long lastKalmanTime = 0; // 上一次进行卡尔曼滤波的时间
extern bool phelecValid; // 光电码盘数据有效性
extern double dx_phelec; // 光电码盘测出的位移
extern double dx_optical; // 光流传感器测出的位移
extern double refreshTime; // 刷新时间(由光流传感器主导)

KalmanFilter kf(0.01, 2.0); // 实例化卡尔曼滤波, 使用光电码盘进行校准, 置信度较高, R值较小

// 更新函数, 输入传感器测量值, 输出最优估计值
double KalmanFilter::updateKalman(double measurement) {
  P = P + Q; // 1. 预测, 累计误差程度
  double K = P / (P+R); // 2. 计算权重, 平衡误差和测量结果
  estimation = estimation + K*(measurement-estimation); // 3. 修正, 根据计算出的卡尔曼增益修正估计值
  P = (1-K)*P; // 4. 校准, 根据卡尔曼增益重新评估误差程度, 防止无限增大
  return estimation;
}

// 卡尔曼滤波处理函数
void processKalmanFilter() {
  // if (millis() - lastKalmanTime > 100) {
  // 为匹配刷新速度, 取消卡尔曼滤波的间隔时间, 完全转为由光电码盘的有效数据驱动
  if (phelecValid) { // 当光电码盘数据有效时(直线行驶)才进行滤波
      kf.updateKalman(dx_optical); // 使用光流传感器的数据先进行一次更新
      double dx_fused = kf.updateKalman(dx_phelec); // 使用光电码盘数据进行一次修正
      double dt = refreshTime / 1000.0f; // 计算出经历的时间
      double finalSpeed = dx_fused / dt;
      car_status.finalSpeed = finalSpeed; // 记录最终直线行驶的速度
      phelecValid = false; // 手动将数据有效性取反, 等待下一次有效数据
  } else {
    // car_status.finalSpeed = 0;
  }
  // lastKalmanTime = millis();
  // }
}